#include "GameScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Character.h"
#include "FHelper.h"
#include "Camera.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipeline.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "Input.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Pipeline.h"
#include "WindowsAPI.h"
#include "MugenStage.h"
#include "RayComputeShader.h"
#include "StageObjectMgr.h"
#include "BLAS.h"
#include "ShellObjectMgr.h"
#include "CharacterMgr.h"
#include "GameSceneMode.h"
#include "DriftParticleMgr.h"
#include "ConcentrationLineMgr.h"
#include "PolygonInstance.h"
#include "BaseItem.h"
#include "BrightnessParam.h"
#include "CharacterTireMask.h"

#include "RayEngine.h"

#include "GLTF.h"
#include "SceneTransition.h"

GameScene::GameScene()
{

	/*===== 初期化処理 =====*/

	Vec3 triangleNormal = Vec3(100, 100, 0).GetNormal();
	Vec3 worldRayOrigin = Vec3(100, 100, 0);
	Vec3 worldPosition = Vec3(0, 0, 0);
	Vec3 rayDirX = Vec3(-100, -100, 0).GetNormal();

	float lengthX = Vec3(-triangleNormal).Dot(worldRayOrigin - worldPosition) / triangleNormal.Dot(rayDirX);
	Vec3 impPosX = rayDirX * lengthX + worldRayOrigin;



	// 甲羅オブジェクトをセッティング。
	ShellObjectMgr::Ins()->Setting();

	// タイヤ痕用クラスをセット。
	tireMaskTexture_ = std::make_shared<RaytracingOutput>();
	tireMaskTexture_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskTextureOutput_ = std::make_shared<RaytracingOutput>();
	tireMaskTextureOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskComputeShader_ = std::make_shared<RayComputeShader>();
	tireMaskComputeShader_->Setting(L"Resource/ShaderFiles/RayTracing/TireMaskComputeShader.hlsl", 0, 1, 1, { tireMaskTextureOutput_->GetUAVIndex() });
	tireMaskConstBuffer_ = std::make_shared<DynamicConstBuffer>();
	tireMaskConstBuffer_->Generate(sizeof(CharacterTireMask::TireMaskUV) * 2, L"TireMaskUV");

	// 白塗りコンピュートシェーダー
	whiteOutComputeShader_ = std::make_shared<RayComputeShader>();
	whiteOutComputeShader_->Setting(L"Resource/ShaderFiles/WhiteMakeUpShader.hlsl", 0, 0, 0, {});

	characterMgr_ = std::make_shared<CharacterMgr>();

	// Instanceのワールド行列を生成。
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// 設定
	DriftParticleMgr::Ins()->Setting();

	// 太陽に関する変数
	sunAngle_ = 0.495f;
	sunSpeed_ = 0.0005f;

	isDisplayFPS_ = false;

	nextScene_ = SCENE_ID::TITLE;
	isTransition_ = false;

	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;
	BrightnessParam::Ins()->isBright_ = false;


	// フォントをロード
	{
		numFontHandle_[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/0.dds");
		numFontHandle_[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/1.dds");
		numFontHandle_[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/2.dds");
		numFontHandle_[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/3.dds");
		numFontHandle_[4] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/4.dds");
		numFontHandle_[5] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/5.dds");
		numFontHandle_[6] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/6.dds");
		numFontHandle_[7] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/7.dds");
		numFontHandle_[8] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/8.dds");
		numFontHandle_[9] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/9.dds");
		numFontHandle_[10] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/slash.dds");
	}


	// スプライトを生成。
	rapUI_ = std::make_shared<Sprite>();
	rapUI_->GenerateForTexture(Vec3(140, 647, 0.1f), Vec2(224 / 2.0f, 64 / 2.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/rapUI.dds");

	nowRapCountUI_ = std::make_shared<Sprite>();
	nowRapCountUI_->GenerateSpecifyTextureID(Vec3(134, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[1]);
	slashUI_ = std::make_shared<Sprite>();
	slashUI_->GenerateSpecifyTextureID(Vec3(169, 651, 0.1f), Vec2(16.0f * 0.6f, 32.0f * 0.6f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[10]);
	maxRapCountUI_ = std::make_shared<Sprite>();
	maxRapCountUI_->GenerateSpecifyTextureID(Vec3(194, 651, 0.1f), Vec2(16.0f * 0.5f, 32.0f * 0.5f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[3]);

	// ランキングに使用するフォントをロード
	rankingFont_[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank1.png");
	rankingFont_[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank2.png");
	rankingFont_[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank3.png");
	rankingFont_[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank4.png");
	rankingUI_ = std::make_shared<Sprite>();
	rankingUI_->GenerateSpecifyTextureID(RANK_UI_POS, Vec2(64, 64), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, rankingFont_[0]);
	rankUIEasingTimer_ = 1;
	isRankUIExp_ = true;

	// 勝敗のUIを生成
	Vec3 WIN_GAMEOVER_POS = Vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.25f, 0.1f);
	winUI_ = std::make_shared<Sprite>();
	winUI_->GenerateForTexture(WIN_GAMEOVER_POS, Vec2(0, 0), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/win.png");
	gameoverUI_ = std::make_shared<Sprite>();
	gameoverUI_->GenerateForTexture(WIN_GAMEOVER_POS, Vec2(0, 0), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/gameover.png");
	gameFinishUISizeRate_ = 0.0f;
	gameFinishUIEasingTimer_ = 1;

	// 集中線
	concentrationLine_ = std::make_shared<ConcentrationLineMgr>();

	// カウントダウン用UI
	countDownSprite_ = std::make_shared<Sprite>();
	countDownSprite_->GenerateSpecifyTextureID(COUNT_DOWN_START_POS, COUNT_DOWN_FONT_SIZE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[2]);
	countDownSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));
	goSprite_ = std::make_shared<Sprite>();
	goSprite_->GenerateForTexture(WINDOW_CENTER, GO_FONT_SIZE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/go.dds");
	goSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));

	// ステージをセッティングする。
	stages_.emplace_back(std::make_shared<MugenStage>());

}

void GameScene::Init()
{

	/*===== 初期化処理 =====*/

	// インスタンスを初期化。
	BLASRegister::Ins()->Setting();
	PolygonInstanceRegister::Ins()->Setting();

	nextScene_ = SCENE_ID::TITLE;
	isTransition_ = false;

	concentrationLine_->Init();

	countDownSprite_->ChangePosition(Vec3(100000, 10000, 100));
	goSprite_->ChangePosition(Vec3(100000, 10000, 100));

	characterMgr_ = std::make_shared<CharacterMgr>();

	if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::AI) {

		// プレイヤーを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// AIを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_, 0);
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_, 1);
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_, 2);

	}
	else if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::DEF) {

		// プレイヤーを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

	}
	else if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::WRITE_GHOST) {

		// プレイヤーを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1_WGHOST), true);

	}
	else if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::GHOST) {

		// プレイヤーを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// ゴーストを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::GHOST), false, GameSceneMode::Ins()->level_);

	}

	// キャラの初期位置を設定。
	characterMgr_->Setting();

	// 一旦サーキットステージを有効化する。
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex());


	// TLASを生成。
	RayEngine::Ins()->SettingTLAS();


	Camera::Ins()->Init();

	DriftParticleMgr::Ins()->Init();

	BrightnessParam::Ins()->isBright_ = false;
	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;
	isStartTransition_ = false;
	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	sunAngle_ = 0.495f;
	isFinishTransition_ = false;
	rankUIEasingTimer_ = 1;
	isRankUIExp_ = true;

	gameFinishUISizeRate_ = 0.0f;
	gameFinishUIEasingTimer_ = 0;

	Camera::Ins()->eye_ = Vec3(0, 0, 0);
	Camera::Ins()->target_ = Vec3(10, 0, 0);


	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ = 2.4f;

}

#include <ostream>
#include <iostream>
#include <fstream>
void GameScene::Update()
{

	/*===== 更新処理 =====*/

	// シーン遷移を終わらせていなかったら終わらせる。
	if (!isFinishTransition_) {
		SceneTransition::Ins()->Exit();
		isFinishTransition_ = true;
	}

	// 入力処理
	Input();

	// キャラを更新。
	characterMgr_->Update(stages_[STAGE_ID::MUGEN], isBeforeStart_, isGameFinish_);

	// 乱数の種を更新。
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 100000);

	// カメラを更新。
	Camera::Ins()->Update(characterMgr_->GetPlayerIns().lock()->GetPos(), characterMgr_->GetPlayerIns().lock()->GetCameraForwardVec(), characterMgr_->GetPlayerIns().lock()->GetUpVec(), characterMgr_->GetPlayerIns().lock()->GetNowSpeedPer(), isBeforeStart_, isGameFinish_);

	// いずれかのキャラがゴールしていたらリザルトシーンに移動する。
	bool isGoalPlayer = false;
	if (characterMgr_->CheckGoal(isGoalPlayer)) {

		// ゴールしたトリガーだったら
		if (!isGameFinish_) {
			// スケールを変える。
			winUI_->ChangeScale(0, 0, 1.0f);
			gameoverUI_->ChangeScale(0, 0, 1.0f);
		}

		isGameFinish_ = true;
		isWin_ = isGoalPlayer;

		++transitionTimer;
		if (TRANSION_TIME < transitionTimer) {

			if (!isStartTransition_) {
				isStartTransition_ = true;
				SceneTransition::Ins()->Appear();
			}

		}

	}

	// 開始していなかったらカウントダウンの処理を行う。
	UpdateCountDown();


	// ラップ数のUIを更新。
	int rapCount = characterMgr_->GetPlayerIns().lock()->GetRapCount();
	if (3 <= rapCount) {

		rapCount = 2;

	}
	nowRapCountUI_->ChangeTextureID(numFontHandle_[rapCount + 1], 0);

	// ステージを更新。
	stages_[STAGE_ID::MUGEN]->Update(characterMgr_);

	// TLASやパイプラインを更新。
	RayEngine::Ins()->Update();

	// 太陽の角度を更新。
	sunAngle_ += sunSpeed_;
	if (0.0f < RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_) {

		sunAngle_ += sunSpeed_ * 50.0f;

	}
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

	// 甲羅を更新。
	ShellObjectMgr::Ins()->Update(stages_[STAGE_ID::MUGEN]);

	// 煙を更新する。
	DriftParticleMgr::Ins()->Update();

	// 集中線を更新。
	if (characterMgr_->GetPlayerIns().lock()->GetIdConcentrationLine()) {
		concentrationLine_->Generate();
	}
	concentrationLine_->Update();

	// シーン遷移の処理
	if (isStartTransition_ && SceneTransition::Ins()->GetIsFinish()) {
		isTransition_ = true;

		characterMgr_->Init();

	}

	// ゲームが終わっていたら
	if (isGameFinish_) {

		// UIの更新処理を行う。
		gameFinishUIEasingTimer_ += GAME_FINISH_UI_EASING_TIMER;

		// イージングの処理を行う。
		float easingAmount = FEasing::EaseInOutExpo(gameFinishUIEasingTimer_);
		if (1.0f < gameFinishUIEasingTimer_) {

			gameFinishUIEasingTimer_ = 1.0f;

		}

		// スケールを変える。
		winUI_->ChangeScale(WIN_UI_SIZE.x_, WIN_UI_SIZE.y_ * easingAmount, 1.0f);
		gameoverUI_->ChangeScale(GAMEOVER_UI_SIZE.x_, GAMEOVER_UI_SIZE.y_ * easingAmount, 1.0f);

	}


	const float PARAM_A_MIN = 2.4f;
	const float PARAM_A_MAX = 10.4f;
	const float PARAM_B_MIN = 1.055f;
	const float PARAM_B_MAX = 1.204f;

	float div = 10.0f;
	// 画面の明るさを補正。
	if (BrightnessParam::Ins()->isBright_) {
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ += (PARAM_A_MAX - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_) / div;
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_ += (PARAM_B_MAX - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_) / div;
	}
	else {
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ += (PARAM_A_MIN - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_) / div;
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_ += (PARAM_B_MIN - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_) / div;
	}

	// ランキングのUIを更新する。
	rankUIEasingTimer_ += RANK_UI_EASING_TIMER;
	if (1.0f < rankUIEasingTimer_) {

		// 拡張中だったら。
		if (isRankUIExp_) {
			rankUIEasingTimer_ = 1.0f;
		}
		else {
			rankUIEasingTimer_ = 0.0f;
			isRankUIExp_ = true;
			rankingUI_->ChangeTextureID(rankingFont_[characterMgr_->GetPlayerRanking()], 0);
		}

	}

	// スケールを更新する。
	float rankingEasingAmount = 0.0f;
	if (isRankUIExp_) {
		rankingEasingAmount = FEasing::EaseInCubic(rankUIEasingTimer_);
	}
	else {
		rankingEasingAmount = 1.0f - FEasing::EaseOutCubic(rankUIEasingTimer_);
	}
	rankingUI_->ChangeScale(Vec3(RANK_UI_SIZE.x_, RANK_UI_SIZE.y_ * rankingEasingAmount, 1.0f));
	rankingUI_->SetColor(DirectX::XMFLOAT4(1, 1, 1, rankingEasingAmount));

	// ランキングのUIが変わった瞬間だったらイージングを初期化してテクスチャを変える。
	if (characterMgr_->GetIsChangePlayerRanking()) {
		rankUIEasingTimer_ = 0;
		isRankUIExp_ = false;
	}

}

void GameScene::Draw()
{

	/*===== 描画処理 =====*/


	// レイトレーシングを実行。
	RayEngine::Ins()->Draw();


	// 床を白塗り
	static int firstTimeClean = 0;
	if (firstTimeClean == 0) {

		whiteOutComputeShader_->Dispatch(4096 / 32, 4096 / 32, 1, tireMaskTexture_->GetUAVIndex()); \
			++firstTimeClean;

	}

	// タイヤ痕を書き込む。
	std::vector<CharacterTireMask::TireMaskUV> tireMaskUV;
	bool isWriteTireMask = characterMgr_->CheckTireMask(stages_[STAGE_ID::MUGEN], tireMaskUV);

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

	// UIを描画
	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	concentrationLine_->Draw();

	// 現在のラップ数のui。
	nowRapCountUI_->Draw();
	slashUI_->Draw();
	maxRapCountUI_->Draw();

	// 左下のuiのフレーム。
	rapUI_->Draw();

	// カウントダウン用のui。
	countDownSprite_->Draw();

	// カウントダウン終了時のgoのui。
	goSprite_->Draw();

	// 勝敗を描画
	if (isGameFinish_) {

		if (isWin_) {
			winUI_->Draw();
		}
		else {
			gameoverUI_->Draw();
		}

	}

	// ランキングのUIを表示
	rankingUI_->Draw();

	// シーン遷移の画像を描画。
	SceneTransition::Ins()->Draw();

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

}

#pragma comment (lib, "winmm.lib")

void GameScene::FPS()
{

	/*===== タイトルバーにFPS表示 =====*/

	static DWORD prev_time = timeGetTime();	// 前回の時間
	static int frame_count = 0;		// フレームカウント
	DWORD now_time = timeGetTime();		// 今回のフレームの時間

	frame_count++;	// フレーム数をカウントする

	// 経過時間が１秒を超えたらカウントと時間をリセット
	if (now_time - prev_time >= 1000)
	{
		wchar_t fps[1000];
		_itow_s(frame_count, fps, 10);
		wchar_t moji[] = L"FPS";
		wcscat_s(fps, moji);
		SetWindowText(Engine::Ins()->windowsAPI_->hwnd_, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}

}

void GameScene::Input()
{

	/*===== 入力処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_START) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		if (!isStartTransition_) {
			isStartTransition_ = true;
			SceneTransition::Ins()->Appear();
		}

	}

	InputImGUI();

}

#include "BaseItem.h"
#include "Character.h"
#include "BLAS.h"
void GameScene::InputImGUI()
{

	/*===== IMGUI更新 =====*/

	ImGui::DragFloat("SunAngle", &sunAngle_, 0.005f);

}

void GameScene::UpdateCountDown()
{

	/*===== ゲーム開始前の更新処理 =====*/

	// ゲームが開始していたら。
	if (!isBeforeStart_) {

		if (countDownEasingTimer_ < 1.0f) {

			// UIを動かす用のタイマーを更新。
			countDownEasingTimer_ += COUNT_DOWN_EASING_TIMER;

		}
		// タイマーがカンストしたら。
		else {

			if (!isCountDownExit_) {

				isCountDownExit_ = true;
				countDownEasingTimer_ = 0.0f;

			}
			else {

				return;

			}

		}

		// GOが出現中だったら。
		if (!isCountDownExit_) {

			// イージング量を計算。
			float easingAmount = FEasing::EaseOutQuint(countDownEasingTimer_);

			// アルファ値を変える。
			goSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, easingAmount));

			// でかいバージョンのサイズ。
			const float BIG_SIZE = 3.0f;
			Vec3 BIG_GO_FONT_SIZE = Vec3(GO_FONT_SIZE.x_ * BIG_SIZE, GO_FONT_SIZE.y_ * BIG_SIZE, 1.0f);
			Vec3 DEF_FONT_SIZE = Vec3(GO_FONT_SIZE.x_, GO_FONT_SIZE.y_, 1.0f);

			goSprite_->ChangeScale(BIG_GO_FONT_SIZE + (DEF_FONT_SIZE - BIG_GO_FONT_SIZE) * easingAmount);

			goSprite_->ChangePosition(WINDOW_CENTER);

		}
		else {

			// イージング量を計算。
			float easingAmount = FEasing::EaseOutSine(countDownEasingTimer_);

			// イージングに使用するフォントサイズ。
			Vec3 DEF_FONT_SIZE = Vec3(GO_FONT_SIZE.x_, GO_FONT_SIZE.y_, 1.0f);
			Vec3 EXIT_SIZE = DEF_FONT_SIZE + DEF_FONT_SIZE / 5.0f;

			goSprite_->ChangeScale((DEF_FONT_SIZE + (EXIT_SIZE - DEF_FONT_SIZE) * easingAmount));

			// イージングの値を反転させる。
			easingAmount = 1.0f - easingAmount;
			goSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, easingAmount));

		}

		return;

	}

	// カウントダウンを始めるまでのタイマーを更新。
	if (!isCountDown_) {
		++countDownStartTimer_;
		if (COUNT_DOWN_TIMER < countDownStartTimer_) {

			isCountDown_ = true;

			// カウントダウンに使用する変数を初期化。
			countDownEasingTimer_ = 0;
			isCountDownExit_ = false;
			countDownNumber_ = 3;
			countDownSprite_->ChangeTextureID(numFontHandle_[countDownNumber_], 0);

		}
	}

	// カウントダウン
	if (isCountDown_) {

		// UIを動かす用のタイマーを更新。
		countDownEasingTimer_ += COUNT_DOWN_EASING_TIMER;

		// 動かし終えたら。
		if (1.0f <= countDownEasingTimer_) {

			countDownEasingTimer_ = 0;

			// 上から真ん中に来ている段階だったら、次はアルファ値を下げる。
			if (!isCountDownExit_) {

				isCountDownExit_ = true;

			}
			// アルファ値を下げる段階だったら次の数字をカウントする。
			else {

				isCountDownExit_ = false;

				--countDownNumber_;
				// カウントダウンの番号が0になったら(カウントダウンが終わったら)
				if (countDownNumber_ < 1) {

					// ゲームを開始する。
					isBeforeStart_ = false;

					// 各変数を初期化。
					isCountDownExit_ = false;
					countDownSprite_->ChangePosition(WINDOW_CENTER);


				}
				else {

					countDownSprite_->ChangeTextureID(numFontHandle_[countDownNumber_], 0);

				}

			}

		}

		// UIを更新する。
		if (isCountDownExit_) {

			// イージング量を求める。
			float easingAmount = FEasing::EaseInQuint(countDownEasingTimer_);

			// アルファ値を下げる。
			countDownSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f - easingAmount));

		}
		else {

			// イージング量を求める。
			float easingAmount = FEasing::EaseOutQuint(countDownEasingTimer_);

			// アルファ値を上げる。
			countDownSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, easingAmount));

			// 移動させる。
			countDownSprite_->ChangePosition(COUNT_DOWN_START_POS + (WINDOW_CENTER - COUNT_DOWN_START_POS) * easingAmount);

		}

	}

}