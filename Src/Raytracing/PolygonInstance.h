#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "Vec.h"

// TLASに登録するインスタンスクラス
class PolygonMeshInstance {

private:

	/*===== メンバ変数 =====*/

	UINT instanceID;								// このインスタンスのID

	DirectX::XMMATRIX worldMat;						// ワールド行列

	DirectX::XMMATRIX matTrans;
	DirectX::XMMATRIX matRot;
	DirectX::XMMATRIX scaleMat;

	Vec3 pos;

	UINT blasIndex;


public:

	/*===== メンバ関数 =====*/

	// Ins生成関数
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& InstanceID);

	// 移動(引数を加算)関数
	void AddTrans(const Vec3& Pos);
	DirectX::XMMATRIX GetTrans() { return matTrans; }

	// 移動(引数に移動)関数
	void ChangeTrans(const Vec3& Pos);

	// 回転(ラジアン、引数を加算)関数
	void AddRotate(const Vec3& Rot);
	void AddRotate(const DirectX::XMMATRIX& Rot);
	DirectX::XMMATRIX GetRotate() { return matRot; }

	// 回転(ラジアン、引数を代入)関数
	void ChangeRotate(const Vec3& Rot);
	void ChangeRotate(const DirectX::XMMATRIX& Rot);

	// 拡大(引数を加算)関数
	void AddScale(const Vec3& Scale);
	DirectX::XMMATRIX GetScale() { return scaleMat; }

	// 拡大(引数を代入)関数
	void ChangeScale(const Vec3& Scale);

	// ワールド行列を加算。
	void CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input);

	// 座標を取得。
	inline const Vec3& GetPos() { return pos; }
	// BLASインデックスを取得。
	inline const UINT& GetBLASIndex() { return blasIndex; }


private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};