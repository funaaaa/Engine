#pragma once
#include <array>
#include <memory>
#include "Vec.h"

class TailLampVertex;

// テールランプ頂点セットの配列を保持するクラス。
class TailLampVertexMgr {

private:

	/*===== メンバ変数 =====*/

	static const int TAIL_LAMP_VERTEX_SIZE = 10;	// テールランプの頂点セットの保存量(最大10頂点まで保存する。)
	std::array<std::shared_ptr<TailLampVertex>, TAIL_LAMP_VERTEX_SIZE> tailLampVertex_;
	int nowGenerateIndex_;		// 今回生成したIndex
	int prevGenerateIndex_;		// 前回生成したIndex


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	TailLampVertexMgr();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(std::array<Vec3, 4> Vertex, float ScaleOffsetRate);

	// 更新処理
	void Update();

	// ゲッタ
	int GetNowGenerateIndex() { return nowGenerateIndex_; }
	int GetPrevGenerateIndex() { return prevGenerateIndex_; }
	std::weak_ptr<TailLampVertex> GetNowGenerateVertexData() { return tailLampVertex_[nowGenerateIndex_]; }
	std::weak_ptr<TailLampVertex> GetPrevGenerateVertexData() { return tailLampVertex_[prevGenerateIndex_]; }

};