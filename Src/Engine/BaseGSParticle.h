#pragma once
#include "BaseDrawData.h"

class BaseGSParticle : public BaseDrawData {

protected:

	// 頂点データ関係
	XMFLOAT3 vertex;											//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;							//頂点バッファビュー

	// 定数データ関係b0
	GSConstBufferDataB0 constBufferDataB0;						//定数 行列や視点座標、カラーなど
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ

	// 定数データ用ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBV用ディスクリプタヒープ

};