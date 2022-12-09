#pragma once
#include "Singleton.h"
#include "ConstBuffers.h"
#include <memory>
#include <array>

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

	// 色出力用クラス。
	std::array<std::shared_ptr<RaytracingOutput>, 2> colorOutput_;

	// ライト出力用。
	std::array<std::shared_ptr<RaytracingOutput>, 2> lightOutput_;
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseLightOutput_;

	// デノイズマスク用。
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseMaskOutput_;

	// デノイズの結果出力用クラスをセット。
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseMixTextureOutput_;

	// デノイズをかけてエフェクトもつけた最終出力用クラス。
	std::array<std::shared_ptr<RaytracingOutput>, 2> finalOutputTexture_;

	// ブルーム用エミッシブ出力
	std::array<std::shared_ptr<RaytracingOutput>, 2> emissiveOutput_;

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