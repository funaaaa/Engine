#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include "Vec.h"

// TLASに登録するインスタンスクラス
class PorygonMeshInstance {

private:

	/*===== メンバ変数 =====*/

	UINT instanceID;								// このインスタンスのID

	DirectX::XMMATRIX worldMat;						// ワールド行列


public:

	/*===== メンバ関数 =====*/

	// Instance生成関数
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& HitGroupIndex, const UINT& InstanceID);

	// 移動(引数を加算)関数
	void AddTrans(D3D12_RAYTRACING_INSTANCE_DESC& Input, const Vec3& Pos);

	// 回転(ラジアン、引数を加算)関数
	void AddRotate(D3D12_RAYTRACING_INSTANCE_DESC& Input, const Vec3& Pos);


private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};