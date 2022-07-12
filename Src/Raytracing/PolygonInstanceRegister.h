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

class PolygonMeshInstance;

// ポリゴンインスタンスの参照を保存するクラス
class PolygonInstanceRegister : public Singleton<PolygonInstanceRegister> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<PolygonMeshInstance>> instance;

	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDesc;

	using XMMATRIX = DirectX::XMMATRIX;


public:

	/*===== メンバ関数 =====*/

	// Insを生成する。
	int CreateInstance(const int& BlasIndex, const UINT& InstanceID);

	// 移動(引数を加算)関数
	void AddTrans(const int& Index, const float& X, const float& Y, const float Z);
	void AddTrans(const int& Index, const Vec3& Pos);

	// 移動(引数に移動)関数
	void ChangeTrans(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeTrans(const int& Index, const Vec3& Pos);
	void ChangeTrans(const int& Index, DirectX::XMMATRIX Trans);
	DirectX::XMMATRIX GetTrans(const int& Index);
	Vec3 GetPos(const int& Index);

	// 回転(ラジアン、引数を加算)関数
	void AddRotate(const int& Index, const float& X, const float& Y, const float Z);
	void AddRotate(const int& Index, const Vec3& Rot);
	void AddRotate(const int& Index, const DirectX::XMMATRIX& Rot);
	DirectX::XMMATRIX GetRotate(const int& Index);
	Vec3 GetRotVec3(const int& Index);

	// 回転(ラジアン、引数を代入)関数
	void ChangeRotate(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeRotate(const int& Index, const Vec3& Rot);
	void ChangeRotate(const int& Index, DirectX::XMMATRIX Rot);

	// 拡縮(引数を加算)関数
	void AddScale(const int& Index, const float& X, const float& Y, const float Z);
	void AddScale(const int& Index, const Vec3& Scale);
	DirectX::XMMATRIX GetScale(const int& Index);
	Vec3 GetScaleVec3(const int& Index);

	// 拡縮(引数を代入)関数
	void ChangeScale(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeScale(const int& Index, const Vec3& Scale);
	void ChangeScale(const int& Index, DirectX::XMMATRIX Scale);

	// 親行列を設定。
	void SetParentInstance(const int& Index, const int& ParentIndex);

	// BLASのIndexをかける。
	UINT GetBLASIndex(const int& Index);

	// インスタンスのワールド行列を求める。
	void CalWorldMat();

	// レジスターのDataを取得する関数。
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc.data(); };

	// レジスターのサイズを取得する関数。
	UINT GetRegisterSize() { return UINT(instance.size()); }

	// hlsl側での動きを判断する用の識別子
	enum SHADER_ID {

		DEF = 0,			// 通常のレイ
		AS = 1,			// 大気散乱用
		TEXCOLOR = 2,		// テクスチャの色をそのまま返す
		REFLECTION = 3,	// 反射させる。
		COMPLETE_REFLECTION = 4,	// 完全反射させる。
		LIGHT = 5,		// ライト用のレイ テクスチャの色をそのまま返し、シャドウとの当たり判定を行わない。
		REFRACTION = 6,	// 屈折用のレイ
		INVISIBILITY = 7,	// 不可視のオブジェクト
		DEF_GI = 8,		// 通常のレイ GIも行う。

	};

};