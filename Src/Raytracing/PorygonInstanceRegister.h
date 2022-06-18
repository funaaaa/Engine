#pragma once
#include "Singleton.h"
#include "Vec.h"

#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include <wrl.h>

class PorygonMeshInstance;

// ポリゴンインスタンスの参照を保存するクラス
class PorygonInstanceRegister : public Singleton<PorygonInstanceRegister> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<PorygonMeshInstance>> instance;

	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDesc;

	using XMMATRIX = DirectX::XMMATRIX;


public:

	/*===== メンバ関数 =====*/

	// Insを生成する。
	int CreateInstance(const int& BlasIndex, const UINT& instanceID);

	// 移動(引数を加算)関数
	void AddTrans(const int& Index, const float& X, const float& Y, const float Z);
	void AddTrans(const int& Index, const Vec3& Pos);

	// 移動(引数に移動)関数
	void ChangeTrans(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeTrans(const int& Index, const Vec3& Pos);

	// 回転(ラジアン、引数を加算)関数
	void AddRotate(const int& Index, const float& X, const float& Y, const float Z);
	void AddRotate(const int& Index, const Vec3& Rot);

	// 回転(ラジアン、引数を代入)関数
	void ChangeRotate(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeRotate(const int& Index, const Vec3& Rot);

	// 拡縮(引数を加算)関数
	void AddScale(const int& Index, const float& X, const float& Y, const float Z);
	void AddScale(const int& Index, const Vec3& Scale);

	// 拡縮(引数を代入)関数
	void ChangeScale(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeScale(const int& Index, const Vec3& Scale);

	// インスタンスのワールド行列を求める。
	void CalWorldMat();

	// レジスターのDataを取得する関数。
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc.data(); };

	// レジスターのサイズを取得する関数。
	UINT GetRegisterSize() { return UINT(instance.size()); }

};