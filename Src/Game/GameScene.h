#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RayPipeline.h"
#include <memory>

#include "OBB.h"


class Character;
class DynamicConstBuffer;
class RaytracingOutput;
class Sprite;
class BaseStage;
class RayComputeShader;
class CharacterMgr;
class ConcentrationLineMgr;
class PolygonMeshInstance;

// ゲームシーン
class GameScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	// ライト用のスフィアを読み込む。
	int sphereBlas_;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns_;

	// 天球用のスフィア
	std::weak_ptr<BLAS> skyDomeBlas_;
	std::weak_ptr<PolygonMeshInstance> skyDomeIns_;

	// ステージ関係。
	std::vector<std::shared_ptr<BaseStage>> stages_;
	enum STAGE_ID {
		MUGEN,	// 無限型ステージ
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
	bool isGameFinish_;
	int transitionTimer;
	const int TRANSION_TIME = 180;

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


	// PBRテスト用
	std::weak_ptr<BLAS> pbrSphereBlas_;
	std::weak_ptr<PolygonMeshInstance> pbrSphereIns_;


	/*-- レース開始前の変数 --*/

	std::shared_ptr<Sprite> countDownSprite_;
	std::shared_ptr<Sprite> goSprite_;
	const Vec2 COUNT_DOWN_FONT_SIZE = Vec2(16.0f * 2.1f, 32.0f * 2.1f);
	const Vec2 GO_FONT_SIZE = Vec2(512.0f / 5.0f, 256.0f / 5.0f);
	int countDownNumber_;		// カウントダウンでの現在の番号
	int countDownStartTimer_;	// 開始前にカウントダウンが始まるまでのタイマー
	const int COUNT_DOWN_TIMER = 120;
	float countDownEasingTimer_;						// カウントダウンのUIが動く際の加算量
	const float COUNT_DOWN_EASING_TIMER = 1.0f / 30.0f;	// カウントダウンのUIが動く際の加算量 値は0.5秒で画面の半分を動かさなければ行けないため、1.0fを30Fで割っている。
	bool isCountDownExit_;		// カウントダウンの際のUIの動きで使用。 t:アルファ値を下げる f:上から真ん中
	bool isBeforeStart_;		// 開始前かどうかフラグ
	bool isCountDown_;			// カウントダウン中。

	Vec3 WINDOW_CENTER = Vec3(1280.0f / 2.0f, 720.0f / 2.0f, 0.1f);
	Vec3 COUNT_DOWN_START_POS = Vec3(1280.0f / 2.0f, 720.0f / 2.0f - 300.0f, 0.1f);


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

private:

	// ゲーム開始前の更新処理
	void UpdateCountDown();


};