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
	bool isExp;
	const float ADD_EASING_TIMER_UI = 0.05f;
	float easingTimerUI_;


public:

	/*===== メンバ関数 =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};