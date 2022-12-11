#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include "WindowsAPI.h"

class RayRootsignature;

struct RayPipelineShaderData {

	std::string shaderPath_;					// シェーダーパス
	std::vector<LPCWSTR> rayGenEnteryPoint_;	// エントリポイント
	std::vector<LPCWSTR> missEntryPoint_;		// エントリポイント
	std::vector<LPCWSTR> hitgroupEntryPoint_;	// エントリポイント
	RayPipelineShaderData() {};
	RayPipelineShaderData(std::string ShaderPath, std::vector<LPCWSTR> RGEntry, std::vector<LPCWSTR> MSEntry, std::vector<LPCWSTR> HGEntry)
		:shaderPath_(ShaderPath), rayGenEnteryPoint_(RGEntry), missEntryPoint_(MSEntry), hitgroupEntryPoint_(HGEntry) {};

};

// レイトレーシングで使用するパイプライン
class RaytracingPipeline {

protected:

	/*===== メンバ変数 =====*/

	std::vector<RayPipelineShaderData> shaderData_;			// 使用するシェーダーを纏めた構造体
	std::vector<D3D12_SHADER_BYTECODE> shaderCode_;			// 使用するシェーダーのバイトコード
	Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject_;	// ステートオブジェクト
	std::shared_ptr<RayRootsignature> globalRootSig_;		// グローバルルートシグネチャ
	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc_;				// レイ発射時の設定
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderTable_;		// シェーダーテーブル
	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps_;
	LPCWSTR hitGroupName_;

	UINT raygenRegion_;
	UINT missRegion_;
	UINT hitgroupRecordSize_;


public:

	/*===== メンバ変数 =====*/

	// セッティング処理
	void Setting(const std::vector<RayPipelineShaderData>& InputData, int UseHitGroup, int SRVCount, int CBVCount, int UAVCount, int PayloadSize, int AttribSize, int ReflectionCount = 4);

	// シェーダーテーブルを構築
	void ConstructionShaderTable(int DispatchX = WINDOW_WIDTH, int DispatchY = WINDOW_HEIGHT);

	// HitGroupの情報を転送。
	void MapHitGroupInfo();

	// ゲッタ
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject_; }
	D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return dispatchRayDesc_; }
	std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return globalRootSig_; }

protected:

	// ヒットグループの中で最も使用データサイズが大きものを取得する。
	UINT GetLargestDataSizeInHitGroup();

	// アライメント
	UINT RoundUp(size_t Size, UINT Align) {
		return UINT(Size + Align - 1) & ~(Align - 1);
	}

	// バッファを生成。
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType, const wchar_t* Name = nullptr);

	// シェーダー識別子を書き込む。
	UINT WriteShaderIdentifier(void* Dst, const void* ShaderId);

	// RayGenerationシェーダーの数を取得。
	int GetRayGenerationCount();

	// MissShaderの数を取得。
	int GetMissCount();

};