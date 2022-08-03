#pragma once
#include "Vec.h"

// 最初のAIを実装する際に使用する道用ウェイポイント
class FirstAIWayPoint {

public:

	/*===== メンバ変数 =====*/

	Vec3 pos_;
	int insIndex_;
	int blasIndex_;
	bool isActive_;

	const float RADIUS = 60.0f;


public:

	/*===== メンバ関数 =====*/

	FirstAIWayPoint();

	void Validation() { isActive_ = true; }
	void Nullification() { isActive_ = false; }

	// Imguiセット用 配置が完了次第削除予定
	void SetPos(Vec3 Pos) { pos_ = Pos; }
	Vec3 GetPos() { return pos_; }

};