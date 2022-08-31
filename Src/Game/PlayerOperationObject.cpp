#include "PlayerOperationObject.h"
#include "Input.h"
#include "Character.h"
#include <ostream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "FileName.h"

PlayerOperationObject::PlayerOperationObject(const int& ControllerIndex, const bool& IsWriteGhostInfo, Character* CharacterPointer)
{

	/*===== コンストラクタ =====*/

	controllerIndex_ = ControllerIndex;
	isWriteGhostInfo_ = IsWriteGhostInfo;
	character_ = CharacterPointer;

	// ゴーストが有効化されていたら。
	if (isWriteGhostInfo_) {

		// 書き込むファイル名を取得。
		DecideFileName();

		frameTimer_ = 0;

	}



}

BaseOperationObject::Operation PlayerOperationObject::Input(const BaseOperationObject::OperationInputData& InputData)
{

	/*===== 入力処理 =====*/

	BaseOperationObject::Operation operation;
	InputData;

	// 前進についての入力処理
	operation.accelerationRate_ = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if (Input::Ins()->IsKey(DIK_W)) {
		operation.accelerationRate_ = 1.0f;
	}

	// アイテムを後ろに撃つかの入力処理
	if (Input::Ins()->IsKey(DIK_O)) {
		operation.isShotBehind_ = true;
	}
	// アイテムを使用するかの入力処理
	if (Input::Ins()->IsKeyTrigger(DIK_P)) {
		operation.isUseItemTrigger_ = true;
	}
	// アイテムを使用するかの入力処理
	if (Input::Ins()->IsKeyRelease(DIK_P)) {
		operation.isUseItemRelease_ = true;
	}

	// 左右のハンドル操作についての入力処理
	operation.handleDriveRate_ = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	// デッドラインを設ける。
	if (std::fabs(operation.handleDriveRate_) < 0.1f) operation.handleDriveRate_ = 0.0f;
	if (Input::Ins()->IsKey(DIK_D)) {
		operation.handleDriveRate_ = 1.0f;
	}
	if (Input::Ins()->IsKey(DIK_A)) {
		operation.handleDriveRate_ = -1.0f;
	}

	// ドリフトについての入力処理を行う。
	operation.isDrift_ = 0.9f < Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	if (Input::Ins()->IsKey(DIK_LSHIFT)) {
		operation.isDrift_ = true;
	}

	// アイテムが入手されていたら。
	if (character_->GetIsGetItem()) {
		operation.isGetItem_ = true;
	}

	// ジャンプアクションのトリガー判定。
	if (Input::Ins()->IsKeyTrigger(DIK_SPACE) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		operation.isJumpActionTrigger_ = true;
	}

	// ゴースト情報の書き込みが有効化されていたら。
	if (isWriteGhostInfo_) {

		WriteGhostInfo(operation);

		++frameTimer_;

	}


	return operation;

}

void PlayerOperationObject::OnGoal()
{
}

void PlayerOperationObject::DecideFileName()
{

	/*===== ファイル名を決定 =====*/

	std::vector<std::string> fileNames;
	std::vector<int> fileNumbers;

	// ファイル名を読み込み。
	bool isSuccess = FFILE::GetFileNames("Resource/Game/CharaGhostData/", fileNames);

	// ファイル名の読み込みが失敗していたら。
	if (!isSuccess) assert(0);

	// 全ファイルを見る。
	for (auto& index : fileNames) {

		std::istringstream iss(index);
		std::string cutString;

		// '_'区切りで文字列を分解。
		while (std::getline(iss, cutString, '_')) {

			// .txtを除外したいので'.'を探す。
			std::istringstream commaIss(cutString);
			std::string commaCutString;
			while (std::getline(commaIss, commaCutString, '.')) {

				// 文字列が"Dev"だったら開発者ゴースト。
				//if (cutString == "Dev");

				// 文字列が数字かどうかを判定する。
				if (std::all_of(commaCutString.cbegin(), commaCutString.cend(), isdigit)) {

					// 数字に変換する。
					fileNumbers.emplace_back(std::stoi(commaCutString));

				}

			}


		}

	}

	// 取得された値の最大の数を求める。
	int maxNumber = 0;

	// 保存された数字が1以上だったら。
	if (0 < static_cast<int>(fileNumbers.size())) {

		std::sort(fileNumbers.begin(), fileNumbers.end(), std::greater<int>());

		// 最大の数字を保存。
		maxNumber = fileNumbers.front() + 1;

	}

	// パスを設定する。
	std::string directoryPath = "Resource/Game/CharaGhostData/";
	std::string ghostCode = "Dev_";
	std::string number = std::to_string(maxNumber);
	std::string extension = ".txt";
	filePath_ = directoryPath + ghostCode + number + extension;

}

void PlayerOperationObject::WriteGhostInfo(BaseOperationObject::Operation Operation)
{

	/*===== ゴースト情報の書き込み =====*/

	// 入力があったかどうかをチェック。
	if (!Operation.CheckInput()) return;

	std::ofstream wfile;

	// ファイルを開く。未生成だったらファイルを生成する。
	wfile.open(filePath_, std::ios::app);

	// 先頭にフレーム数を書き込む。
	wfile << std::to_string(frameTimer_) << " ";

	// 各情報を書き込んでいく。
	if (Operation.accelerationRate_ != 0) {

		wfile << "A:" << std::to_string(Operation.accelerationRate_) << " ";

	}
	if (Operation.handleDriveRate_ != 0) {

		wfile << "H:" << std::to_string(Operation.handleDriveRate_) << " ";

	}
	if (Operation.isDrift_) {

		wfile << "D:" << std::to_string(static_cast<int>(Operation.isDrift_)) << " ";

	}
	if (Operation.isGetItem_) {

		wfile << "G:" << std::to_string(static_cast<int>(Operation.isGetItem_)) << " ";

	}
	if (Operation.isUseItemTrigger_) {

		wfile << "UT:" << std::to_string(static_cast<int>(Operation.isUseItemTrigger_)) << " ";

	}
	if (Operation.isUseItemRelease_) {

		wfile << "UR:" << std::to_string(static_cast<int>(Operation.isUseItemRelease_)) << " ";

	}

	wfile << std::endl;


	// ファイルを閉じる。
	wfile.close();

}
