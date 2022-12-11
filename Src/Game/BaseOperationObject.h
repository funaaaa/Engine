#pragma once
#include "Vec.h"
#include "BaseItem.h"

// 操作オブジェクト基底クラス これを継承したクラスをCharacterにセットして車を動かす。
class BaseOperationObject {

protected:

	/*===== メンバ変数 =====*/


public:

	enum class OPERATION_ID {

		P1,			// プレイヤー1P
		NORMAL_AI,	// 通常のAI
		GHOST,		// ゴースト

	};

	struct Operation {

		float accelerationRate_;	// 加速の割合
		float handleDriveRate_;		// 左右のハンドル操作の割合 -1 ~ 1
		bool isDrift_;				// ドリフト状態か
		bool isDriftTrigger_;		// ドリフト状態のトリガーか
		bool isShotBehind_;			// アイテムを後ろに投げる状態か。
		bool isUseItemTrigger_;		// アイテムを使用するか。
		bool isUseItemRelease_;		// アイテムを使用するか。
		bool isGetItem_;			// アイテムを取得したかどうか

		Operation() : accelerationRate_(0), handleDriveRate_(0), isDrift_(false), isShotBehind_(false), isUseItemTrigger_(false), isUseItemRelease_(false), isGetItem_(false), isDriftTrigger_(false) {};
		bool CheckInput() { return accelerationRate_ != 0 || handleDriveRate_ != 0 || isDrift_ || isShotBehind_ || isUseItemTrigger_ || isUseItemRelease_ || isGetItem_ || isDriftTrigger_; }

	};

	// AIが操作を決定する際に必要な情報。
	struct OperationInputData
	{

		Vec3 pos_;					// 現在のキャラクターの座標
		Vec3 forwradVec_;			// キャラクターの移動方向ベクトル
		bool isHitJumpBoostGimmick_;// 段差ジャンプギミックにあたっているかどうか
		bool isPrevFrameDrift_;		// 前フレームにドリフトしていたか。
		BaseItem::ItemID hasItemID_;// 保持しているアイテムのID

		OperationInputData() :pos_({}), forwradVec_(Vec3(0, 0, 1)), isHitJumpBoostGimmick_(false), isPrevFrameDrift_(false), hasItemID_(BaseItem::ItemID::NONE) {};

	};


public:

	/*===== メンバ関数 =====*/

	virtual Operation Input(const OperationInputData& InputData) = 0;

	virtual void OnGoal() = 0;

};