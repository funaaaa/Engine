#pragma once
#include "DirectXBase.h"
#include "RayDescriptor.h"
#include <DirectXMath.h>

using namespace DirectX;

// レイトレ用頂点構造体
struct RayVertex {

	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;

};

// ポリゴン形状を保存してあるBLASクラス
class PorygonMeshBlas {

private:

	/*===== メンバ変数 =====*/

	ComPtr<ID3D12Resource> vertexBuffer;	// 頂点バッファ
	ComPtr<ID3D12Resource> indexBuffer;		// 頂点インデックスバッファ
	RayDescriptor vertexDescriptor;			// 頂点ディスクリプタ
	RayDescriptor indexDescriptor;			// 頂点インデックスディスクリプタ

	ComPtr<ID3D12Resource> blasBuffer;		// BLAS用バッファ
	ComPtr<ID3D12Resource> scratchBuffer;	// スクラッチバッファ
	ComPtr<ID3D12Resource> updateBuffer;	// 更新用バッファ

	UINT vertexCount;						// 頂点の数
	UINT indexCount;						// 頂点インデックスの数
	UINT vertexStride;						// 1頂点のデータサイズ
	UINT indexStride;						// 1頂点インデックスのデータサイズ

	wstring hitGroupName;					// 使用するヒットグループの名前

	// デバッグ用
	vector<RayVertex> vertex;
	vector<UINT> vertIndex;


public:

	/*===== メンバ関数 =====*/

	// BLASの生成
	void GenerateBLASObj(const string& directryPath, const string& modelName, const wstring& hitGroupName);
	void GenerateBLASFbx(const string& directryPath, const string& modelName, const wstring& hitGroupName);

	// BLASの更新
	void Update();

	// アクセッタ
	ComPtr<ID3D12Resource> GetBLASBuffer() { return blasBuffer; }
	ComPtr<ID3D12Resource>& GetVertexBuffer() { return vertexBuffer; }
	ComPtr<ID3D12Resource>& GetIndexBuffer() { return indexBuffer; }
	wstring& GetHitGroupName() { return hitGroupName; }
	RayDescriptor& GetVertexDescriptor() { return vertexDescriptor; }
	RayDescriptor& GetIndexDescriptor() { return indexDescriptor; }

	// デバッグ用
	vector<RayVertex>& GetVertex() { return vertex; }

private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// バッファ全般を生成する処理
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// BLAS生成時に設定を取得する関数
	D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc();

	// 加速構造体の設定用関数
	void SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc);

	// 加速構造体の構築用関数
	void CreateAccelerationStructure();
};