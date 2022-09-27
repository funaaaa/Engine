#pragma once
#include "Singleton.h"
#include "Vec.h"
#include "FHelper.h"

#include <DirectXMath.h>
#include <array>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

class PolygonMeshInstance;
class BLAS;

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
	std::weak_ptr<PolygonMeshInstance> CreateInstance(std::weak_ptr<BLAS> Blas_, const UINT& ShaderID, bool HaveMeshCollisionData = false);

	// インスタンスのワールド行列を求める。
	void CalWorldMat();

	// インスタンスを破棄。
	void DestroyInstance(std::weak_ptr<PolygonMeshInstance> Instance);
	void DestroyInstance(const int& Index);

	// インスタンスを表示
	void Display(std::weak_ptr<PolygonMeshInstance> Instance);
	void Display(const int& Index);

	// インスタンスを非表示
	void NonDisplay(std::weak_ptr<PolygonMeshInstance> Instance);
	void NonDisplay(const int& Index);

	// レジスターのDataを取得する関数。
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc_.data(); };

	// レジスターのサイズを取得する関数。
	UINT GetRegisterSize() { return UINT(instance_.size()); }

	// メッシュの当たり判定データを取得。
	const std::vector<FHelper::CheckHitPorygon>& GetMeshCollisionData(const int& Index);

	// hlsl側での動きを判断する用の識別子
	enum SHADER_ID {

		DEF = 0,			// 通常のレイ
		AS = 1,				// 大気散乱用
		TEXCOLOR = 2,		// テクスチャの色をそのまま返す
		LIGHT = 5,			// ライト用のレイ テクスチャの色をそのまま返し、シャドウとの当たり判定を行わない。
		REFRACTION = 6,		// 屈折用のレイ
		INVISIBILITY = 7,	// 不可視のオブジェクト
		DEF_GI = 8,			// 通常のレイ GIも行う。
		DEF_GI_TIREMASK = 9,// 通常のレイ GI タイヤ痕
		ALPHA = 10,			// 半透明 定数バッファを登録する必要有り。
		ADD = 11,			// 加算合成 定数バッファを登録する必要有り。

	};

};