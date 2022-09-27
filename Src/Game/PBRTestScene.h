#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RaytracingPipline.h"
#include <memory>

#include "OBB.h"


class Character;
class DynamicConstBuffer;
class RaytracingPipline;
class TLAS;
class RaytracingOutput;
class Sprite;
class BaseStage;
class BLAS;
class RayComputeShader;
class CharacterMgr;
class ConcentrationLineMgr;
class PolygonMeshInstance;

// ゲームシーン
class PBRTestScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;

	// デノイズAO用のパイプラインを設定。
	std::vector<RayPiplineShaderData> dAOuseShaders_;
	std::shared_ptr<RaytracingPipline> pipline_;

	std::shared_ptr<Sprite> itemFrameUI_;
	float itemFrameEasingTimer_;
	const Vec3 ITEM_FRAME_OUT_POS = Vec3(128.0f, -200.0f, 1.0f);
	const Vec3 ITEM_FRAME_IN_POS = Vec3(128.0f, 125.0f, 1.0f);

	// PBR用オブジェクト
	std::weak_ptr<BLAS> pbrSphereBlas_;
	std::weak_ptr<PolygonMeshInstance> pbrSphereIns_;

	// ライト位置用スフィア
	std::weak_ptr<BLAS> lightSphereBlas_;
	std::weak_ptr<PolygonMeshInstance> lightSphereIns_;

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

	// タイヤ痕出力用クラス
	std::shared_ptr<RaytracingOutput> tireMaskTexture_;

	// タイヤ痕出テスト用クラス
	std::shared_ptr<RaytracingOutput> tireMaskTextureOutput_;
	std::shared_ptr<RayComputeShader> tireMaskComputeShader_;
	std::shared_ptr<RayComputeShader> whiteOutComputeShader_;
	std::shared_ptr<DynamicConstBuffer> tireMaskConstBuffer_;

	// FPS表示をするか否か
	bool isDisplayFPS_;

	// 太陽の角度
	float sunAngle_;
	float sunSpeed_;




public:

	/*===== メンバ関数 =====*/

	PBRTestScene();
	void Init()override;
	void Update()override;
	void Draw()override;


};