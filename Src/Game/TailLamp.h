#pragma once
#include "Vec.h"
#include <memory>

class BLAS;
class PolygonMeshInstance;
class TailLampVertex;

// テールランプの形クラス。筒状のモデルを扱う。
class TailLamp {

private:

	/*===== メンバ変数 =====*/

	std::weak_ptr<BLAS> blas_;
	std::weak_ptr<PolygonMeshInstance> ins_;
	std::weak_ptr<TailLampVertex> vertexStart_;
	std::weak_ptr<TailLampVertex> vertexEnd_;
	bool isActive_;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	TailLamp();

	// 生成処理
	void Generate(std::weak_ptr<TailLampVertex> VertexStart, std::weak_ptr<TailLampVertex> VertexEnd, int TextureColor);

	// 更新処理
	void Update();

	// ゲッタ
	bool GetIsActive() { return isActive_; }


};