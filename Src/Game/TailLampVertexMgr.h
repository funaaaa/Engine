#pragma once
#include <array>
#include <memory>
#include "Vec.h"

class TailLampVertex;

// テールランプ頂点管理クラス
class TailLampVertexMgr {

private:

	/*===== メンバ変数 =====*/

	static const int TAIL_LAMP_VERTEX_SIZE = 30;
	std::array<std::shared_ptr<TailLampVertex>, TAIL_LAMP_VERTEX_SIZE> tailLampVertex_;
	int prevGenerateIndex_;		// 前回生成したIndex


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	TailLampVertexMgr();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(std::array<Vec3, 4> Vertex, int TextureHandle);

	// 更新処理
	void Update();

};