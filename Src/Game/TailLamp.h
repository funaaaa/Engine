#pragma once
#include "Vec.h"
#include <memory>

class BLAS;
class PolygonMeshInstance;
class TailLampVertex;

// テールランプの形クラス。テールランプ頂点クラスから筒状のモデルを扱う。
class TailLamp {

private:

	/*===== メンバ変数 =====*/

	std::weak_ptr<BLAS> blas_;					// テールランプの形を保存するBLAS。テールランプ頂点クラス2つを参照して筒状の形状を生成する。
	std::weak_ptr<PolygonMeshInstance> ins_;	// テールランプのInstance。
	std::weak_ptr<TailLampVertex> vertexStart_;	// テールランプの筒形状の開始地点。
	std::weak_ptr<TailLampVertex> vertexEnd_;	// テールランプの筒形状の終了地点。
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