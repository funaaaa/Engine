#pragma once
#include <memory>
#include <string>
#include <vector>
#include <wtypes.h>
#include "Vec.h"

class OBB;

class BaseStageObject {

public:

	/*===== メンバ変数 =====*/


	// 当たり判定ID
	enum class COLLISION_ID {
		OBB,	// OBBの判定
		MESH,	// メッシュの判定
		NONE,	// 当たり判定を行わない
	};

	// オブジェクトのID
	enum class OBJECT_ID {
		STAGE,			// ステージ
		STAGE_GRASS,	// ステージの草
		WALL,			// 壁
		GOAL,			// ゴール
		MIDDLE_POINT,	// 中間地点
		ORNAMENT,		// 装飾オブジェクト
		FLOATING_ORNAMENT,	// ふわふわ動く装飾オブジェクト
		BOOST_GIMMICK,	// 加速ギミック
		ITEM_BOX,		// アイテムボックス
		STEP_BOOST_GIMMICK,	// 段差加速ギミック
	};


protected:

	int blasIndex_;		// BLASのインデックス
	int insIndex_;		// Instanceのインデックス
	bool isActive_;		// 有効化フラグ
	COLLISION_ID collisionID_;	// 当たり判定のID
	OBJECT_ID objID_;			// オブジェクトのID
	std::shared_ptr<OBB> obb_;	// 当たり判定用OBB


public:

	/*===== メンバ関数 =====*/

	virtual void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID) = 0;
	virtual void Destroy() = 0;
	virtual void Update(const int& Timer) = 0;
	virtual void Disable(const int& TimerToActivation) = 0;

	// 有効化。
	inline void Activate() { isActive_ = true; }
	// 無効化。
	inline void Invalidate() { isActive_ = false; }

	// 表示、非表示
	void Display();
	void NonDisplay();

	// ゲッタ
	inline const int& GetBLASIndex() { return blasIndex_; }
	inline const int& GetINSTANCEIndex() { return insIndex_; }
	inline const bool& GetIsActive() { return isActive_; }
	inline std::shared_ptr<OBB> GetOBB() { return obb_; }
	inline const COLLISION_ID GetCollisionID() { return collisionID_; }
	inline const OBJECT_ID GetObjectID() { return objID_; }

	// 移動関係
	void AddTrans(const Vec3& Trans);
	void ChangeTrans(const Vec3& Trans);

	// サイズ関係
	void AddScale(const Vec3& Scale);
	void ChangeScale(const Vec3& Scale);

	// 回転関係
	void AddRotate(const Vec3& Rotate);
	void ChangeRotate(const Vec3& Rotate);

protected:

	// 基底クラスのメンバ変数の初期化
	void BasicInit(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID);

};