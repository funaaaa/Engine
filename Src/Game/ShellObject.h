#pragma once
#include "Vec.h"
#include <memory>

class OBB;

class ShellObject {

private:

	/*===== メンバ変数 =====*/

	int blasIndex_;			// BLASのインデックス
	int insIndex_;			// INSTANCEのインデックス
	Vec3 pos_;				// 座標
	Vec3 forwardVec_;		// 移動方向ベクトル
	float speed_;			// 移動速度
	bool isActive_;			// 有効化フラグ

	std::shared_ptr<OBB> obb_;	// 当たり判定用OBB

	const float SPEED = 30.0f;


public:

	/*===== メンバ関数 =====*/

	// 生成処理
	void Generate(const Vec3& Pos, const Vec3& ForwardVec);

	// 更新処理
	void Update();

	bool GetIsActive() { return isActive_; }

};