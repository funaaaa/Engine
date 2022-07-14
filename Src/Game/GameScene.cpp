#include "GameScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Player.h"
#include "FHelper.h"
#include "Camera.h"
#include "RayDenoiser.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipline.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "Input.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Pipline.h"
#include "WindowsAPI.h"
#include "GimmickMgr.h"

GameScene::GameScene()
{

	/*===== 初期化処理 =====*/

	// 定数バッファを生成。
	constBufferData_.Init();
	constBuffer_ = std::make_shared<DynamicConstBuffer>();
	constBuffer_->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer_->Write(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(RayConstBufferData));

	// デノイズAO用のパイプラインを設定。
	dAOuseShaders_.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	pipline_ = std::make_shared<RaytracingPipline>();
	pipline_->Setting(dAOuseShaders_, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 5, sizeof(Vec3) * 5 + sizeof(UINT) + sizeof(UINT), sizeof(Vec2), 6);

	// 天球用のスフィアを生成する。
	skyDomeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/skydome.jpg" });
	skyDomeIns_ = PolygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas_, PolygonInstanceRegister::SHADER_ID::AS);
	PolygonInstanceRegister::Ins()->AddScale(skyDomeIns_, Vec3(1000, 1000, 1000));

	// ステージを読み込む。
	stageBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "stage3.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/white.png" });
	stageIns_ = PolygonInstanceRegister::Ins()->CreateInstance(stageBlas_, PolygonInstanceRegister::DEF_GI);
	PolygonInstanceRegister::Ins()->AddScale(stageIns_, Vec3(200, 200, 200));
	stageGrassBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "grass.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" });
	stageGrassIns_ = PolygonInstanceRegister::Ins()->CreateInstance(stageGrassBlas_, PolygonInstanceRegister::DEF_GI);
	PolygonInstanceRegister::Ins()->AddScale(stageGrassIns_, Vec3(200, 200, 200));



	// ゴール用のオブジェクトを読み込む。
	goalBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	goalIns_ = PolygonInstanceRegister::Ins()->CreateInstance(goalBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
	PolygonInstanceRegister::Ins()->AddScale(goalIns_, Vec3(200, 200, 200));
	PolygonInstanceRegister::Ins()->AddTrans(goalIns_, GOAL_DEF_POS);

	goalCollisionBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "goalCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/inv.png" }, false, false);
	goalCollisionIns_ = PolygonInstanceRegister::Ins()->CreateInstance(goalCollisionBlas_, PolygonInstanceRegister::SHADER_ID::INVISIBILITY);
	PolygonInstanceRegister::Ins()->AddScale(goalCollisionIns_, Vec3(200, 200, 200));

	middlePointCollisionBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "middlePointCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/inv.png" }, false, false);
	middlePointCollisionIns_ = PolygonInstanceRegister::Ins()->CreateInstance(middlePointCollisionBlas_, PolygonInstanceRegister::SHADER_ID::INVISIBILITY);
	PolygonInstanceRegister::Ins()->AddScale(middlePointCollisionIns_, Vec3(200, 200, 200));

	// ステージの装飾オブジェクトをロード
	{
		stageOrnamentBlas_.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockA.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/gray.png" }));
		stageOrnamentIns_.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas_[static_cast<int>(stageOrnamentBlas_.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFLECTION));
		stageOrnamentBlas_.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockB.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/gray.png" }));
		stageOrnamentIns_.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas_[static_cast<int>(stageOrnamentBlas_.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFLECTION));
		stageOrnamentBlas_.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockC.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/green.png" }));
		stageOrnamentIns_.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas_[static_cast<int>(stageOrnamentBlas_.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFRACTION));
		stageOrnamentBlas_.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockD.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }));
		stageOrnamentIns_.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas_[static_cast<int>(stageOrnamentBlas_.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFRACTION));
		stageOrnamentBlas_.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockE.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/green.png" }));
		stageOrnamentIns_.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas_[static_cast<int>(stageOrnamentBlas_.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFRACTION));
		stageOrnamentBlas_.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockF.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }));
		stageOrnamentIns_.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas_[static_cast<int>(stageOrnamentBlas_.size()) - 1], PolygonInstanceRegister::SHADER_ID::DEF));
		stageOrnamentBlas_.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "goalSideObj.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/gray.png" }));
		stageOrnamentIns_.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas_[static_cast<int>(stageOrnamentBlas_.size()) - 1], PolygonInstanceRegister::SHADER_ID::DEF));
	}
	for (auto& index_ : stageOrnamentIns_) {

		PolygonInstanceRegister::Ins()->AddScale(index_, Vec3(200, 200, 200));

	}

	// ギミックをロード。
	GenerateGimmick();

	// プレイヤーを初期化。
	Player::StageData stageData;
	stageData.stageBlasIndex_ = stageBlas_;
	stageData.stageInsIndex_ = stageIns_;
	stageData.stageGrassBlasIndex_ = stageGrassBlas_;
	stageData.stageGrassInsIndex_ = stageGrassIns_;
	stageData.middlePointBlasIndex_ = middlePointCollisionBlas_;
	stageData.middlePointInsIndex_ = middlePointCollisionIns_;
	stageData.goalBlasIndex_ = goalCollisionBlas_;
	stageData.goalInsIndex_ = goalCollisionIns_;
	stageData.stageOrnamentBlasIndex_ = stageOrnamentBlas_;
	stageData.stageOrnamentInsIndex_ = stageOrnamentIns_;
	player_ = std::make_shared<Player>(stageData);

	// ゴール前のふわふわしているオブジェクトのBLASをロード
	beforeTheGoalObjectBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	for (int index_ = 0; index_ < 4; ++index_) {

		std::pair<int, int> buff;
		std::pair<Vec3, Vec3> defPosBuff;
		buff.first = PolygonInstanceRegister::Ins()->CreateInstance(beforeTheGoalObjectBlas_, PolygonInstanceRegister::DEF);
		defPosBuff.first = Vec3(250, 200, 1000 * index_ + 1500.0f);
		PolygonInstanceRegister::Ins()->AddTrans(buff.first, defPosBuff.first);
		PolygonInstanceRegister::Ins()->AddScale(buff.first, Vec3(40.0f, 40.0f, 40.0f));
		PolygonInstanceRegister::Ins()->AddRotate(buff.first, Vec3(DirectX::XM_2PI / 2.0f * index_, DirectX::XM_2PI / 2.0f * index_, 0));
		buff.second = PolygonInstanceRegister::Ins()->CreateInstance(beforeTheGoalObjectBlas_, PolygonInstanceRegister::DEF);
		defPosBuff.second = Vec3(-250, 200, 1000 * index_ + 1500.0f);
		PolygonInstanceRegister::Ins()->AddTrans(buff.second, defPosBuff.second);
		PolygonInstanceRegister::Ins()->AddScale(buff.second, Vec3(40.0f, 40.0f, 40.0f));
		PolygonInstanceRegister::Ins()->AddRotate(buff.second, Vec3(DirectX::XM_2PI / 2.0f * index_, DirectX::XM_2PI / 2.0f * index_, 0));
		beforeTheGoalObjectIns_.emplace_back(buff);
		beforeTheGoalObjectDefPos_.emplace_back(defPosBuff);
		beforeTheGoalObjectTimer_.push_back(1.0f * index_);

	}

	// Instanceのワールド行列を生成。
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// TLASを生成。
	tlas_ = std::make_shared<TLAS>();
	tlas_->GenerateTLAS();

	// AO出力用クラスをセット。
	aoOutput_ = std::make_shared<RaytracingOutput>();
	aoOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"AOOutput");
	denoiseAOOutput_ = std::make_shared<RaytracingOutput>();
	denoiseAOOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseAOOutput");

	// 色出力用クラスをセット。
	colorOutput_ = std::make_shared<RaytracingOutput>();
	colorOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput");

	// 明るさ情報出力用クラスをセット。
	lightOutput_ = std::make_shared<RaytracingOutput>();
	lightOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"LightOutput");
	denoiseLightOutput_ = std::make_shared<RaytracingOutput>();
	denoiseLightOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseLightOutput");

	// GI出力用クラスをセット。
	giOutput_ = std::make_shared<RaytracingOutput>();
	giOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"GIOutput");
	denoiseGiOutput_ = std::make_shared<RaytracingOutput>();
	denoiseGiOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseGIOutput");

	// デノイズマスク用クラスをセット。
	denoiseMaskOutput_ = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_->Setting(DXGI_FORMAT_R32G32B32A32_FLOAT, L"DenoiseMaskOutput");

	// 最終出力用クラスをセット。
	denoiseMixTextureOutput_ = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseMixTextureOutput");

	// シェーダーテーブルを生成。
	pipline_->ConstructionShaderTable();

	// 太陽に関する変数
	sunAngle_ = 0;
	sunSpeed_ = 0.0001f;

	isDisplayFPS_ = false;

	nextScene_ = SCENE_ID::RESULT;
	isTransition = false;

	isPassedMiddlePoint_ = false;
	rapCount_ = 0;

	// スプライトを生成。
	nowRapCountSprite_ = std::make_shared<Sprite>();
	nowRapCountSprite_->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	maxRapCountSprite_ = std::make_shared<Sprite>();
	maxRapCountSprite_->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	rapSlashSprite_ = std::make_shared<Sprite>();
	rapSlashSprite_->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);

	// フォントをロード
	{
		numFontHandle_[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/0.png");
		numFontHandle_[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/1.png");
		numFontHandle_[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/2.png");
		numFontHandle_[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/3.png");
		numFontHandle_[4] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/4.png");
		numFontHandle_[5] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/5.png");
		numFontHandle_[6] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/6.png");
		numFontHandle_[7] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/7.png");
		numFontHandle_[8] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/8.png");
		numFontHandle_[9] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/9.png");
		numFontHandle_[10] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/slash.png");
	}

}

void GameScene::Init()
{

	/*===== 初期化処理 =====*/

	nextScene_ = SCENE_ID::RESULT;
	isTransition = false;
	player_->Init();
	Camera::Ins()->Init();

	isPassedMiddlePoint_ = false;
	rapCount_ = 0;
	sunAngle_ = 0;

}

void GameScene::Update()
{

	/*===== 更新処理 =====*/

	// 入力処理
	Input();

	// ウィンドウの名前を更新。
	if (isDisplayFPS_) {

		FPS();

	}
	else {

		SetWindowText(DirectXBase::Ins()->windowsAPI_->hwnd, L"LE3A_21_フナクラベ_タクミ");

	}

	// 中間地点のオブジェクトを元の大きさに戻す。 AOの影でてしまうバグの対策用。シェーダー側で変える時間がなかったので臨時でずらしてます！
	PolygonInstanceRegister::Ins()->ChangeScale(middlePointCollisionIns_, Vec3(200, 200, 200));
	PolygonInstanceRegister::Ins()->ChangeTrans(middlePointCollisionIns_, Vec3(0, 0, 0));

	// プレイヤーを更新。
	player_->Update(constBufferData_, isPassedMiddlePoint_, rapCount_);

	// 乱数の種を更新。
	constBufferData_.debug.seed = FHelper::GetRand(0, 1000);

	// カメラを更新。
	Camera::Ins()->Update(player_->GetPos(), player_->GetForwardVec(), player_->GetUpVec(), player_->GetNowSpeedPer());


	// 中間地点に達していたらゴールを定位置に出す。
	if (isPassedMiddlePoint_) {

		PolygonInstanceRegister::Ins()->ChangeTrans(goalIns_, GOAL_DEF_POS);
		PolygonInstanceRegister::Ins()->ChangeScale(goalIns_, Vec3(200, 200, 200));
		PolygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns_, GOAL_DEF_POS);
		PolygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns_, Vec3(200, 200, 200));

	}
	else {

		PolygonInstanceRegister::Ins()->ChangeTrans(goalIns_, Vec3(0, -100, 0));
		PolygonInstanceRegister::Ins()->ChangeScale(goalIns_, Vec3(0, 0, 0));
		PolygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns_, Vec3(0, -100, 0));
		PolygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns_, Vec3(0, 0, 0));

	}

	// 3週していたらリザルトシーンに移動する。
	if (3 <= rapCount_) {

		isTransition = true;

	}

	// ラップ数のUIを更新。
	maxRapCountSprite_->ChangeTextureID(numFontHandle_[3], 0);
	nowRapCountSprite_->ChangeTextureID(numFontHandle_[rapCount_], 0);
	rapSlashSprite_->ChangeTextureID(numFontHandle_[10], 0);

	// ゴールオブジェクトを回転させる。
	PolygonInstanceRegister::Ins()->AddRotate(goalIns_, Vec3(0.01f, 0, 0));

	// ゴール前のオブジェクトを回転させる。
	static const int BEFORE_THE_GOAL_OBJECT_COUNT = 4;
	for (int index_ = 0; index_ < BEFORE_THE_GOAL_OBJECT_COUNT; ++index_) {

		const float SIN_WAVE_MOVE = 50.0f;

		PolygonInstanceRegister::Ins()->AddRotate(beforeTheGoalObjectIns_[index_].first, Vec3(0.01f, 0.01f, 0));
		PolygonInstanceRegister::Ins()->AddRotate(beforeTheGoalObjectIns_[index_].second, Vec3(0.01f, 0.01f, 0));
		PolygonInstanceRegister::Ins()->ChangeTrans(beforeTheGoalObjectIns_[index_].first, beforeTheGoalObjectDefPos_[index_].first + Vec3(0, sinf(beforeTheGoalObjectTimer_[index_]) * SIN_WAVE_MOVE, 0));
		PolygonInstanceRegister::Ins()->ChangeTrans(beforeTheGoalObjectIns_[index_].second, beforeTheGoalObjectDefPos_[index_].second + Vec3(0, sinf(beforeTheGoalObjectTimer_[index_]) * SIN_WAVE_MOVE, 0));

		beforeTheGoalObjectTimer_[index_] += 0.05f;

	}

	// 中間地点のオブジェクトを小さくする。 AOの影でてしまうバグの対策用。シェーダー側で変える時間がなかったので臨時で小さくしています！
	PolygonInstanceRegister::Ins()->ChangeScale(middlePointCollisionIns_, Vec3(0, 0, 0));
	PolygonInstanceRegister::Ins()->ChangeTrans(middlePointCollisionIns_, Vec3(0, -100, 0));


	tlas_->Update();

	// 太陽の角度を更新。
	sunAngle_ += sunSpeed_;
	constBufferData_.light.dirLight.lihgtDir = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	constBufferData_.light.dirLight.lihgtDir.Normalize();
	// 天球自体も回転させる。
	PolygonInstanceRegister::Ins()->AddRotate(skyDomeIns_, Vec3(0.001f, 0, 0));

}

void GameScene::Draw()
{

	/*===== 描画処理 =====*/

	// カメラ行列を更新。
	auto frameIndex = DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex();
	constBufferData_.camera.mtxView = Camera::Ins()->matView_;
	constBufferData_.camera.mtxViewInv = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera.mtxView);
	constBufferData_.camera.mtxProj = Camera::Ins()->matPerspective_;
	constBufferData_.camera.mtxProjInv = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera.mtxProj);

	// 定数バッファをセット。
	constBuffer_->Write(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(constBufferData_));


	// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList_->SetComputeRootSignature(pipline_->GetGlobalRootSig()->GetRootSig().Get());

	// TLASを設定。
	DirectXBase::Ins()->cmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas_->GetDescriptorHeapIndex()));

	// 定数バッファをセット
	DirectXBase::Ins()->cmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(frameIndex)->GetGPUVirtualAddress());

	// バリアを設定し各リソースの状態を遷移させる.
	aoOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseAOOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseGiOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 出力用UAVを設定。
	aoOutput_->SetComputeRootDescriptorTalbe(2);		// AOの結果出力用
	lightOutput_->SetComputeRootDescriptorTalbe(3);	// ライトの明るさの結果出力用
	colorOutput_->SetComputeRootDescriptorTalbe(4);	// テクスチャの色情報出力用
	giOutput_->SetComputeRootDescriptorTalbe(5);		// giの結果出力用
	denoiseMaskOutput_->SetComputeRootDescriptorTalbe(6);// デノイズをする際のマスク出力用

	// パイプラインを設定。
	DirectXBase::Ins()->cmdList_->SetPipelineState1(pipline_->GetStateObject().Get());

	// レイトレーシングを実行。
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipline_->GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList_->DispatchRays(&rayDesc);

	// [ノイズを描画]のときはデノイズをかけない。
	if (!constBufferData_.debug.isNoiseScene) {

		// デバッグ機能で[法線描画][メッシュ描画][ライトに当たった点のみ描画]のときはデノイズをかけないようにする。
		if (!constBufferData_.debug.isMeshScene && !constBufferData_.debug.isNormalScene && !constBufferData_.debug.isLightHitScene) {

			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				lightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseLightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

			// ライトにデノイズをかける。
			Denoiser::Ins()->Denoise(lightOutput_->GetUAVIndex(), denoiseLightOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 1, 1);

		}

		// AO情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				aoOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseAOOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

			// AOにデノイズをかける。
			Denoiser::Ins()->Denoise(aoOutput_->GetUAVIndex(), denoiseAOOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 100, 6);
		}


		// GI情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				giOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseGiOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

			// GIにデノイズをかける。
			Denoiser::Ins()->Denoise(giOutput_->GetUAVIndex(), denoiseGiOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 100, 1);

		}

		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseAOOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseGiOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList_->ResourceBarrier(5, barrierToUAV);

		// デノイズをかけたライティング情報と色情報を混ぜる。
		Denoiser::Ins()->MixColorAndLuminance(colorOutput_->GetUAVIndex(), denoiseAOOutput_->GetUAVIndex(), denoiseLightOutput_->GetUAVIndex(), denoiseGiOutput_->GetUAVIndex(), denoiseMixTextureOutput_->GetUAVIndex());
		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}
	// デノイズしないデバッグ状態の場合は、レイトレ関数から出力された生の値を合成する。
	else {


		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			aoOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			lightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			giOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList_->ResourceBarrier(5, barrierToUAV);

		// デノイズをかけたライティング情報と色情報を混ぜる。
		Denoiser::Ins()->MixColorAndLuminance(colorOutput_->GetUAVIndex(), aoOutput_->GetUAVIndex(), lightOutput_->GetUAVIndex(), giOutput_->GetUAVIndex(), denoiseMixTextureOutput_->GetUAVIndex());
		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}


	// バックバッファのインデックスを取得する。
	UINT backBufferIndex = DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
	};
	DirectXBase::Ins()->cmdList_->ResourceBarrier(_countof(barriers), barriers);

	// デバッグ情報によって描画するデータを変える。
	if (constBufferData_.debug.isLightHitScene || constBufferData_.debug.isMeshScene || constBufferData_.debug.isNormalScene) {

		// デノイズされた通常の描画
		lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList_->CopyResource(DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(), lightOutput_->GetRaytracingOutput().Get());
		lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}
	else {

		DirectXBase::Ins()->cmdList_->CopyResource(DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(), denoiseMixTextureOutput_->GetRaytracingOutput().Get());

	}

	// レンダーターゲットのリソースバリアをもとに戻す。
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	DirectXBase::Ins()->cmdList_->ResourceBarrier(_countof(endBarriers), endBarriers);

	// バリアを設定し各リソースの状態を遷移させる.
	aoOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseAOOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseLightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	colorOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseGiOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseMaskOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	// UIを描画
	nowRapCountSprite_->Draw();
	maxRapCountSprite_->Draw();
	rapSlashSprite_->Draw();

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
		SetWindowText(DirectXBase::Ins()->windowsAPI_->hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}

}

void GameScene::Input()
{

	/*===== 入力処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

	InputImGUI();

}

void GameScene::InputImGUI()
{

	/*===== IMGUI更新 =====*/

	// 太陽の移動速度を更新。
	ImGui::SliderFloat("Sun Speed", &sunSpeed_, 0.0f, 0.1f, "%.5f");

	// メッシュを表示する。
	bool isMesh = constBufferData_.debug.isMeshScene;
	ImGui::Checkbox("Mesh Scene", &isMesh);
	constBufferData_.debug.isMeshScene = isMesh;

	// 法線を表示する。
	bool isNormal = constBufferData_.debug.isNormalScene;
	ImGui::Checkbox("Normal Scene", &isNormal);
	constBufferData_.debug.isNormalScene = isNormal;

	// ライトがあたった面だけ表示するフラグを更新。
	bool isLightHit = constBufferData_.debug.isLightHitScene;
	ImGui::Checkbox("LightHit Scene", &isLightHit);
	constBufferData_.debug.isLightHitScene = isLightHit;

	// デバッグ用でノイズ画面を出すためのフラグをセット。
	bool isNoise = constBufferData_.debug.isNoiseScene;
	ImGui::Checkbox("Noise Scene", &isNoise);
	constBufferData_.debug.isNoiseScene = isNoise;

	// AOを行うかのフラグをセット。
	bool isNoAO = constBufferData_.debug.isNoAO;
	ImGui::Checkbox("NoAO Scene", &isNoAO);
	constBufferData_.debug.isNoAO = isNoAO;

	// GIを行うかのフラグをセット。
	bool isNoGI = constBufferData_.debug.isNoGI;
	ImGui::Checkbox("NoGI Scene", &isNoGI);
	constBufferData_.debug.isNoGI = isNoGI;

	// GIのみを描画するかのフラグをセット。
	bool isGIOnlyScene = constBufferData_.debug.isGIOnlyScene;
	ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene);
	constBufferData_.debug.isGIOnlyScene = isGIOnlyScene;

	// FPSを表示するかのフラグをセット。
	ImGui::Checkbox("Display FPS", &isDisplayFPS_);

	// 1個目
	GimmickMgr::Ins()->ChangeTrans(0, Vec3(100, -15, 1400));
	GimmickMgr::Ins()->ChangeRotate(0, Vec3(0, 0, 0));
	GimmickMgr::Ins()->ChangeScale(0, Vec3(100, 200, 200));

	// 2個目
	GimmickMgr::Ins()->ChangeTrans(1, Vec3(-80, -15, 3000));
	GimmickMgr::Ins()->ChangeRotate(1, Vec3(0, 0, 0));
	GimmickMgr::Ins()->ChangeScale(1, Vec3(100, 200, 200));

	// 3個目
	GimmickMgr::Ins()->ChangeTrans(2, Vec3(100, -15, 4000));
	GimmickMgr::Ins()->ChangeRotate(2, Vec3(0, 0, 0));
	GimmickMgr::Ins()->ChangeScale(2, Vec3(100, 200, 200));

	// 4個目
	GimmickMgr::Ins()->ChangeTrans(3, Vec3(-5842, -29, -167));
	GimmickMgr::Ins()->ChangeRotate(3, Vec3(0, 1.48f, 0));
	GimmickMgr::Ins()->ChangeScale(3, Vec3(200, 200, 200));

	// 5個目
	GimmickMgr::Ins()->ChangeTrans(4, Vec3(-2352, -18, 6336));
	GimmickMgr::Ins()->ChangeRotate(4, Vec3(0, 1.58f, 0));
	GimmickMgr::Ins()->ChangeScale(4, Vec3(190, 200, 200));


}

void GameScene::GenerateGimmick()
{

	// 1個目
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);

	// 2個目
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);

	// 3個目
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);

	// 4個目
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);

	// 5個目
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);

}
