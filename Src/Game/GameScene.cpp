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
#include "CircuitStage.h"
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

#include "RayEngine.h"

#include "GLTF.h"

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
	tireMaskConstBuffer_->Generate(sizeof(Character::TireMaskUV) * 2, L"TireMaskUV");

	// 白塗りコンピュートシェーダー
	whiteOutComputeShader_ = std::make_shared<RayComputeShader>();
	whiteOutComputeShader_->Setting(L"Resource/ShaderFiles/WhiteMakeUpShader.hlsl", 0, 0, 0, {});

	characterMgr_ = std::make_shared<CharacterMgr>();

	// Instanceのワールド行列を生成。
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// 設定
	DriftParticleMgr::Ins()->Setting();

	// 太陽に関する変数
	sunAngle_ = 0.1f;
	sunSpeed_ = 0.01f;

	isDisplayFPS_ = false;

	nextScene_ = SCENE_ID::RESULT;
	isTransition_ = false;

	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;



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
	//nowRapCountSprite_ = std::make_shared<Sprite>();
	//nowRapCountSprite_->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), Vec2(16.0f, 32.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	//maxRapCountSprite_ = std::make_shared<Sprite>();
	//maxRapCountSprite_->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), Vec2(16.0f, 32.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	//rapSlashSprite_ = std::make_shared<Sprite>();
	//rapSlashSprite_->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), Vec2(16.0f, 32.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	coinUI_ = std::make_shared<Sprite>();
	coinUI_->GenerateForTexture(Vec3(140, 647, 0.1f), Vec2(192 / 2.0f, 64 / 2.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/coinUI.dds");
	rapUI_ = std::make_shared<Sprite>();
	rapUI_->GenerateForTexture(Vec3(327, 647, 0.1f), Vec2(224 / 2.0f, 64 / 2.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/rapUI.dds");

	coinCountUI_[0] = std::make_shared<Sprite>();
	coinCountUI_[0]->GenerateSpecifyTextureID(Vec3(145, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[0]);
	coinCountUI_[1] = std::make_shared<Sprite>();
	coinCountUI_[1]->GenerateSpecifyTextureID(Vec3(178, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[0]);

	nowRapCountUI_ = std::make_shared<Sprite>();
	nowRapCountUI_->GenerateSpecifyTextureID(Vec3(321, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[1]);
	slashUI_ = std::make_shared<Sprite>();
	slashUI_->GenerateSpecifyTextureID(Vec3(356, 651, 0.1f), Vec2(16.0f * 0.6f, 32.0f * 0.6f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[10]);
	maxRapCountUI_ = std::make_shared<Sprite>();
	maxRapCountUI_->GenerateSpecifyTextureID(Vec3(381, 651, 0.1f), Vec2(16.0f * 0.5f, 32.0f * 0.5f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[3]);

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

	nextScene_ = SCENE_ID::RESULT;
	isTransition_ = false;

	concentrationLine_->Init();

	countDownSprite_->ChangePosition(Vec3(100000, 10000, 100));
	goSprite_->ChangePosition(Vec3(100000, 10000, 100));

	characterMgr_ = std::make_shared<CharacterMgr>();

	if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::AI) {

		// プレイヤーを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// AIを生成。
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_);

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

	// 一旦サーキットステージを有効化する。
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex());


	// TLASを生成。
	RayEngine::Ins()->SettingTLAS();


	Camera::Ins()->Init();

	DriftParticleMgr::Ins()->Init();

	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;
	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	sunAngle_ = 0.8f;

}

void GameScene::Update()
{

	/*===== 更新処理 =====*/

	// 入力処理
	Input();

	// ウィンドウの名前を更新。
	//if (true) {

	FPS();

	//}
	//else {

	//SetWindowText(Engine::Ins()->windowsAPI_->hwnd_, L"LE3A_20_フナクラベ_タクミ");

	//}

	// キャラを更新。
	characterMgr_->Update(stages_[STAGE_ID::MUGEN], isBeforeStart_, isGameFinish_);

	// 乱数の種を更新。
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// カメラを更新。
	Camera::Ins()->Update(characterMgr_->GetPlayerIns().lock()->GetPos(), characterMgr_->GetPlayerIns().lock()->GetCameraForwardVec(), characterMgr_->GetPlayerIns().lock()->GetUpVec(), characterMgr_->GetPlayerIns().lock()->GetNowSpeedPer(), isBeforeStart_, isGameFinish_);

	// いずれかのキャラがゴールしていたらリザルトシーンに移動する。
	if (characterMgr_->CheckGoal()) {


		isGameFinish_ = true;

		++transitionTimer;
		if (TRANSION_TIME < transitionTimer) {

			isTransition_ = true;

			characterMgr_->Init();

			return;

		}

		//isTransition_ = true;

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
	stages_[STAGE_ID::MUGEN]->Update();

	// ゴールの表示非表示を切り替え。
	if (characterMgr_->GetPlayerIns().lock()->GetIsPassedMiddlePoint()) {
		stages_[STAGE_ID::MUGEN]->DisplayGoal();
	}
	else {
		stages_[STAGE_ID::MUGEN]->NonDisplayGoal();
	}

	// TLASやパイプラインを更新。
	RayEngine::Ins()->Update();

	// 太陽の角度を更新。
	//sunAngle_ = 0.8f;
	//if (0.0f < RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_) {

	//	sunAngle_ += sunSpeed_;
	//	sunAngle_ += sunSpeed_;
	//	sunAngle_ += sunSpeed_;

	//}
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();
	// 天球自体も回転させる。
	//skyDomeIns_.lock()->AddRotate(Vec3(0.001f, 0, 0));

	// 甲羅を更新。
	ShellObjectMgr::Ins()->Update(stages_[STAGE_ID::MUGEN]);

	// 煙を更新する。
	DriftParticleMgr::Ins()->Update();

	// 集中線を更新。
	if (characterMgr_->GetPlayerIns().lock()->GetIdConcentrationLine()) {
		concentrationLine_->Generate();
	}
	concentrationLine_->Update();

}

void GameScene::Draw()
{

	/*===== 描画処理 =====*/

	// レイトレーシングを実行。
	RayEngine::Ins()->Draw();


	// 床を白塗り
	static int a = 0;
	if (Input::Ins()->IsKeyTrigger(DIK_R) || a == 0) {

		whiteOutComputeShader_->Dispatch(4096 / 32, 4096 / 32, 1, tireMaskTexture_->GetUAVIndex());
		++a;

	}

	// タイヤ痕を書き込む。
	std::vector<Character::TireMaskUV> tireMaskUV;
	bool isWriteTireMask = characterMgr_->CheckTireMask(stages_[STAGE_ID::MUGEN], tireMaskUV);

	if (isWriteTireMask) {

		// UAVを書き込む。
		tireMaskConstBuffer_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), tireMaskUV.data(), sizeof(Character::TireMaskUV) * 2);
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
	static int firstTime = 0;
	if (firstTime != 0) {

		UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

		concentrationLine_->Draw();

		// コインの取得数のui。
		coinCountUI_[0]->Draw();
		coinCountUI_[1]->Draw();

		// 現在のラップ数のui。
		nowRapCountUI_->Draw();
		slashUI_->Draw();
		maxRapCountUI_->Draw();

		// 左下のuiのフレーム。
		coinUI_->Draw();
		rapUI_->Draw();

		// カウントダウン用のui。
		countDownSprite_->Draw();

		// カウントダウン終了時のgoのui。
		goSprite_->Draw();

		resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	}
	if (firstTime == 0) ++firstTime;

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

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

	InputImGUI();

}

#include "BaseItem.h"
void GameScene::InputImGUI()
{

	/*===== IMGUI更新 =====*/

	ImGui::Text("Let's do three laps!");

	ImGui::DragFloat("SunAngle", &sunAngle_, 0.005f);
	bool isMip = RayEngine::Ins()->constBufferData_.light_.pointLight_[0].pad_.x;
	ImGui::Checkbox("MipFlag", &isMip);
	RayEngine::Ins()->constBufferData_.light_.pointLight_[0].pad_.x = isMip;

	//// 太陽の移動速度を更新。
	//ImGui::SliderFloat("Sun Speed", &sunSpeed_, 0.0f, 0.1f, "%.5f");

	//// メッシュを表示する。
	//bool isMesh = constBufferData_.debug_.isMeshScene_;
	//ImGui::Checkbox("Mesh Scene", &isMesh);
	//constBufferData_.debug_.isMeshScene_ = isMesh;

	//// 法線を表示する。
	//bool isNormal = constBufferData_.debug_.isNormalScene_;
	//ImGui::Checkbox("Normal Scene", &isNormal);
	//constBufferData_.debug_.isNormalScene_ = isNormal;

	//// ライトがあたった面だけ表示するフラグを更新。
	//bool isLightHit = constBufferData_.debug_.isLightHitScene_;
	//ImGui::Checkbox("LightHit Scene", &isLightHit);
	//constBufferData_.debug_.isLightHitScene_ = isLightHit;

	//// デバッグ用でノイズ画面を出すためのフラグをセット。
	//bool isNoise = constBufferData_.debug_.isNoiseScene_;
	//ImGui::Checkbox("Noise Scene", &isNoise);
	//constBufferData_.debug_.isNoiseScene_ = isNoise;

	//// AOを行うかのフラグをセット。
	//bool isNoAO_ = constBufferData_.debug_.isNoAO_;
	//ImGui::Checkbox("NoAO Scene", &isNoAO_);
	//constBufferData_.debug_.isNoAO_ = isNoAO_;

	//// GIを行うかのフラグをセット。
	//bool isNoGI_ = constBufferData_.debug_.isNoGI_;
	//ImGui::Checkbox("NoGI Scene", &isNoGI_);
	//constBufferData_.debug_.isNoGI_ = isNoGI_;

	//// GIのみを描画するかのフラグをセット。
	//bool isGIOnlyScene_ = constBufferData_.debug_.isGIOnlyScene_;
	//ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene_);
	//constBufferData_.debug_.isGIOnlyScene_ = isGIOnlyScene_;

	//// FPSを表示するかのフラグをセット。
	//ImGui::Checkbox("Display FPS", &isDisplayFPS_);

	//// アイテムデバッグ用。
	//bool haveItem = characterMgr_->GetPlayerIns().lock()->item_.operator bool();

	//if (haveItem) {

	//	bool haveBoostItem = characterMgr_->GetPlayerIns().lock()->item_->GetItemID() == BaseItem::ItemID::BOOST;

	//	ImGui::Checkbox("BoostItem", &haveBoostItem);

	//	bool haveShellItem = characterMgr_->GetPlayerIns().lock()->item_->GetItemID() == BaseItem::ItemID::SHELL;

	//	ImGui::Checkbox("ShellItem", &haveShellItem);

	//}

	//Vec3 position = itemFrameUI_->GetPos();
	//float pos[3] = { position.x_, position.y_, position.z_ };

	//ImGui::DragFloat3("Position", pos, 1.0f);

	//position = Vec3(pos[0], pos[1], pos[2]);
	//itemFrameUI_->ChangePosition(position);



	//int index = 37;

	//Vec3 pos = PolygonInstanceRegister::Ins()->GetPos(index);

	//float posArray[3] = { pos.x_, pos.y_, pos.z_ };

	//ImGui::DragFloat3("Pos", posArray, 1.0f);

	//pos.x_ = posArray[0];
	//pos.y_ = posArray[1];
	//pos.z_ = posArray[2];

	//PolygonInstanceRegister::Ins()->ChangeTrans(index, pos);


	//Vec3 rotate = PolygonInstanceRegister::Ins()->GetRotateVec3(index);

	//float rotateArray[3] = { rotate.x_, rotate.y_, rotate.z_ };

	//ImGui::DragFloat3("Rotate", rotateArray, 0.001f);

	//rotate.x_ = rotateArray[0];
	//rotate.y_ = rotateArray[1];
	//rotate.z_ = rotateArray[2];

	//PolygonInstanceRegister::Ins()->ChangeRotate(index, rotate);


	//DirectX::XMMATRIX scale = PolygonInstanceRegister::Ins()->GetScale(index);

	//float scaleArray[3] = { scale.r[0].m128_f32[0], scale.r[1].m128_f32[1], scale.r[2].m128_f32[2] };

	//ImGui::DragFloat3("Scale", scaleArray, 1.0f);

	//Vec3 scaleVec3;
	//scaleVec3.x_ = scaleArray[0];
	//scaleVec3.y_ = scaleArray[1];
	//scaleVec3.z_ = scaleArray[2];

	//PolygonInstanceRegister::Ins()->ChangeScale(index, scaleVec3);



	//// デバッグ用
	//if (Input::Ins()->IsKey(DIK_U)) {

	//	PolygonInstanceRegister::Ins()->ChangeTrans(index, characterMgr_->GetPlayerIns().lock()->GetPos());

	//}

	// マテリアルの値を書き換える。
	//ImGui::Text("MaterialData");
	//ImGui::SliderFloat("Metalness", &pbrSphereBlas_.lock()->GetMaterial().metalness_, 0.0f, 1.0f);
	//ImGui::SliderFloat("Specular", &pbrSphereBlas_.lock()->GetMaterial().specular_, 0.0f, 1.0f);
	//ImGui::SliderFloat("Roughness", &pbrSphereBlas_.lock()->GetMaterial().roughness_, 0.0f, 1.0f);
	//RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

	//pbrSphereBlas_.lock()->IsChangeMaterial();


}

void GameScene::GenerateGimmick()
{

	//// 1個目
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 2個目
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 3個目
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 4個目
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 5個目
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

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