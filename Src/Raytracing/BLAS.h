#pragma once
#include "DirectXBase.h"
#include "RayDescriptor.h"
#include "ComputeShader.h"
#include "ModelDataManager.h"
#include "FbxLoader.h"
#include "Vec.h"
#include "DynamicConstBuffer.h"
#include <DirectXMath.h>

// レイトレ用頂点構造体
struct RayVertex {

	Vec3 position;
	Vec3 normal;
	Vec2 uv;

};

// ポリゴン形状を保存してあるBLASクラス
class BLAS {

private:

	/*===== メンバ変数 =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;		// 頂点インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer;		// 頂点インデックスバッファ
	RayDescriptor vertexDescriptor;			// 頂点ディスクリプタ
	RayDescriptor indexDescriptor;			// 頂点インデックスディスクリプタ
	RayDescriptor materialDescriptor;		// マテリアル情報用ディスクリプタ

	// マテリアル情報用定数バッファ
	ModelDataManager::Material material;

	Microsoft::WRL::ComPtr<ID3D12Resource> blasBuffer;		// BLAS用バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer;	// スクラッチバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> updateBuffer;	// 更新用バッファ

	UINT vertexCount;						// 頂点の数
	UINT indexCount;						// 頂点インデックスの数
	UINT vertexStride;						// 1頂点のデータサイズ
	UINT indexStride;						// 1頂点インデックスのデータサイズ

	std::wstring hitGroupName;					// 使用するヒットグループの名前

	int modelIndex;							// モデルのインデックス

	bool isOpaque;							// 不透明フラグ

	std::vector<int> textureHandle;			// 使用するテクスチャのハンドル

	ComputeShader skinComput;				// スキニング行列を元に頂点を書き換えるコンピュートシェーダー

	// デバッグ用
	std::vector<RayVertex> vertex;
	std::vector<Vec3> vertexPos;
	std::vector<Vec3> vertexNormal;
	std::vector<UINT> vertIndex;


private:

	std::vector<FbxLoader::SkinComputeInput> skinComputeInput;


public:

	/*===== メンバ関数 =====*/

	// BLASの生成
	void GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing = false, const bool& IsOpaque = true);
	void GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	void GenerateBLASData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLASの更新
	void Update();

	// スキニングを計算
	void ComputeSkin();

	// アニメーションの有効化
	void InitAnimation();	// 初期化
	void PlayAnimation();	// 再生
	void StopAnimation();	// 停止

	// テクスチャを追加。
	inline void AddTex(const int& Index) { textureHandle.emplace_back(Index); }

	// シェーダーレコードを書き込む。
	uint8_t* WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);
	uint8_t* WriteShaderRecordSpecifyUAV(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName, const int& SpecifyIndex);

	// アクセッタ
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer() { return blasBuffer; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetVertexBuffer() { return vertexBuffer; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetIndexBuffer() { return indexBuffer; }
	std::wstring& GetHitGroupName() { return hitGroupName; }
	RayDescriptor& GetVertexDescriptor() { return vertexDescriptor; }
	RayDescriptor& GetIndexDescriptor() { return indexDescriptor; }

	// デバッグ用
	std::vector<RayVertex> GetVertex() { return vertex; }
	std::vector<Vec3> GetVertexPos() { return vertexPos; }
	std::vector<Vec3> GetVertexNormal() { return vertexNormal; }
	std::vector<UINT> GetVertexIndex() { return vertIndex; }

private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// BLAS生成時に設定を取得する関数
	D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc(const bool& IsOpaque);

	// 加速構造体の設定用関数
	void SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc);

	// 加速構造体の構築用関数
	void CreateAccelerationStructure();

	// GPUディスクリプタを書き込む。
	inline UINT WriteGPUDescriptor(void* Dst, const D3D12_GPU_DESCRIPTOR_HANDLE* Descriptor)
	{
		memcpy(Dst, Descriptor, sizeof(Descriptor));
		return static_cast<UINT>((sizeof(Descriptor)));
	}
	inline UINT WriteGPUDescriptor(void* Dst, const D3D12_GPU_VIRTUAL_ADDRESS* Descriptor)
	{
		memcpy(Dst, Descriptor, sizeof(Descriptor));
		return static_cast<UINT>((sizeof(Descriptor)));
	}
};