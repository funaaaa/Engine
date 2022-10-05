#pragma once
#include "Singleton.h"
#include <memory>
#include <vector>
#include <string>
#include <wtypes.h>

class DynamicConstBuffer;
class RayPipeline;
class TLAS;
class RaytracingOutput;
struct RayPipelineShaderData;
struct RayConstBufferData;

class RayEngine : public Singleton<RayEngine> {

private:

	/*===== メンバ変数 =====*/

	// 定数バッファ。
	std::shared_ptr<RayConstBufferData> constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;

	// レイトレ用のパイプラインを設定。
	std::vector<std::shared_ptr<RayPipelineShaderData>> pipelineShaders_;
	std::shared_ptr<RayPipeline> pipeline_;

	// TLASを生成。
	std::shared_ptr<TLAS> tlas_;

	// AO出力用。
	std::shared_ptr<RaytracingOutput> aoOutput_;
	std::shared_ptr<RaytracingOutput> denoiseAOOutput_;

	// 色出力用クラス。
	std::shared_ptr<RaytracingOutput> colorOutput_;

	// ライト出力用。
	std::shared_ptr<RaytracingOutput> lightOutput_;
	std::shared_ptr<RaytracingOutput> denoiseLightOutput_;

	// GI出力用。
	std::shared_ptr<RaytracingOutput> giOutput_;
	std::shared_ptr<RaytracingOutput> denoiseGiOutput_;

	// デノイズマスク用。
	std::shared_ptr<RaytracingOutput> denoiseMaskOutput_;

	// デノイズの結果出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput_;

	std::string shaderName = "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl";
	std::vector<LPCWSTR> rgEntry = { L"mainRayGen" };
	std::vector<LPCWSTR> msEntry = { L"mainMS", L"shadowMS" };
	std::vector<LPCWSTR> hgEntry = { L"mainCHS", L"mainAnyHit" };


public:

	/*===== メンバ関数 =====*/

	// 事前準備処理
	void Setting();

	// TLASを設定。
	void SettingTLAS();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// レイトレワールドをクリア
	void ClearRayWorld();

	// 定数バッファを取得。
	std::weak_ptr<RayConstBufferData> GetConstBufferPtr() { return constBufferData_; }

};