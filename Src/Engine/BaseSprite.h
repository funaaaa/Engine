#pragma once
#include "BaseDrawData.h"
#include <array>

// スプライト基盤クラス
class BaseSprite : public BaseDrawData {

protected:

	/*-- メンバ変数 --*/

	// 頂点データ関係
	std::vector<Vertex> vertex_;								// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;			// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView_;							// 頂点バッファビュー

	// 定数データ関係
	ConstBufferDataB0 constBufferDataB0_;						// 定数 行列や視点座標、カラーなど
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> constBuffB0_;		// 定数バッファ

	// デバッグor当たり判定用
	Vec3 pos_;													// 座標


public:

	inline const std::vector<Vertex>& GetVertex() { return vertex_; }

};