#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

class RayRootsignature;

struct RayPiplineShaderData {

	std::string shaderPath;					// シェーダーパス
	std::vector<LPCWSTR> rayGenEnteryPoint;	// エントリポイント
	std::vector<LPCWSTR> missEntryPoint;	// エントリポイント
	std::vector<LPCWSTR> hitgroupEntryPoint;// エントリポイント
	RayPiplineShaderData() {};
	RayPiplineShaderData(std::string ShaderPath, std::vector<LPCWSTR> RGEntry, std::vector<LPCWSTR> MSEntry, std::vector<LPCWSTR> HGEntry)
		:shaderPath(ShaderPath), rayGenEnteryPoint(RGEntry), missEntryPoint(MSEntry), hitgroupEntryPoint(HGEntry) {};

};

// レイトレーシングで使用するパイプライン
class RaytracingPipline {

protected:

	/*===== メンバ変数 =====*/

	std::vector<RayPiplineShaderData> shaderData;			// 使用するシェーダーを纏めた構造体
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
	void Setting(const std::vector<RayPiplineShaderData>& InputData, const int& UseHitGroup, const int& SRVCount, const int& CBVCount, const int& UAVCount, const int& PayloadSize, const int& AttribSize);

	// シェーダーテーブルを構築
	void ConstructionShaderTable();

	// ゲッタ
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject; }
	D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return dispatchRayDesc; }
	std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return globalRootSig; }

protected:

	// ヒットグループの中で最も使用データサイズが大きものを取得する。
	UINT GetLargestDataSizeInHitGroup();

	// アライメント
	UINT RoundUp(size_t size, UINT align) {
		return UINT(size + align - 1) & ~(align - 1);
	}

	// バッファを生成。
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, const wchar_t* name = nullptr);

	// シェーダー識別子を書き込む。
	UINT WriteShaderIdentifier(void* dst, const void* shaderId);

	// RayGenerationシェーダーの数を取得。
	const int& GetRayGenerationCount();

	// MissShaderの数を取得。
	const int& GetMissCount();

};