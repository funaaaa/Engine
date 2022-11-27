#pragma once
#include "Engine.h"
#include "RayDescriptor.h"
#include "ComputeShader.h"
#include "ModelDataManager.h"
#include "FbxLoader.h"
#include "Vec.h"
#include "DynamicConstBuffer.h"
#include <DirectXMath.h>

// レイトレ用頂点構造体
struct RayVertex {

	Vec3 position_;
	Vec3 normal_;
	Vec2 uv_;
	Vec2 subUV_;

};

// ポリゴン形状を保存してあるBLASクラス
class BLAS {

private:

	/*===== メンバ変数 =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer_;	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;	// 頂点インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer_;	// 頂点インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer_;	// 頂点インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialUploadBuffer_;	// 頂点インデックスバッファ
	RayDescriptor vertexDescriptor_;		// 頂点ディスクリプタ
	RayDescriptor indexDescriptor_;			// 頂点インデックスディスクリプタ
	RayDescriptor materialDescriptor_;		// マテリアル情報用ディスクリプタ

	// マテリアル情報用定数バッファ
	ModelDataManager::Material material_;

	// BLASのインデックス。
	int blasIndex_;

	Microsoft::WRL::ComPtr<ID3D12Resource> blasBuffer_;		// BLAS用バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;	// スクラッチバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> updateBuffer_;	// 更新用バッファ

	UINT vertexCount_;						// 頂点の数
	UINT indexCount_;						// 頂点インデックスの数
	UINT vertexStride_;						// 1頂点のデータサイズ
	UINT indexStride_;						// 1頂点インデックスのデータサイズ

	Vec3 vertexMax_;							// 頂点の各成分の最大値
	Vec3 vertexMin_;							// 頂点の各成分の最小値

	std::wstring hitGroupName_;				// 使用するヒットグループの名前
	std::string modelPath_;					// 使用するモデルのパス
	std::vector<LPCWSTR> texturePath_;		// 使用するテクスチャのパス

	int modelIndex_;							// モデルのインデックス

	bool isOpaque_;							// 不透明フラグ

	bool isChangeTexture_;
	bool isChangeVertex_;

	bool isGenerate_;

	int baseTextureHandle_;				// 使用するテクスチャのハンドル
	int mapTextureHandle_;
	std::vector<int> uavHandle_;				// 使用するUAVのハンドル

	ComputeShader skinComput_;				// スキニング行列を元に頂点を書き換えるコンピュートシェーダー

	// デバッグ用
	std::vector<RayVertex> defVertex_;	// 生成した時点の頂点
	std::vector<RayVertex> vertex_;		// 現在の頂点 頂点を書き換える場合があるのでその時用
	std::vector<Vec3> vertexPos_;
	std::vector<Vec3> vertexNormal_;
	std::vector<Vec2> vertexUV_;
	std::vector<UINT> vertIndex_;


private:

	std::vector<FbxLoader::SkinComputeInput> skinComputeInput_;

public:

	enum class MAP_PARAM {
		NONE,
		NORMAL,
		SPECULAR,
		AO
	};


public:

	/*===== メンバ関数 =====*/

	BLAS();

	// 初期化処理
	void Init();

	// BLASの生成
	void GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque = false);
	void GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque = false);
	void GenerateBLASGLTF(const std::wstring& Path, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque = false);

	// BLASの更新
	void Update();

	// スキニングを計算
	void ComputeSkin();

	// アニメーションの有効化
	void InitAnimation();	// 初期化
	void PlayAnimation();	// 再生
	void StopAnimation();	// 停止

	// マテリアルの参照を取得。
	ModelDataManager::Material& GetMaterial() { return material_; }

	// マテリアルを書き換えた判定。
	void IsChangeMaterial();

	// テクスチャを追加。
	void ChangeBaseTexture(int Index);
	void ChangeMapTexture(int Index, MAP_PARAM MapParam);
	void AddUAVTex(int Index) { uavHandle_.emplace_back(Index); }

	// シェーダーレコードを書き込む。
	uint8_t* WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);

	// 各成分の長さの最大を返す。
	Vec3 GetVertexLengthMax();

	// 全ての頂点にVec3情報をかける。 重い処理なので動的には呼ばない。
	void MulVec3Vertex(Vec3 Vec);

	// 指定のUVをSUBUVに代入する。
	void AssignUV(const std::vector<RayVertex>& UV);

	// アクセッサ
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer() { return blasBuffer_; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetVertexBuffer() { return vertexBuffer_; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetIndexBuffer() { return indexBuffer_; }
	std::wstring& GetHitGroupName() { return hitGroupName_; }
	RayDescriptor& GetVertexDescriptor() { return vertexDescriptor_; }
	RayDescriptor& GetIndexDescriptor() { return indexDescriptor_; }
	const std::string& GetModelPath() { return modelPath_; }
	const std::vector<LPCWSTR>& GetTexturePath() { return texturePath_; }
	const Vec3& GetVertexMin() { return vertexMin_; }
	const Vec3& GetVertexMax() { return vertexMax_; }
	bool GetIsGenerate() { return isGenerate_; }
	int GetBlasIndex() { return blasIndex_; }

	std::vector<RayVertex> GetVertex() { return vertex_; }
	const std::vector<Vec3>& GetVertexPos() { return vertexPos_; }
	const std::vector<Vec3>& GetVertexNormal() { return vertexNormal_; }
	const std::vector<UINT>& GetVertexIndex() { return vertIndex_; }
	const std::vector<Vec2>& GetVertexUV() { return vertexUV_; }

private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size_, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// BLAS生成時に設定を取得する関数
	D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc(bool IsOpaque);

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