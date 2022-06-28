#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RaytracingPipline.h"
#include <memory>

class Player;
class DynamicConstBuffer;
class RaytracingPipline;
class TLAS;
class RaytracingOutput;

// ゲームシーン
class GameScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	RayConstBufferData constBufferData;
	std::shared_ptr<DynamicConstBuffer> constBuffer;

	// デノイズAO用のパイプラインを設定。
	std::vector<RayPiplineShaderData> dAOuseShaders;
	std::shared_ptr<RaytracingPipline> deAOPipline;

	// SPONZAを読み込む。
	std::vector<int> sponzaInstance;

	// ライト用のスフィアを読み込む。
	int sphereBlas;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns;

	// 天球用のスフィア
	int skyDomeBlas;
	int skyDomeIns;

	// ステージ
	int stageBlas;
	int stageIns;
	int stageGrassBlas;
	int stageGrassIns;

	// ゴール
	int goalBlas;
	int goalIns;

	// ゴールとかの当たり判定用
	int goalCollisionBlas;
	int goalCollisionIns;
	int middlePointCollisionBlas;
	int middlePointCollisionIns;

	// ゴール関係
	bool isPassedMiddlePoint;
	int rapCount;
	const Vec3 GOAL_DEF_POS = Vec3(10, -30, 0);

	// TLASを生成。
	std::shared_ptr<TLAS> tlas;

	// アンビエントオクルージョン出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> aoOutput;

	// 色出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> colorOutput;

	// デノイズするライト出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> lightOutput;

	// GI出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> giOutput;

	// デノイズの結果出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput;

	// FPS表示をするか否か
	bool isDisplayFPS;


	// デバッグ用のパイプラインを切り替えるやつ。
	enum DEGU_PIPLINE_ID {
		DEF_PIPLINE,
		AO_PIPLINE,
		DENOISE_AO_PIPLINE,
	};

	// デバッグ用でノイズ画面を出すフラグ。
	DEGU_PIPLINE_ID debugPiplineID;

	// ライトが動いたか
	bool isMoveLight;

	// プレイヤー
	std::shared_ptr<Player> player;


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
	void InputImGUI(bool& IsMove);


};