#include "TitleScene.h"
#include "Input.h"
#include "Pipeline.h"
#include "Engine.h"
#include "FHelper.h"
#include "GameSceneMode.h"
#include "PolygonInstanceRegister.h"
#include "RayEngine.h"
#include "Character.h"
#include "PolygonInstance.h"
#include "PolygonInstanceRegister.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include "HitGroupMgr.h"
#include "MugenStage.h"
#include "BaseStage.h"
#include "RayComputeShader.h"
#include "RaytracingOutput.h"
#include "DriftParticleMgr.h"

TitleScene::TitleScene()
{

	/*===== コンストラクタ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	title_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");
	titleOperation_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/titleOperation.png");

	// ステージをセッティングする。
	stages_.emplace_back(std::make_shared<MugenStage>());

	// タイヤ痕用クラスをセット。
	tireMaskTexture_ = std::make_shared<RaytracingOutput>();
	tireMaskTexture_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskTextureOutput_ = std::make_shared<RaytracingOutput>();
	tireMaskTextureOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskComputeShader_ = std::make_shared<RayComputeShader>();
	tireMaskComputeShader_->Setting(L"Resource/ShaderFiles/RayTracing/TireMaskComputeShader.hlsl", 0, 1, 1, { tireMaskTextureOutput_->GetUAVIndex() });
	tireMaskConstBuffer_ = std::make_shared<DynamicConstBuffer>();
	tireMaskConstBuffer_->Generate(sizeof(Character::TireMaskUV) * 2, L"TireMaskUV");

	// 設定
	DriftParticleMgr::Ins()->Setting();

}

#include "TextureManager.h"

void TitleScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	// インスタンスを初期化。
	PolygonInstanceRegister::Ins()->Setting();

	// プレイヤーを生成。
	player_ = std::make_shared<Character>(Character::CHARA_ID::GHOST, 0, 0);

	// 一旦サーキットステージを有効化する。
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex());

	// 設定
	DriftParticleMgr::Ins()->Init();

	// TLASを生成。
	RayEngine::Ins()->SettingTLAS();

	// カメラの初期状態を設定。
	cameraMode_ = CAMERA_MODE::START;
	cameraTimer_ = 0;
	cameraAngle_ = 0.0f;
	cameraHeight_ = 0.0f;
	cameraDistance_ = 0.0f;
	transitionCounter_ = 0;
	isExp = false;
	easingTimerUI_ = 0.0f;

}

#include "Camera.h"
void TitleScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;
		player_->Init();

	}

	ImGui::Text("Choose your level!");

	// AIかゴーストかを選択する。
	int mode = static_cast<int>(GameSceneMode::Ins()->mode_);
	ImGui::RadioButton("GHOST", &mode, 3);
	GameSceneMode::Ins()->mode_ = static_cast<GameSceneMode::MODE>(mode);

	// AIだったら。
	if (mode == 1) {

		int level = GameSceneMode::Ins()->level_;
		ImGui::RadioButton("Level : 1", &level, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 2", &level, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 3", &level, 2);
		GameSceneMode::Ins()->level_ = level;

	}
	// GHOSTだったら。
	else if (mode == 3) {

		int level = GameSceneMode::Ins()->level_;
		ImGui::RadioButton("Level : 1", &level, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 2", &level, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 3", &level, 2);
		GameSceneMode::Ins()->level_ = level;

	}

	// TLASやパイプラインを更新。
	RayEngine::Ins()->Update();

	// 乱数の種を更新。
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// プレイヤーの位置を調整。
	player_->Update(stages_[0], false, false);

	// カメラの情報。
	Vec3& eye = Camera::Ins()->eye_;
	Vec3& target = Camera::Ins()->target_;
	Vec3& up = Camera::Ins()->up_;

	// カメラの状態によって処理を変える。
	switch (cameraMode_)
	{
	case TitleScene::CAMERA_MODE::START:
	{

		/*-- 開始時用カメラだったら --*/

		// 高さを上げて回転させる。
		const float MAX_HEIGHT = 200.0f;
		const float ADD_HEIGHT = MAX_HEIGHT / CAMERA_TIMER[static_cast<int>(TitleScene::CAMERA_MODE::START)];
		const float ADD_ANGLE = 0.03f;
		const float MAX_DISTANCE = 200.0f;
		const float ADD_DISTANCE = MAX_DISTANCE / CAMERA_TIMER[static_cast<int>(TitleScene::CAMERA_MODE::START)];;
		cameraHeight_ = FHelper::Clamp(cameraHeight_ + ADD_HEIGHT, 0.0f, MAX_HEIGHT);
		cameraAngle_ += ADD_ANGLE;
		cameraDistance_ = FHelper::Clamp(cameraDistance_ + ADD_DISTANCE, 0.0f, MAX_DISTANCE);

		// 角度をベクトルに直す。
		Vec3 cameraVec = Vec3(cosf(cameraAngle_), 0.0f, sinf(cameraAngle_));

		// カメラの位置を求める。
		target = player_->GetPos();
		eye = target + cameraVec * cameraDistance_ + Vec3(0, cameraHeight_, 0);
		up = player_->GetUpVec();

		break;

	}
	case TitleScene::CAMERA_MODE::PUT_BEFORE:
	{

		/*-- 前設置用カメラだったら --*/

		float divCount = 10.0f;
		if (transitionCounter_ == 1) {
			divCount = 30.0f;
		}

		eye += (cameraPutBeforePos_ - eye) / divCount;
		target = player_->GetPos();

		break;

	}
	case TitleScene::CAMERA_MODE::UP_CLOSE:
	{

		/*-- 超接近カメラだったら --*/

		const float CAMERA_FOCUS = 300.0f;
		const float CAMERA_DISTANCE = 25.0f;
		const float CAMERA_BEHIND_DISTANCE = 10.0f;

		Vec3 baseEye = player_->GetPos() + Vec3(0, 1, 0) * CAMERA_DISTANCE + player_->GetForwardVec() * CAMERA_BEHIND_DISTANCE;
		Vec3 baseTarget = player_->GetPos() + player_->GetForwardVec() * CAMERA_FOCUS;
		up = player_->GetUpVec();

		// 座標を補間する。
		eye += (baseEye - eye) / 10.0f;
		target += (baseTarget - target) / 10.0f;

		break;

	}
	case TitleScene::CAMERA_MODE::FROM_FRONT:
	{

		/*-- 前追尾用カメラだったら --*/

		const Vec3 CAMERA_DIR = Vec3(2.5f, 1.0f, 0).GetNormal();

		// カメラの各ステータスを設定。
		target = player_->GetPos();
		float nowDistance = UPCLOSE_DISTANCE_MIN + (UPCLOSE_DISTANCE_MAX - UPCLOSE_DISTANCE_MIN) * (static_cast<float>(cameraTimer_) / CAMERA_TIMER[static_cast<int>(cameraMode_)]);
		Vec3 baseEye = target + CAMERA_DIR * nowDistance;
		eye += (baseEye - eye) / 10.0f;
		up = Vec3(0, 1, 0);

		break;

	}
	default:
		break;
	}

	// カメラのタイマーを更新してカメラの状態を変える。
	++cameraTimer_;
	if (CAMERA_TIMER[static_cast<int>(cameraMode_)] <= cameraTimer_) {

		// 乱数でカメラを変える。
		if (cameraMode_ == CAMERA_MODE::START) {
			cameraMode_ = CAMERA_MODE::PUT_BEFORE;
		}
		else {

			// 前フレームと同じにならないようにする。
			while (1) {
				CAMERA_MODE randomMode = static_cast<CAMERA_MODE>(FHelper::GetRand(1, 3));
				if (randomMode != cameraMode_) {
					cameraMode_ = randomMode;
					break;
				}
			}
		}

		// カメラ用変数を初期化。
		cameraHeight_ = 0.0f;
		cameraAngle_ = 0.0f;
		cameraDistance_ = 0.0f;
		cameraTimer_ = 0;
		++transitionCounter_;

		// モードごとの初期化処理
		switch (cameraMode_)
		{
		case TitleScene::CAMERA_MODE::PUT_BEFORE:
			cameraPutBeforePos_ = player_->GetPos() + player_->GetForwardVec() * 1000.0f + Vec3(100, 100, 0);
			break;
		case TitleScene::CAMERA_MODE::UP_CLOSE:
			cameraDistance_ = UPCLOSE_DISTANCE_MIN;
			break;
		case TitleScene::CAMERA_MODE::FROM_FRONT:
			break;
		default:
			break;
		}

	}

	Camera::Ins()->GenerateMatView();


	// 操作方法のUIの更新処理
	easingTimerUI_ += ADD_EASING_TIMER_UI;
	if (1.0f < easingTimerUI_) {
		easingTimerUI_ = 0.0f;
		isExp = isExp ? false : true;
	}

	// イージングでアルファを変える。
	float easingValue = 0;
	if (isExp) {
		easingValue = FEasing::EaseOutQuint(easingTimerUI_);
	}
	else {
		easingValue = 1.0f - FEasing::EaseInQuint(easingTimerUI_);
	}
	titleOperation_.SetColor(DirectX::XMFLOAT4(1, 1, 1, easingValue));


}

void TitleScene::Draw()
{

	RayEngine::Ins()->Draw();

	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	title_.Draw();
	titleOperation_.Draw();

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

}