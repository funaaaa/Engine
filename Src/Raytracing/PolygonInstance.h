#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include "Vec.h"

// TLASに登録するインスタンスクラス
class PolygonMeshInstance {

private:

	/*===== メンバ変数 =====*/

	UINT instanceID;								// このインスタンスのID

	DirectX::XMMATRIX worldMat;						// ワールド行列

	DirectX::XMMATRIX matTrans;
	DirectX::XMMATRIX matRot;
	DirectX::XMMATRIX scaleMat_;

	bool isActive;	// このインスタンスが有効化されているかどうか。

	int childCount;	// このインスタンスと親子関係を結んでいる子供の数。

	// 親Instanceのポインタ
	std::weak_ptr<PolygonMeshInstance> parentInstance;

	Vec3 pos_;
	Vec3 rot;		// デバッグ用 ギミックを配置するときの調整用に使用したもので、この値が正確な回転量を持っている前提でデバッグしてはいけない。 (Matrixを直接代入する場合の回転量は取得できていないため)
	Vec3 scale_;

	UINT blasIndex_;


public:

	/*===== メンバ関数 =====*/

	// Ins生成関数
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& InstanceID);

	// 移動(引数を加算)関数
	void AddTrans(const Vec3& Pos);
	DirectX::XMMATRIX GetTrans() { return matTrans; }

	// 移動(引数に移動)関数
	void ChangeTrans(const Vec3& Pos);
	void ChangeTrans(const DirectX::XMMATRIX& Trans);

	// 回転(ラジアン、引数を加算)関数
	void AddRotate(const Vec3& Rot);
	void AddRotate(const DirectX::XMMATRIX& Rot);
	DirectX::XMMATRIX GetRotate() { return matRot; }

	// 回転(ラジアン、引数を代入)関数
	void ChangeRotate(const Vec3& Rot);
	void ChangeRotate(const DirectX::XMMATRIX& Rot);

	// 拡大(引数を加算)関数
	void AddScale(const Vec3& Scale);
	DirectX::XMMATRIX GetScale() { return scaleMat_; }

	// 拡大(引数を代入)関数
	void ChangeScale(const Vec3& Scale);
	void ChangeScale(const DirectX::XMMATRIX& Scale);

	// ワールド行列を加算。
	void CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input);

	// ワールド行列を取得。
	DirectX::XMMATRIX GetWorldMat();

	// 親行列を設定。
	void SetParentInstance(std::weak_ptr<PolygonMeshInstance> ParentInstance);

	// 座標を取得。
	inline const Vec3& GetPos() { return pos_; }
	inline const Vec3& GetRotVec3() { return rot; }
	inline const Vec3& GetScaleVec3() { return scale_; }

	// BLASインデックスを取得。
	inline const UINT& GetBLASIndex() { return blasIndex_; }

	// instanceを無効化する。
	void Disable();
	inline const bool GetIsActive() { return isActive; }


private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};