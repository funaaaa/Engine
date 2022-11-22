#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include <memory>
#include <array>

class Character;
class PolygonMeshInstance;
class BLAS;
class BaseStage;
class RayComputeShader;
class RaytracingOutput;
class DynamicConstBuffer;

// タイトルシーン
class TitleScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	Sprite title_;
	Sprite titleOperation_;

	// レベル選択のUI
	std::array<Sprite, 3> levelSprite_;

	// レベル選択のUIの座標
	std::array<Vec3, 3> levelPos_;
	const std::array<Vec3, 3> LEVEL_POS = { Vec3(248.0f, 634.0f, 0.1f) ,Vec3(637.0f, 634.0f, 0.1f),Vec3(1022.0f, 634.0f, 0.1f) };
	std::array<float, 3> levelScaleRate_;	// Levelの画像の大きさにかけるスケールの割合。
	const Vec2 LEVEL_SPRITE_SCALE = Vec2(308.0f / 2.0f, 32.0f);
	float sinTimer_;	// レベルの画像を拡縮させるようのタイマー
	const float ADD_SIN_TIMER = 0.3f;
	const float SIN_SCALE_RATE = 0.04f;

	// レベルのUIの後ろの赤いやつ用変数
	Sprite redSprite_;
	float redSpriteScaleRate_;
	float redSpriteAlpha_;
	int redSpriteExpSpan_;

	// 遷移予約
	bool isReservationTransition_;
	bool isStartSceneTransition_;	// シーン遷移を開始させたフラグ

	// ステージ関係。
	std::vector<std::shared_ptr<BaseStage>> stages_;
	enum STAGE_ID {
		MUGEN,	// 無限型ステージ
		MAX_STAGE,	// ステージの最大数
	};
	STAGE_ID nowStageID;

	// タイヤ痕出力用クラス
	std::shared_ptr<RaytracingOutput> tireMaskTexture_;
	std::shared_ptr<RaytracingOutput> tireMaskTextureOutput_;
	std::shared_ptr<RayComputeShader> tireMaskComputeShader_;
	std::shared_ptr<RayComputeShader> whiteOutComputeShader_;
	std::shared_ptr<DynamicConstBuffer> tireMaskConstBuffer_;

	std::shared_ptr<Character> player_;

	// カメラの状態
	enum class CAMERA_MODE {
		START = 0,	// スタート時のカメラ状態
		PUT_BEFORE = 1,	// 車の正面に配置
		UP_CLOSE = 2,	// 超接近
		FROM_FRONT = 3,	// 正面から
	};
	// 各状態の時間
	const std::array<int, 4> CAMERA_TIMER = { 270, 180, 200, 200 };
	Vec3 cameraPutBeforePos_;	// カメラを正面に置く際の座標
	const float UPCLOSE_DISTANCE_MIN = 100.0f;
	const float UPCLOSE_DISTANCE_MAX = 400.0f;
	CAMERA_MODE cameraMode_;	// 現在のカメラの状態
	int transitionCounter_;		// カメラが遷移した数
	int cameraTimer_;			// カメラの切り替えに使用するタイマー
	float cameraAngle_;			// カメラを回転させるときの角度。主にスタート時のカメラで使用する。
	float cameraHeight_;		// カメラの高さのオフセット。主にスタート時のカメラで使用する。
	float cameraDistance_;		// カメラとプレイヤーの距離。主にスタート時のカメラで使用する。

	// 操作方法のUI用
	Vec3 titleSpritePos_;
	bool isExp;
	const float ADD_EASING_TIMER_UI = 0.05f;
	float easingTimerUI_;

	// 現在のステータス
	enum class TITLE_STAT {
		TITLE,			// タイトル画面
		SELECT_LEVEL,	// レベル選択画面
	};
	float SPRITE_EXIT_POS_Y = 1280.0f + 1000.0f;
	TITLE_STAT titleStat_;
	TITLE_STAT nextStat_;
	float transitionEasingTimer_;
	int nowSelectLevel_;
	Vec2 LEVEL_RANGE = Vec2(0, 2);
	const float TRANSITION_EASING_ADD = 0.05f;
	bool isExitSprite_;	// 遷移前の画像が画面外へでたかどうかのフラグ
	bool isStatTransition_;	// ステータスを遷移するかどうか。
	bool isFinishSceneTransition_;
	bool isAppear_;		// 現在スプライトがでている状態か。

	int logoTimer_;	// 起動時のみロゴを長めにだす。
	const int LOGO_TIMER = 30;


public:

	/*===== メンバ関数 =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

private:

	// 入力処理
	void Input();

	// カメラの更新処理
	void CameraUpdate();

	// タイトルのステータスごとの更新処理
	void UpdateTitleStat();

};