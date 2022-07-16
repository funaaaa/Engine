#pragma once
#include <memory>
#include "Vec.h"

class StageObjectMgr;
class OBB;

class BaseStage {

protected:

	/*====== メンバ変数 =====*/

	std::shared_ptr<StageObjectMgr> stageObjectMgr_;	// ステージオブジェクト

public:

	// 当たり判定入力構造体
	struct ColliderInput {
		Vec3 targetPrevPos_;			// 当たり判定を行うオブジェクトの前フレームの座標
		Vec3 targetPos_;				// 当たり判定を行うオブジェクトの座標
		Vec3 targetSize_;				// 当たり判定を行うオブジェクトのサイズ
		std::weak_ptr<OBB> targetOBB_;	// 当たり判定を行うオブジェクトのOBB
		float targetRotY_;				// 当たり判定を行うオブジェクトのY軸の回転量
		int targetInsIndex_;			// 当たり判定を行うオブジェクトのインスタンスのインデックス
	};
	// 当たり判定出力構造体
	struct ColliderOutput {
		Vec3 resultPos_;			// 当たり判定結果の座標
		Vec3 forwardVec_;			// 正面ベクトル
		Vec3 upVec_;				// 上ベクトル
		bool isHitStage_;			// ステージと当たった判定
		bool isHitStageGrass_;		// ステージの草と当たった判定
		bool isHitGoal_;			// ゴールと当たった判定
		bool isHitMiddlePoint_;		// 中間地点と当たった判定
		bool isHitBoostGimmick_;	// ブーストギミックと当たった判定
		bool isHitOrnament_;		// 装飾オブジェクトと当たった判定
	};


public:

	/*===== メンバ関数 =====*/

	virtual void Setting() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual ColliderOutput Collider(ColliderInput Input) = 0;

	// ゴールの表示、非表示
	virtual void DisplayGoal() = 0;
	virtual void NonDisplayGoal() = 0;

};