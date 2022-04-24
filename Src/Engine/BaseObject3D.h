#pragma once
#include "BaseDrawData.h"
#include "ConstBufferData.h"

class BaseObject3D : public BaseDrawData {

protected:

	/*-- メンバ変数 --*/

	// 定数b1、b2をマップ処理するかどうか
	bool isLighting;

	// 定数b1(マテリアル)を転送したかどうかのフラグ
	bool isAlreadyMapB1;

	// 頂点データ関係
	vector<Vertex> vertex;										//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;							//頂点バッファビュー

	// 頂点インデックスデータ関係
	vector<unsigned short> index;								//頂点インデックス
	ComPtr<ID3D12Resource> indexBuff;							//頂点インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView;								//頂点インデックスバッファビュー

	// 定数データ関係b0
	ConstBufferDataB0 constBufferDataB0;						//定数 行列や視点座標、カラーなど
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ

	// 定数データ関係b1
	ConstBufferDataB1 constBufferDataB1;						//定数2	マテリアルなど
	ComPtr<ID3D12Resource> constBuffB1;							//定数バッファ

	// 定数データ関係b2
	ConstBufferDataB2 constBufferDataB2;						//定数3	ライトなど
	ComPtr<ID3D12Resource> constBuffB2;							//定数バッファ

	// 追加の定数バッファ関係
	vector<ComPtr<ID3D12Resource>> constBufferData;				// 追加された定数バッファ
	vector<string> constBufferDataName;							// 追加された定数バッファの型名を保存するための配列

	// 定数データ用ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBV用ディスクリプタヒープ

public:

	// 頂点データのマップ処理
	void MapVertexData();

	// 色データを改変
	void ChangeColor(const XMFLOAT4& color) { constBufferDataB0.color = color; };

	// 定数バッファを追加
	short AddConstBuffer(const int& dataSize, const string& structName);

	// 定数バッファを転送
	bool TransData(const short& handle, const void* data,const int& dataSize, const string& structName);

};