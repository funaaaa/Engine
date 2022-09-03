#pragma once
#include "BaseOperationObject.h"
#include <string>
#include <vector>

class GhostOperationObject : public BaseOperationObject {

private:

	/*===== メンバ変数 =====*/

	std::string filePath_;
	int frameTimer_;		// 始まってからのフレームを計測する用タイマー
	int operationCurrentIndex_;	// 現在の命令のインデックス。

	enum class OPERATION_ID {
		NONE,
		ACCEL,
		HANDLE,
		DRIFT,
		GETITEM,
		USETRIGGER,
		USERELEASE,
		JUMPACTIONTRIGGER,
	};
	struct OperationElement {
		OPERATION_ID operationID_;
		float operationValue_;
		OperationElement() : operationID_(OPERATION_ID::NONE) {};
	};
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