#pragma once
#include <memory>
#include "Vec.h"
#include "ConstBuffers.h"

class StageObjectMgr;
class PolygonMeshInstance;
class OBB;
class CharacterMgr;

// ステージ基底クラス
class BaseStage {

public:

	/*====== メンバ変数 =====*/

	std::shared_ptr<StageObjectMgr> stageObjectMgr_;	// ステージオブジェクト

public:

	// 当たり判定入力構造体
	struct ColliderInput {
		Vec3 targetPrevPos_;			// 当たり判定を行うオブジェクトの前フレームの座標
		Vec3 targetPos_;				// 当たり判定を行うオブジェクトの座標
		Vec3 targetSize_;				// 当たり判定を行うオブジェクトのサイズ
		Vec3 targetUpVec_;				// 上ベクトル
		std::weak_ptr<OBB> targetOBB_;	// 当たり判定を行うオブジェクトのOBB
		std::weak_ptr<PolygonMeshInstance> targetInstance_;			// 当たり判定を行うオブジェクトのインスタンスのインデックス
		float targetRotY_;				// 当たり判定を行うオブジェクトのY軸の回転量
		int characterIndex_;			// CharacterMgr上でのキャラのIndex
		bool isInvalidateRotY_;			// 回転行列を求める際にY軸回転を無効化するフラグ
		bool isPlayer_;					// プレイヤーかどうか アイテムボックスとの判定の有無を識別するために使用する。
	};
	// 当たり判定出力構造体
	struct ColliderOutput {
		DirectX::XMMATRIX matRot_;	// 当たり判定の結果の回転行列。
		Vec3 resultPos_;			// 当たり判定結果の座標
		Vec3 forwardVec_;			// 正面ベクトル
		Vec3 upVec_;				// 上ベクトル
		Vec3 ornamentHitNormal_;	// 装飾オブジェクトの当たった面の法線
		bool isHitStage_;			// ステージと当たった判定
		bool isHitStageGrass_;		// ステージの草と当たった判定
		bool isHitGoal_;			// ゴールと当たった判定
		bool isHitMiddlePoint_;		// 中間地点と当たった判定
		bool isHitBoostGimmick_;	// ブーストギミックと当たった判定
		bool isHitOrnament_;		// 装飾オブジェクトと当たった判定
		bool isHitItemBox_;			// アイテムボックスと当たった判定
		bool isHitStepBoostGimmick_;// 段差加速ギミックと当たった判定
		bool isHitBrightnessWall_;
		bool isHitDarknessWall_;
	};


public:

	/*===== メンバ関数 =====*/

	virtual void Setting(int TireMaskIndex, bool IsBoostGimmick = true) = 0;
	virtual void Destroy() = 0;
	virtual void Update(std::weak_ptr<CharacterMgr> Character) = 0;
	virtual ColliderOutput Collider(ColliderInput Input) = 0;

};