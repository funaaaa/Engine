#include "TitleScene.h"
#include "Input.h"
#include "Pipeline.h"
#include "Engine.h"
#include "FHelper.h"
#include "GameSceneMode.h"
#include "PolygonInstanceRegister.h"
#include "RayEngine.h"
#include "Character.h"
#include "CharacterMgr.h"
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
#include "SceneTransition.h"

TitleScene::TitleScene()
{

	/*===== コンストラクタ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	titleSpritePos_ = FHelper::WindowCenterPos();
	title_.GenerateForTexture(titleSpritePos_, FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");
	titleOperation_.GenerateForTexture(titleSpritePos_, FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/titleOperation.png");

	redSprite_.GenerateForTexture(titleSpritePos_, FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/red.png");
	redSprite_.SetColor(DirectX::XMFLOAT4(1, 1, 1, 0));

	levelSprite_[0].GenerateForTexture(FHelper::WindowCenterPos(), LEVEL_SPRITE_SCALE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/level1.png");
	levelSprite_[1].GenerateForTexture(FHelper::WindowCenterPos(), LEVEL_SPRITE_SCALE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/level2.png");
	levelSprite_[2].GenerateForTexture(FHelper::WindowCenterPos(), LEVEL_SPRITE_SCALE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/level3.png");


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
	tireMaskConstBuffer_->Generate(sizeof(CharacterTireMask::TireMaskUV) * 2, L"TireMaskUV");

	// 設定
	DriftParticleMgr::Ins()->Setting();
	logoTimer_ = 0;

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
	player_ = std::make_shared<CharacterMgr>();
	player_->AddChara(static_cast<int>(Character::CHARA_ID::GHOST), false, 0, 0);
	player_->Setting();

	// 一旦サーキットステージを有効化する。
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex(), false);

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
	sinTimer_ = 0.0f;
	transitionCounter_ = 0;
	nowSelectLevel_ = 0;
	redSpriteAlpha_ = 0.0f;
	redSpriteScaleRate_ = 0.0f;
	redSpriteExpSpan_ = 0;

	// 太陽に関する変数
	sunAngle_ = 0.495f;
	sunSpeed_ = 0.0005f;

	isExp = false;
	easingTimerUI_ = 0.0f;
	titleStat_ = TITLE_STAT::TITLE;
	nextStat_ = TITLE_STAT::SELECT_LEVEL;
	transitionEasingTimer_ = 0;
	isExitSprite_ = false;
	isStatTransition_ = false;
	isFinishSceneTransition_ = false;
	isStartSceneTransition_ = false;
	isAppear_ = false;
	isReservationTransition_ = false;
	for (int index = 0; index < 3; ++index) {
		levelScaleRate_[index] = 0.5f;
		levelPos_[index] = Vec3(-1000, -1000, 0.1f);
	}
	titleSpritePos_ = FHelper::WindowCenterPos();

}

#include "Camera.h"
void TitleScene::Update()
{

	/*===== 更新処理 =====*/

	// 基本的なデバッグ機能
	bool isMoveOnly1F = false;
	ImGuiDebug(isMoveOnly1F, sunAngle_);

	// ゲームの一時停止フラグが立っていたら処理をとばす。
	if (Engine::Ins()->isStopGame_ && !isMoveOnly1F) return;



	if (!isFinishSceneTransition_) {
		SceneTransition::Ins()->Exit();

		// ロゴを一定時間表示し続ける。
		++logoTimer_;
		if (LOGO_TIMER <= logoTimer_) {
			isFinishSceneTransition_ = true;
		}
	}

	// 入力処理
	Input();

	// TLASやパイプラインを更新。
	RayEngine::Ins()->Update();

	// 乱数の種を更新。
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// ステージを更新。
	stages_[STAGE_ID::MUGEN]->Update(player_);

	// 太陽の角度を更新。
	sunAngle_ += sunSpeed_;
	if (0.0f < RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_) {

		sunAngle_ += sunSpeed_ * 50.0f;

	}
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

	// プレイヤーの位置を調整。
	player_->Update(stages_[0], false, false);

	// カメラの更新処理
	CameraUpdate();

	// タイトルのステータスごとの更新処理
	UpdateTitleStat();

	const float PARAM_A_MIN = 2.4f;
	const float PARAM_B_MIN = 1.055f;

	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ = PARAM_A_MIN;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_ = PARAM_B_MIN;

}

void TitleScene::Draw()
{

	RayEngine::Ins()->Draw();

	// タイヤ痕を書き込む。
	std::vector<CharacterTireMask::TireMaskUV> tireMaskUV;
	bool isWriteTireMask = player_->CheckTireMask(stages_[STAGE_ID::MUGEN], tireMaskUV);

	if (isWriteTireMask) {

		// UAVを書き込む。
		tireMaskConstBuffer_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), tireMaskUV.data(), sizeof(CharacterTireMask::TireMaskUV) * 2);
		tireMaskComputeShader_->Dispatch(1, 1, 1, tireMaskTexture_->GetUAVIndex(), { tireMaskConstBuffer_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTexture_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTextureOutput_->GetRaytracingOutput().Get())
			};

			Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(2, barrierToUAV);
		}

	}

	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	// タイトル画像を描画
	title_.ChangePosition(Vec3(titleSpritePos_.x_, titleSpritePos_.y_, 0.1f));
	title_.Draw();
	titleOperation_.ChangePosition(Vec3(titleSpritePos_.x_, titleSpritePos_.y_, 0.1f));
	titleOperation_.Draw();


	// レベル選択のUIを描画
	for (int index = 0; index < 3; ++index) {
		levelSprite_[index].ChangePosition(levelPos_[index]);
		levelSprite_[index].Draw();
	}
	redSprite_.Draw();

	SceneTransition::Ins()->Draw();

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

}

void TitleScene::Input()
{

	/*===== 入力処理 =====*/

	switch (titleStat_)
	{
	case TitleScene::TITLE_STAT::TITLE:
	{

		/*-- タイトル画像が出ている状態 --*/

		if (!isStatTransition_ && (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_RETURN))) {

			isStatTransition_ = true;
			isAppear_ = false;
			nextStat_ = TITLE_STAT::SELECT_LEVEL;
			transitionEasingTimer_ = 0.0f;

		}

		break;

	}
	case TitleScene::TITLE_STAT::SELECT_LEVEL:
	{

		/*-- レベル選択画像が出ている状態 --*/

		// 左が入力されたら。
		if (Input::Ins()->IsKeyTrigger(DIK_LEFT) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_DPAD_LEFT)) {

			--nowSelectLevel_;

		}
		// 右が入力されたら。
		if (Input::Ins()->IsKeyTrigger(DIK_RIGHT) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_DPAD_RIGHT)) {

			++nowSelectLevel_;

		}

		// 決定されたら。
		if (!isStatTransition_ && (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_RETURN))) {

			isStatTransition_ = true;
			isReservationTransition_ = true;
			isAppear_ = false;
			nextStat_ = TITLE_STAT::SELECT_LEVEL;
			transitionEasingTimer_ = 0.0f;

		}

		nowSelectLevel_ = static_cast<int>(FHelper::Clamp(static_cast<float>(nowSelectLevel_), LEVEL_RANGE.x_, LEVEL_RANGE.y_));

		break;

	}
	default:
		break;
	}
}

void TitleScene::CameraUpdate()
{

	/*===== カメラの更新処理 =====*/

	if (Engine::Ins()->isStopGame_) return;

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
		target = player_->GetPlayerIns().lock()->GetPos();
		eye = target + cameraVec * cameraDistance_ + Vec3(0, cameraHeight_, 0);
		up = player_->GetPlayerIns().lock()->GetUpVec();

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
		target = player_->GetPlayerIns().lock()->GetPos();

		break;

	}
	case TitleScene::CAMERA_MODE::UP_CLOSE:
	{

		/*-- 超接近カメラだったら --*/

		const float CAMERA_FOCUS = 300.0f;
		const float CAMERA_DISTANCE = 25.0f;
		const float CAMERA_BEHIND_DISTANCE = 10.0f;

		Vec3 baseEye = player_->GetPlayerIns().lock()->GetPos() + Vec3(0, 1, 0) * CAMERA_DISTANCE + player_->GetPlayerIns().lock()->GetForwardVec() * CAMERA_BEHIND_DISTANCE;
		Vec3 baseTarget = player_->GetPlayerIns().lock()->GetPos() + player_->GetPlayerIns().lock()->GetForwardVec() * CAMERA_FOCUS;
		up = player_->GetPlayerIns().lock()->GetUpVec();

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
		target = player_->GetPlayerIns().lock()->GetPos();
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
			cameraPutBeforePos_ = player_->GetPlayerIns().lock()->GetPos() + player_->GetPlayerIns().lock()->GetForwardVec() * 1000.0f + Vec3(100, 100, 0);
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

}

void TitleScene::UpdateTitleStat()
{

	/*===== タイトルのステータスごとの更新処理 =====*/

	switch (titleStat_)
	{
	case TitleScene::TITLE_STAT::TITLE:
	{

		/*-- タイトル画像を描画 --*/

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

		// 遷移中だったら。
		if (isStatTransition_) {

			// タイマーを更新してイージングさせる。
			transitionEasingTimer_ += TRANSITION_EASING_ADD;
			if (1.0f < transitionEasingTimer_) {

				// 出現中だったら
				if (isAppear_) {
					isStatTransition_ = false;
				}
				else {
					titleStat_ = nextStat_;
					isAppear_ = true;
				}

				transitionEasingTimer_ = 0.0f;

			}
			else {

				// 出現中だったら。
				if (isAppear_) {
					float easingAmount = FEasing::EaseOutQuint(transitionEasingTimer_);
					titleSpritePos_ = FHelper::WindowCenterPos() + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
				}
				else {
					float easingAmount = FEasing::EaseInQuint(transitionEasingTimer_);
					titleSpritePos_ = FHelper::WindowCenterPos() + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
				}

			}

		}

		break;

	}
	case TitleScene::TITLE_STAT::SELECT_LEVEL:
	{

		/*-- レベルを選択する画面。 --*/

		const float NOT_SELECTED_IMAGE_SIZE_RATE = 0.5f;
		const float SELECTED_IMAGE_SIZE_RATE = 1.0f;

		// サイン波で選択されているレベルのUIを拡縮させるようタイマー
		sinTimer_ += ADD_SIN_TIMER;
		float sinAmount = sinf(sinTimer_);
		float sinExpRate = sinAmount * SIN_SCALE_RATE;


		// 選択されている画像を大きくして、選択されていない画像を小さく暗くする。
		for (int index = 0; index < 3; ++index) {

			// 選択されていたら。
			if (nowSelectLevel_ == index) {
				levelScaleRate_[index] += ((sinExpRate + SELECTED_IMAGE_SIZE_RATE) - levelScaleRate_[index]) / 3.0f;
			}
			else {
				levelScaleRate_[index] += (NOT_SELECTED_IMAGE_SIZE_RATE - levelScaleRate_[index]) / 3.0f;
			}
			levelSprite_[index].ChangeScale(LEVEL_SPRITE_SCALE.x_ * levelScaleRate_[index], LEVEL_SPRITE_SCALE.y_ * levelScaleRate_[index], 1.0f);
			levelSprite_[index].SetColor(DirectX::XMFLOAT4(levelScaleRate_[index], levelScaleRate_[index], levelScaleRate_[index], levelScaleRate_[index]));

		}

		// レベルのUIのうらの赤いやつを更新する。
		redSprite_.ChangePosition(levelSprite_[nowSelectLevel_].GetPos());

		// 一定間隔で赤いやつを出すようにする。
		const int RED_SPRITE_EXP_SPAN = 12;
		++redSpriteExpSpan_;
		if (RED_SPRITE_EXP_SPAN <= redSpriteExpSpan_) {
			redSpriteExpSpan_ = 0;
			redSpriteAlpha_ = 1.0f;
			redSpriteScaleRate_ = 0.0f;
		}

		// 赤いやつのサイズをアルファを更新。
		const float SUB_REDSPRITE_ALPHA = 0.09f;
		const float SUB_REDSPRITE_SCALE_RATE = 0.09f;
		redSpriteAlpha_ = FHelper::Clamp(redSpriteAlpha_ - SUB_REDSPRITE_ALPHA, 0.0f, 1.0f);
		redSpriteScaleRate_ = FHelper::Clamp(redSpriteScaleRate_ + SUB_REDSPRITE_SCALE_RATE, 0.0f, 1.0f);
		redSprite_.SetColor(DirectX::XMFLOAT4(1, 1, 1, redSpriteAlpha_));
		redSprite_.ChangeScale(Vec3((LEVEL_SPRITE_SCALE * 1.5f).x_ * redSpriteScaleRate_, (LEVEL_SPRITE_SCALE * 1.5f).y_ * redSpriteScaleRate_, 1.0f));



		// 遷移中だったら。
		if (isStatTransition_) {

			// タイマーを更新してイージングさせる。
			transitionEasingTimer_ += TRANSITION_EASING_ADD;
			if (1.0f < transitionEasingTimer_) {

				// 出現中だったら
				if (isAppear_) {
					isStatTransition_ = false;
					transitionEasingTimer_ = 0.0f;
				}
				else {

					// シーン遷移が予約されていたら遷移する。
					if (isReservationTransition_) {

						// 遷移演出が始まっていなかったら演出をつける。
						if (!isStartSceneTransition_) {
							SceneTransition::Ins()->Appear();
							isStartSceneTransition_ = true;
						}

						// 遷移演出が終わっていたら次へ。
						if (isStartSceneTransition_ && SceneTransition::Ins()->GetIsFinish()) {

							isTransition_ = true;
							GameSceneMode::Ins()->level_ = nowSelectLevel_;

						}

					}
					else {
						transitionEasingTimer_ = 0.0f;
						titleStat_ = nextStat_;
						isAppear_ = true;
					}

				}

			}
			else {

				// 出現中だったら。
				if (isAppear_) {
					float easingAmount = FEasing::EaseOutQuint(transitionEasingTimer_);
					levelPos_[0] = LEVEL_POS[0] + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
					levelPos_[1] = LEVEL_POS[1] + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
					levelPos_[2] = LEVEL_POS[2] + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
				}
				else {
					float easingAmount = FEasing::EaseInQuint(transitionEasingTimer_);
					levelPos_[0] = LEVEL_POS[0] + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
					levelPos_[1] = LEVEL_POS[1] + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
					levelPos_[2] = LEVEL_POS[2] + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
				}

			}

		}

		break;

	}
	default:
		break;
	}

}
