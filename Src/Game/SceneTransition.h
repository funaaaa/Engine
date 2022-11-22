#pragma once
#include "Vec.h"
#include "Sprite.h"
#include "Singleton.h"

class SceneTransition : public Singleton<SceneTransition> {

private:

	/*===== メンバ変数 =====*/

	Sprite titleLogo_;
	float alpha_;
	bool isAppear_;
	float easingTimer_;
	const float ADD_EASING_TIMER = 0.05f;


public:

	// セッティング
	void Setting();

	// 開始
	void Appear();

	// 終了
	void Exit();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 終了しているかフラグ
	bool GetIsFinish() { return 1.0f <= easingTimer_; }


};