#pragma once
#include "Singleton.h"
#include "ConstBuffers.h"
#include <memory>

#include "ConstBuffers.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipeline.h"
#include "RayRootsignature.h"

class TLAS;
class RaytracingOutput;
class RaytracingPipeline;
class DynamicConstBuffer;

class RayEngine : public Singleton<RayEngine> {

public:

	/*===== メンバ変数 =====*/

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

	// デノイズAO用のパイプラインを設定。
	std::vector<RayPipelineShaderData> pipelineShaders_;
	std::shared_ptr<RaytracingPipeline> pipeline_;

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;


public:

	/*===== メンバ変数 =====*/

	void Setting();

	void SettingTLAS();

	void Update();

	void Draw();

	RayConstBufferData& GetConstBufferData() { return constBufferData_; }

};