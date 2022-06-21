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
	DirectX::XMMATRIX GetTrans(const int& Index);

	// 回転(ラジアン、引数を加算)関数
	void AddRotate(const int& Index, const float& X, const float& Y, const float Z);
	void AddRotate(const int& Index, const Vec3& Rot);
	DirectX::XMMATRIX GetRotate(const int& Index);

	// 回転(ラジアン、引数を代入)関数
	void ChangeRotate(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeRotate(const int& Index, const Vec3& Rot);

	// 拡縮(引数を加算)関数
	void AddScale(const int& Index, const float& X, const float& Y, const float Z);
	void AddScale(const int& Index, const Vec3& Scale);
	DirectX::XMMATRIX GetScale(const int& Index);

	// 拡縮(引数を代入)関数
	void ChangeScale(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeScale(const int& Index, const Vec3& Scale);

	// インスタンスのワールド行列を求める。
	void CalWorldMat();

	// レジスターのDataを取得する関数。
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc.data(); };

	// レジスターのサイズを取得する関数。
	UINT GetRegisterSize() { return UINT(instance.size()); }

	// hlsl側での動きを判断する用の識別子
	enum{
	
		SHADER_ID_DEF = 0,			// 通常のレイ
		SHADER_ID_AS = 1,			// 大気散乱用
		SHADER_ID_TEXCOLOR = 2,		// テクスチャの色をそのまま返す
		SHADER_ID_REFLECTION = 3,	// 反射させる。
		SHADER_ID_COMPLETE_REFLECTION = 4,	// 完全反射させる。

	};

};