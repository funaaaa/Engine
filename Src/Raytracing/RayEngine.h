#pragma once
#include "Singleton.h"
#include "ConstBuffers.h"
#include <memory>
#include <array>

#include "ConstBuffers.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipeline.h"
#include "RayRootsignature.h"

#include "VolumeTexture.h"

class TLAS;
class RaytracingOutput;
class RaytracingPipeline;
class RayComputeShader;
class DynamicConstBuffer;

// レイトレーシング用の処理をまとめたクラス。Engineに付随する形で使用する。
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

	// フォグ出力
	std::array<std::shared_ptr<RaytracingOutput>, 2> fogOutput_;

	// デノイズAO用のパイプラインを設定。
	std::vector<RayPipelineShaderData> pipelineShaders_;
	std::shared_ptr<RaytracingPipeline> pipeline_;

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;
	int debugMesnInfoHandle_;

	// フォグ用ボリュームテクスチャ
	VolumeTextureUAV volumeTexture_;

	// 3Dノイズ書き込み用コンピュートシェーダー
	std::shared_ptr<RayComputeShader> write3DNoiseCS_;
	struct NoiseConstData {
		Vec3 worldPos_;
		float timer_;
		float windSpeed_;
		float windStrength_;
		float threshold_;
		float scale_;
		int octaves_;
		float persistence_;
		float lacunarity_;
		float pad_;
	}noiseConstData_;
	std::shared_ptr<DynamicConstBuffer> noiseConstBuffer_;

	enum FOG_MODE {
		VOLUME_FOG,
		SIMPLE_FOG,
		DEBUG_BOX,
		CUSTOM
	}fogMode_;


public:

	/*===== メンバ変数 =====*/

	void Setting();

	void SettingTLAS();

	void Update();

	void Draw();

	RayConstBufferData& GetConstBufferData() { return constBufferData_; }

private:

	// レイトレーシングを実行。
	void TraceRay();

	// デノイズコマンドを積む。
	void DenoiseCommand();

	// コピーコマンドを積む。
	void CopyCommand();

};