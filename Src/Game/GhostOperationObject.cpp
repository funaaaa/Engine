#include "GhostOperationObject.h"
#include <fstream>
#include <iostream>
#include <sstream>

GhostOperationObject::GhostOperationObject(std::string GhostFilePath)
{

	/*===== コンストラクタ =====*/

	filePath_ = GhostFilePath;
	frameTimer_ = 0;
	operationCurrentIndex_ = 0;

	// 命令を読み取る。

	// ファイルを開く。
	std::ifstream file(filePath_);

	// 失敗を検出。
	if (!file) {
		assert(0);
	}

	// 1行を読み取る。
	std::string cutLine;
	while (std::getline(file, cutLine, '\n')) {

		bool isCurrentFrame = false;

		// 読み込んだ1行の先頭にある数字をフレーム数と比較する。
		std::istringstream line(cutLine);
		std::string cutElement;
		OperationData operationData;
		while (std::getline(line, cutElement, ' ')) {

			// ここまでで文字列を命令単位で分割することに成功した。
			std::istringstream operationElement(cutElement);
			std::string operationString;
			OperationElement operationElementData;
			while (std::getline(operationElement, operationString, ':')) {

				// Aだったらアクセル。
				if (operationString == "A") {
					operationElementData.operationID_ = OPERATION_ID::ACCEL;
					continue;
				}
				// Hだったらハンドル操作。
				if (operationString == "H") {
					operationElementData.operationID_ = OPERATION_ID::HANDLE;
					continue;
				}
				// Dだったらドリフト。
				if (operationString == "D") {
					operationElementData.operationID_ = OPERATION_ID::DRIFT;
					continue;
				}
				// Gだったらアイテム獲得。
				if (operationString == "G") {
					operationElementData.operationID_ = OPERATION_ID::GETITEM;
					continue;
				}
				// UTだったらアイテム使用トリガー。
				if (operationString == "UT") {
					operationElementData.operationID_ = OPERATION_ID::USETRIGGER;
					continue;
				}
				// URだったらアイテム使用リリース。
				if (operationString == "UR") {
					operationElementData.operationID_ = OPERATION_ID::USERELEASE;
					continue;
				}
				// JTだったらジャンプアクショントリガーリリース。
				if (operationString == "JT") {
					operationElementData.operationID_ = OPERATION_ID::JUMPACTIONTRIGGER;
					continue;
				}

				// operationElementData.operationID_がNONEなのにここまで来たとしたらそれはフレーム数なのでフレーム数を保存する。
				if (operationElementData.operationID_ == OPERATION_ID::NONE) {
					operationData.frameTimer_ = std::stof(operationString);
					continue;
				}

				// ここまで来たら数字なので、文字列を数字に変換する。
				operationElementData.operationValue_ = std::stof(operationString);

				// 命令を保存する。
				operationData.operation_.emplace_back(operationElementData);

			}

		}

		// 命令を保存。
		operationData_.emplace_back(operationData);

	}

	// ファイルを閉じる。
	file.close();

}

BaseOperationObject::Operation GhostOperationObject::Input(const BaseOperationObject::OperationInputData& InputData)
{

	/*===== 入力処理 =====*/

	BaseOperationObject::Operation operation;

	// 次の命令と今のフレーム数を比較する。
	if (operationData_[operationCurrentIndex_].frameTimer_ == frameTimer_) {

		for (auto& index : operationData_[operationCurrentIndex_].operation_) {

			switch (index.operationID_)
			{
			case GhostOperationObject::OPERATION_ID::NONE:
				break;
			case GhostOperationObject::OPERATION_ID::ACCEL:
				operation.accelerationRate_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::HANDLE:
				operation.handleDriveRate_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::DRIFT:
				operation.isDrift_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::GETITEM:
				operation.isGetItem_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::USETRIGGER:
				operation.isUseItemTrigger_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::USERELEASE:
				operation.isUseItemRelease_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::JUMPACTIONTRIGGER:
				operation.isJumpActionTrigger_ = index.operationValue_;
				break;
			default:
				break;
			}

		}

		// 次の命令へ
		++operationCurrentIndex_;

		// 命令インデックスが限界サイズを超えないようにする。
		if (static_cast<int>(operationData_.size()) <= operationCurrentIndex_) {

			operationCurrentIndex_ = static_cast<int>(operationData_.size()) - 1;

		}

	}

	// 経過フレームを計算。
	++frameTimer_;

	return operation;

}

void GhostOperationObject::OnGoal()
{
}
