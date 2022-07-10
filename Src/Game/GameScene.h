#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RaytracingPipline.h"
#include <memory>

#include "OBB.h"

class Player;
class DynamicConstBuffer;
class RaytracingPipline;
class TLAS;
class RaytracingOutput;
class Sprite;

// ゲームシーン
class GameScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	RayConstBufferData constBufferData;
	std::shared_ptr<DynamicConstBuffer> constBuffer;

	// デノイズAO用のパイプラインを設定。
	std::vector<RayPiplineShaderData> dAOuseShaders;
	std::shared_ptr<RaytracingPipline> pipline;

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

	// ブーストのギミック
	int boostGimmickTest;

	// 装飾のブロックのインデックス
	std::vector<int> stageOrnamentBlas;
	std::vector<int> stageOrnamentIns;

	// ゴール前でふわふわしているオブジェクトのインデックス。
	int beforeTheGoalObjectBlas;
	std::vector<std::pair<int, int>> beforeTheGoalObjectIns;
	std::vector<std::pair<Vec3, Vec3>> beforeTheGoalObjectDefPos;
	std::vector<float> beforeTheGoalObjectTimer;

	// UI関係
	std::shared_ptr<Sprite> nowRapCountSprite;
	std::shared_ptr<Sprite> maxRapCountSprite;
	std::shared_ptr<Sprite> rapSlashSprite;
	std::array<int, 11> numFontHandle;


	// ゴール関係
	bool isPassedMiddlePoint;
	int rapCount;
	const Vec3 GOAL_DEF_POS = Vec3(10, -30, 0);

	// TLASを生成。
	std::shared_ptr<TLAS> tlas;

	// アンビエントオクルージョン出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> aoOutput;
	std::shared_ptr<RaytracingOutput> denoiseAOOutput;

	// 色出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> colorOutput;

	// デノイズするライト出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> lightOutput;
	std::shared_ptr<RaytracingOutput> denoiseLightOutput;

	// GI出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> giOutput;
	std::shared_ptr<RaytracingOutput> denoiseGiOutput;

	// デノイズマスク用クラスをセット。
	std::shared_ptr<RaytracingOutput> denoiseMaskOutput;

	// デノイズの結果出力用クラスをセット。
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput;

	// FPS表示をするか否か
	bool isDisplayFPS;

	// 太陽の角度
	float sunAngle;
	float sunSpeed;

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
	void InputImGUI();

	// ギミックを生成。
	void GenerateGimmick();


};