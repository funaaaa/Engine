#pragma once
#include "Singleton.h"
#include "Vec.h"

#include <DirectXMath.h>
#include <array>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

class PolygonMeshInstance;

// ポリゴンインスタンスの参照を保存するクラス
class PolygonInstanceRegister : public Singleton<PolygonInstanceRegister> {

public:

	/*===== メンバ変数 =====*/

	static const int MAX_INSTANCE = 256;	// インスタンスの最大数。


private:

	std::array<std::shared_ptr<PolygonMeshInstance>, MAX_INSTANCE> instance_;

	std::array<D3D12_RAYTRACING_INSTANCE_DESC, MAX_INSTANCE> instanceDesc_;

	using XMMATRIX = DirectX::XMMATRIX;


public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting();

	// Insを生成する。
	int CreateInstance(const int& BlasIndex, const UINT& ShaderID);

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
	Vec3 GetRotateVec3(const int& Index);

	// 回転(ラジアン、引数を代入)関数
	void ChangeRotate(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeRotate(const int& Index, const Vec3& Rot);
	void ChangeRotate(const int& Index, DirectX::XMMATRIX Rot);

	// 拡縮(引数を加算)関数
	void AddScale(const int& Index, const float& X, const float& Y, const float Z);
	void AddScale(const int& Index, const Vec3& Scale);
	DirectX::XMMATRIX GetScale(const int& Index);

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

	// 親子関係も考慮したワールド座標系での座標を取得。
	Vec3 GetWorldPos(const int& Index);

	// インスタンスを破棄。
	void DestroyInstance(const int& Index);

	// インスタンスを表示
	void Display(const int& Index);

	// インスタンスを非表示
	void NonDisplay(const int& Index);

	// レジスターのDataを取得する関数。
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc_.data(); };

	// レジスターのサイズを取得する関数。
	UINT GetRegisterSize() { return UINT(instance_.size()); }

	// hlsl側での動きを判断する用の識別子
	enum SHADER_ID {

		DEF = 0,			// 通常のレイ
		AS = 1,				// 大気散乱用
		TEXCOLOR = 2,		// テクスチャの色をそのまま返す
		REFLECTION = 3,		// 反射させる。
		COMPLETE_REFLECTION = 4,	// 完全反射させる。
		LIGHT = 5,			// ライト用のレイ テクスチャの色をそのまま返し、シャドウとの当たり判定を行わない。
		REFRACTION = 6,		// 屈折用のレイ
		INVISIBILITY = 7,	// 不可視のオブジェクト
		DEF_GI = 8,			// 通常のレイ GIも行う。
		DEF_GI_TIREMASK = 9,// 通常のレイ GI タイヤ痕
		ALPHA = 10,			// 半透明 定数バッファを登録する必要有り。

	};

};