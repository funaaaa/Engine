#pragma once
#include "BaseDrawData.h"

class BaseSprite : public BaseDrawData {

protected:

	/*-- メンバ変数 --*/

	// 頂点データ関係
	vector<Vertex> vertex;										//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;							//頂点バッファビュー

	// 定数データ関係
	ConstBufferDataB0 constBufferDataB0;						//定数 行列や視点座標、カラーなど
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBV用ディスクリプタヒープ

	// デバッグor当たり判定用
	XMFLOAT3 pos;												//座標


public:

	inline const vector<Vertex>& GetVertex() { return vertex; }

};