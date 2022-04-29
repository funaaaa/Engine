#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

class RayRootsignature;

struct RayPiplineShaderData {

	std::string shaderPath;				// シェーダーパス
	std::vector<LPCWSTR> entryPoint;	// エントリポイント
	RayPiplineShaderData() {};
	RayPiplineShaderData(std::string ShaderPath, std::vector<LPCWSTR> EntryPoint) :shaderPath(ShaderPath), entryPoint(EntryPoint) {};

};

// レイトレーシングで使用するパイプライン
class RaytracingPipline {

private:

	/*===== メンバ変数 =====*/

	std::vector<RayPiplineShaderData> shaderData;			// 使用するシェーダーを纏めた構造体
	std::vector<D3D12_SHADER_BYTECODE> shaderCode;			// 使用するシェーダーのバイトコード

	Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject;	// ステートオブジェクト

	std::shared_ptr<RayRootsignature> globalRootSig;		// グローバルルートシグネチャ


public:

	/*===== メンバ変数 =====*/

	// セッティング処理
	void Setting(const std::vector<RayPiplineShaderData>& InputData);

	// ゲッタ
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject; }

};