#pragma once
#include "Vec.h"

// 最初のAIを実装する際に使用する道用ウェイポイント
class FirstAIWayPoint {

public:

	/*===== メンバ変数 =====*/

	Vec3 pos_;			// 配置する座標
	int insIndex_;		// デバッグでウェイポイントを描画する際の描画データ
	int blasIndex_;		// デバッグでウェイポイントを描画する際のモデルデータ
	bool isActive_;		// このウェイポイントが有効化されているか。

public:

	static const int RADIUS = 500;	// ウェイポイントと車の当たり判定を行う際のサイズ


public:

	/*===== メンバ関数 =====*/

	FirstAIWayPoint();

	void Validation() { isActive_ = true; }
	void Nullification() { isActive_ = false; }

	bool GetIsActive() { return isActive_; }

	// Imguiセット用 配置が完了次第削除予定
	void SetPos(Vec3 Pos);
	Vec3 GetPos() { return pos_; }

};