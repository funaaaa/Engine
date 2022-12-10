#pragma once
#include <array>
#include <memory>
#include "Vec.h"

class TailLamp;
class TailLampVertexMgr;

// テールランプ管理クラス テールランプを生成する頂点を指定することで、頂点の保存とテールランプの生成を行う。
class TailLampMgr {

private:

	/*===== メンバ変数 =====*/

	static const int LAMP_COUNT = 10;
	std::array<std::shared_ptr<TailLamp>, LAMP_COUNT> tailLamp_;	// テールランプ本体クラス
	std::shared_ptr<TailLampVertexMgr> tailLampVertex_;				// テールランプを作成する際に参照する頂点クラス
	bool isContinuity_;			// 連続しているか
	int continuityTimer_;		// 連続している判定の時間
	const int CONTINUITY_TIMER = 4;	// 連続判定している時間。


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	TailLampMgr();

	// 初期化
	void Init();

	// 更新処理
	void Generate(std::array<Vec3, 4> Vertex, int TextureColor, float ScaleOffsetRate);

	// 更新処理
	void Update();

};