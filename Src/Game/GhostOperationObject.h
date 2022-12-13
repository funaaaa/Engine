#pragma once
#include "BaseOperationObject.h"
#include <string>
#include <vector>

// ゴーストの操作オブジェクトクラス。
class GhostOperationObject : public BaseOperationObject {

private:

	/*===== メンバ変数 =====*/

	std::string filePath_;
	int frameTimer_;		// 始まってからのフレームを計測する用タイマー
	int operationCurrentIndex_;	// 現在の命令のインデックス。

	// 命令をテキストに入出力する際に必要な識別子
	enum class OPERATION_ID {
		NONE,
		ACCEL,			// アクセルの情報です。
		HANDLE,			// ハンドルの情報です。
		DRIFT,			// ドリフトの情報です。
		GETITEM,		// Itemを取得したかの情報です。
		USETRIGGER,		// Itemを使用したトリガーかどうかの情報です。
		USERELEASE,		// Itemを使用したリリースかどうかの情報です。
		DRIFTTRIGGER,	// ドリフトのトリガーかどうかの情報です。
	};
	// 命令のIDとそのパラメーターを保存するための構造体
	struct OperationElement {
		OPERATION_ID operationID_;
		float operationValue_;
		OperationElement() : operationID_(OPERATION_ID::NONE) {};
	};
	// 命令の配列
	struct OperationData {
		std::vector<OperationElement> operation_;
		int frameTimer_;
	};
	std::vector<OperationData> operationData_;

public:

	/*===== メンバ関数 =====*/

	GhostOperationObject(std::string GhostFilePath);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;

};