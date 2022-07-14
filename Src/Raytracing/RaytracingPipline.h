#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "WindowsAPI.h"

class RayRootsignature;

struct RayPiplineShaderData {

	std::string shaderPath_;					// シェーダーパス
	std::vector<LPCWSTR> rayGenEnteryPoint;	// エントリポイント
	std::vector<LPCWSTR> missEntryPoint;	// エントリポイント
	std::vector<LPCWSTR> hitgroupEntryPoint;// エントリポイント
	RayPiplineShaderData() {};
	RayPiplineShaderData(std::string ShaderPath, std::vector<LPCWSTR> RGEntry, std::vector<LPCWSTR> MSEntry, std::vector<LPCWSTR> HGEntry)
		:shaderPath_(ShaderPath), rayGenEnteryPoint(RGEntry), missEntryPoint(MSEntry), hitgroupEntryPoint(HGEntry) {};

};

// レイトレーシングで使用するパイプライン
class RaytracingPipline {

protected:

	/*===== メンバ変数 =====*/

	std::vector<RayPiplineShaderData> shaderData_;			// 使用するシェーダーを纏めた構造体
	std::vector<D3D12_SHADER_BYTECODE> shaderCode;			// 使用するシェーダーのバイトコード
	Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject;	// ステートオブジェクト
	std::shared_ptr<RayRootsignature> globalRootSig;		// グローバルルートシグネチャ
	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc;				// レイ発射時の設定
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderTable;		// シェーダーテーブル
	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps;
	LPCWSTR hitGroupName;


public:

	/*===== メンバ変数 =====*/

	// セッティング処理
	void Setting(const std::vector<RayPiplineShaderData>& InputData, const int& UseHitGroup, const int& SRVCount, const int& CBVCount, const int& UAVCount, const int& PayloadSize, const int& AttribSize, const int& ReflectionCount = 4);

	// シェーダーテーブルを構築
	void ConstructionShaderTable(const int& DispatchX = window_width, const int& DispatchY = window_height);

	// ゲッタ
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject; }
	D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return dispatchRayDesc; }
	std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return globalRootSig; }

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