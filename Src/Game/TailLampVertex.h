#pragma once
#include "Vec.h"
#include <array>

// テールランプに使用する4頂点を保持しておくクラス。
class TailLampVertex {

private:

	/*===== メンバ変数 =====*/

	std::array<Vec3, 4> vertex_;			// 描画する4頂点
	bool isActive_;							// 有効化フラグ
	int startScaleDownTimer_;				// 縮小が開始するまでのタイマー
	const int START_SCALE_DOWN_TIMER = 30;	// 縮小が開始するまでのタイマー


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	TailLampVertex();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(std::array<Vec3, 4> Vertex, int TextureHandle);

	// 更新処理
	void Update();

	// ゲッタ
	bool GetIsActive() { return isActive_; }

};