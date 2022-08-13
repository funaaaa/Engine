#pragma once
#include "Vec.h"

// 操作オブジェクト基底クラス
class BaseOperationObject {

protected:

	/*===== メンバ変数 =====*/


public:

	enum class OPERATION_ID {

		P1,			// プレイヤー1P
		NORMAL_AI,	// 通常のAI

	};

	struct Operation {

		float accelerationRate_;	// 加速の割合
		float handleDriveRate_;		// 左右のハンドル操作の割合 -1 ~ 1
		bool isDrift_;				// ドリフト状態か
		bool isShotBehind_;			// アイテムを後ろに投げる状態か。
		bool isUseItemTrigger_;		// アイテムを使用するか。
		bool isUseItemRelease_;		// アイテムを使用するか。

		Operation() : accelerationRate_(0), handleDriveRate_(0), isDrift_(false), isShotBehind_(false), isUseItemTrigger_(false), isUseItemRelease_(false) {};
		bool CheckInput() { return accelerationRate_ != 0 || handleDriveRate_ != 0 || isDrift_ || isShotBehind_ || isUseItemTrigger_ || isUseItemRelease_; }

	};

	struct OperationInputData
	{

		Vec3 pos_;
		Vec3 forwradVec_;

	};


public:

	/*===== メンバ関数 =====*/

	virtual Operation Input(const OperationInputData& InputData) = 0;

	virtual void OnGoal() = 0;

};