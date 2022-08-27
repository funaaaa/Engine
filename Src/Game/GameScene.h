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
class RayComputeShader;
class CharacterMgr;
class ConcentrationLineMgr;

// ゲームシーン
class GameScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;

	// デノイズAO用のパイプラインを設定。
	std::vector<RayPiplineShaderData> dAOuseShaders_;
	std::shared_ptr<RaytracingPipline> pipline_;

	// SPONZAを読み込む。
	std::vector<int> sponzaInstance_;

	// ライト用のスフィアを読み込む。
	int sphereBlas_;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns_;

	// 天球用のスフィア
	int skyDomeBlas_;
	int skyDomeIns_;

	// ステージ関係。
	std::vector<std::shared_ptr<BaseStage>> stages_;
	enum STAGE_ID {
		CIRCUIT,	// サーキット
		MAX_STAGE,	// ステージの最大数
	};
	STAGE_ID nowStageID;

	// UI関係
	std::shared_ptr<Sprite> coinUI_;
	std::shared_ptr<Sprite> rapUI_;
	std::array<std::shared_ptr<Sprite>, 2> coinCountUI_;
	std::shared_ptr<Sprite> nowRapCountUI_;
	std::shared_ptr<Sprite> slashUI_;
	std::shared_ptr<Sprite> maxRapCountUI_;
	std::array<int, 11> numFontHandle_;

	std::shared_ptr<Sprite> itemFrameUI_;
	float itemFrameEasingTimer_;
	const Vec3 ITEM_FRAME_OUT_POS = Vec3(128.0f, -200.0f, 1.0f);
	const Vec3 ITEM_FRAME_IN_POS = Vec3(128.0f, 125.0f, 1.0f);

	// 集中線
	std::shared_ptr<ConcentrationLineMgr> concentrationLine_;


	// ゴール関係
	bool isPassedMiddlePoint_;
	int rapCount_;
	const Vec3 GOAL_DEF_POS = Vec3(10, -30, 0);

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

	// キャラ管理クラス
	std::shared_ptr<CharacterMgr> characterMgr_;

	int testIns_;




public:

	/*===== メンバ関数 =====*/

	GameScene();
	void Init()override;
	void Update()override;
	void Draw()override;


	// fps更新
	void FPS();

	// 入力操作
	void Input();
	void InputImGUI();

	// ギミックを生成。
	void GenerateGimmick();


};