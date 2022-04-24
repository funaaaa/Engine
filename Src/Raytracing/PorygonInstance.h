#pragma once
#include "DirectXBase.h"
#include <DirectXMath.h>

using namespace DirectX;

// TLASに登録するインスタンスクラス
class PorygonMeshInstance {

private:

	/*===== メンバ変数 =====*/

	UINT instanceID;								// このインスタンスのID
	UINT registerID;								// registerの何番目に格納されているかのID デバッグ用

	XMMATRIX transMat;								// 移動行列


public:

	/*===== メンバ関数 =====*/

	// Instance生成関数
	void CreateInstance(const ComPtr<ID3D12Resource>& blassBuffer, const UINT& hitGroupIndex, const UINT& instanceID);

	// 移動(引数を加算)関数
	void AddTrans(const float& x, const float& y, const float z);
	void AddTrans(const XMFLOAT3& pos);


private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// バッファ全般を生成する処理
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

};