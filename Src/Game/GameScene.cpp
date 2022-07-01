#include "GameScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PorygonInstanceRegister.h"
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

GameScene::GameScene()
{

	/*===== 初期化処理 =====*/

	// 定数バッファを生成。
	constBufferData.Init();
	constBuffer = std::make_shared<DynamicConstBuffer>();
	constBuffer->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(RayConstBufferData));

	// デノイズAO用のパイプラインを設定。
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	pipline = std::make_shared<RaytracingPipline>();
	pipline->Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 5, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZAを読み込む。
	//sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// 天球用のスフィアを生成する。
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, PorygonInstanceRegister::SHADER_ID_AS);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(1000, 1000, 1000));

	// ライト用のスフィアを読み込む。
	sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });
	for (auto& index : sphereIns) {

		index = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, PorygonInstanceRegister::SHADER_ID_LIGHT);
		PorygonInstanceRegister::Ins()->AddScale(index, Vec3(1, 1, 1));
		PorygonInstanceRegister::Ins()->ChangeTrans(index, Vec3(0, 300, 0));

	}

	// ドリフト時のパーティクルのクラスをセッティングする。
	//DriftParticleMgr::Ins()->Setting(0);

	// ステージを読み込む。
	stageBlas = BLASRegister::Ins()->GenerateObj("Resource/", "stage3.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	stageIns = PorygonInstanceRegister::Ins()->CreateInstance(stageBlas, PorygonInstanceRegister::SHADER_ID_DEF_GI);
	PorygonInstanceRegister::Ins()->AddScale(stageIns, Vec3(200, 200, 200));
	stageGrassBlas = BLASRegister::Ins()->GenerateObj("Resource/", "grass.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/green.png",L"Resource/grassNormal.png" });
	stageGrassIns = PorygonInstanceRegister::Ins()->CreateInstance(stageGrassBlas, PorygonInstanceRegister::SHADER_ID_DEF);
	PorygonInstanceRegister::Ins()->AddScale(stageGrassIns, Vec3(200, 200, 200));

	// ゴール用のオブジェクトを読み込む。
	goalBlas = BLASRegister::Ins()->GenerateObj("Resource/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });
	goalIns = PorygonInstanceRegister::Ins()->CreateInstance(goalBlas, PorygonInstanceRegister::SHADER_ID_REFRACTION);
	PorygonInstanceRegister::Ins()->AddScale(goalIns, Vec3(200, 200, 200));
	PorygonInstanceRegister::Ins()->AddTrans(goalIns, GOAL_DEF_POS);

	goalCollisionBlas = BLASRegister::Ins()->GenerateObj("Resource/", "goalCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/inv.png" }, false, false);
	goalCollisionIns = PorygonInstanceRegister::Ins()->CreateInstance(goalCollisionBlas, PorygonInstanceRegister::SHADER_ID_INVISIBILITY);
	PorygonInstanceRegister::Ins()->AddScale(goalCollisionIns, Vec3(200, 200, 200));

	middlePointCollisionBlas = BLASRegister::Ins()->GenerateObj("Resource/", "middlePointCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/inv.png" }, false, false);
	middlePointCollisionIns = PorygonInstanceRegister::Ins()->CreateInstance(middlePointCollisionBlas, PorygonInstanceRegister::SHADER_ID_INVISIBILITY);
	PorygonInstanceRegister::Ins()->AddScale(middlePointCollisionIns, Vec3(200, 200, 200));

	// ステージの装飾オブジェクトをロード
	{
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockA.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockB.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockC.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockD.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockE.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockF.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "goalSideObj.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
	}
	for (auto& index : stageOrnamentIns) {

		PorygonInstanceRegister::Ins()->AddScale(index, Vec3(200, 200, 200));

	}

	// プレイヤーを初期化。
	Player::StageData stageData;
	stageData.stageBlasIndex = stageBlas;
	stageData.stageInsIndex = stageIns;
	stageData.stageGrassBlasIndex = stageGrassBlas;
	stageData.stageGrassInsIndex = stageGrassIns;
	stageData.middlePointBlasIndex = middlePointCollisionBlas;
	stageData.middlePointInsIndex = middlePointCollisionIns;
	stageData.goalBlasIndex = goalCollisionBlas;
	stageData.goalInsIndex = goalCollisionIns;
	player = std::make_shared<Player>(stageData);

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLASを生成。
	tlas = std::make_shared<TLAS>();
	tlas->GenerateTLAS();

	// AO出力用クラスをセット。
	aoOutput = std::make_shared<RaytracingOutput>();
	aoOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	denoiseAOOutput = std::make_shared<RaytracingOutput>();
	denoiseAOOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 色出力用クラスをセット。
	colorOutput = std::make_shared<RaytracingOutput>();
	colorOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 明るさ情報出力用クラスをセット。
	lightOutput = std::make_shared<RaytracingOutput>();
	lightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	denoiseLightOutput = std::make_shared<RaytracingOutput>();
	denoiseLightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// GI出力用クラスをセット。
	giOutput = std::make_shared<RaytracingOutput>();
	giOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	denoiseGiOutput = std::make_shared<RaytracingOutput>();
	denoiseGiOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// デノイズマスク用クラスをセット。
	denoiseMaskOutput = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 最終出力用クラスをセット。
	denoiseMixTextureOutput = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// シェーダーテーブルを生成。
	pipline->ConstructionShaderTable();

	// 太陽に関する変数
	sunAngle = 0;
	sunSpeed = 0.0001f;

	isDisplayFPS = false;

	nextScene = SCENE_ID::RESULT;
	isTransition = false;

	isPassedMiddlePoint = false;
	rapCount = 0;

	// スプライトを生成。
	nowRapCountSprite = std::make_shared<Sprite>();
	nowRapCountSprite->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	maxRapCountSprite = std::make_shared<Sprite>();
	maxRapCountSprite->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	rapSlashSprite = std::make_shared<Sprite>();
	rapSlashSprite->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);

	// フォントをロード
	{
		numFontHandle[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/0.png");
		numFontHandle[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/1.png");
		numFontHandle[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/2.png");
		numFontHandle[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/3.png");
		numFontHandle[4] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/4.png");
		numFontHandle[5] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/5.png");
		numFontHandle[6] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/6.png");
		numFontHandle[7] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/7.png");
		numFontHandle[8] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/8.png");
		numFontHandle[9] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/9.png");
		numFontHandle[10] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/slash.png");
	}

	// バリアを設定し各リソースの状態を遷移させる.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseAOOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseGiOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

}

void GameScene::Init()
{

	/*===== 初期化処理 =====*/

	nextScene = SCENE_ID::RESULT;
	isTransition = false;
	player->Init();
	Camera::Ins()->Init();

	isPassedMiddlePoint = false;
	rapCount = 0;
	sunAngle = 0;

}

void GameScene::Update()
{

	/*===== 更新処理 =====*/

	// 入力処理
	Input();

	// ウィンドウの名前を更新。
	if (isDisplayFPS) {

		FPS();

	}
	else {

		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, L"LE3A_21_フナクラベ_タクミ");

	}

	// プレイヤーを更新。
	player->Update(constBufferData, isPassedMiddlePoint, rapCount);

	// 乱数の種を更新。
	constBufferData.debug.seed = FHelper::GetRand(0, 1000);

	// カメラを更新。
	Camera::Ins()->Update(player->GetPos(), player->GetForwardVec(), player->GetUpVec(), player->GetNowSpeedPer());

	// 点光源の位置を更新。
	int counter = 0;
	for (auto& index : sphereIns) {
		PorygonInstanceRegister::Ins()->ChangeTrans(index, constBufferData.light.pointLight[counter].lightPos);

		// ライトが有効化されていなかったらサイズを0にして描画しない。
		if (constBufferData.light.pointLight[counter].isActive) {
			PorygonInstanceRegister::Ins()->ChangeScale(index, constBufferData.light.pointLight[counter].lightSize);
		}
		else {
			PorygonInstanceRegister::Ins()->ChangeTrans(index, Vec3(-100000, -100000, -100000));
		}
		++counter;
	}

	// 中間地点に達していたらゴールを定位置に出す。
	if (isPassedMiddlePoint) {

		PorygonInstanceRegister::Ins()->ChangeTrans(goalIns, GOAL_DEF_POS);
		PorygonInstanceRegister::Ins()->ChangeScale(goalIns, Vec3(200, 200, 200));
		PorygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns, GOAL_DEF_POS);
		PorygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns, Vec3(200, 200, 200));

	}
	else {

		PorygonInstanceRegister::Ins()->ChangeTrans(goalIns, Vec3(-10000, -10000, -10000));
		PorygonInstanceRegister::Ins()->ChangeScale(goalIns, Vec3(0, 0, 0));
		PorygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns, Vec3(-10000, -10000, -10000));
		PorygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns, Vec3(0, 0, 0));

	}

	// 3週していたらリザルトシーンに移動する。
	if (3 <= rapCount) {

		isTransition = true;

	}

	// ラップ数のUIを更新。
	maxRapCountSprite->ChangeTextureID(numFontHandle[3], 0);
	nowRapCountSprite->ChangeTextureID(numFontHandle[rapCount], 0);
	rapSlashSprite->ChangeTextureID(numFontHandle[10], 0);

	// ゴールオブジェクトを回転させる。
	PorygonInstanceRegister::Ins()->AddRotate(goalIns, Vec3(0.01f, 0, 0));

	tlas->Update();

	// 太陽の角度を更新。
	sunAngle += sunSpeed;
	constBufferData.light.dirLight.lihgtDir = Vec3(-cos(sunAngle), -sin(sunAngle), 0.5f);
	constBufferData.light.dirLight.lihgtDir.Normalize();
	// 天球自体も回転させる。
	PorygonInstanceRegister::Ins()->AddRotate(skyDomeIns, Vec3(0.001f, 0, 0));

}

void GameScene::Draw()
{

	/*===== 描画処理 =====*/

	// カメラ行列を更新。
	auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
	constBufferData.camera.mtxView = Camera::Ins()->matView;
	constBufferData.camera.mtxViewInv = DirectX::XMMatrixInverse(nullptr, constBufferData.camera.mtxView);
	constBufferData.camera.mtxProj = Camera::Ins()->matPerspective;
	constBufferData.camera.mtxProjInv = DirectX::XMMatrixInverse(nullptr, constBufferData.camera.mtxProj);

	// 定数バッファをセット。
	constBuffer->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(constBufferData));


	// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList->SetComputeRootSignature(pipline->GetGlobalRootSig()->GetRootSig().Get());

	// TLASを設定。
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas->GetDescriptorHeapIndex()));

	// 定数バッファをセット
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, constBuffer->GetBuffer(frameIndex)->GetGPUVirtualAddress());

	// 出力用UAVを設定。
	aoOutput->SetComputeRootDescriptorTalbe(2);		// AOの結果出力用
	lightOutput->SetComputeRootDescriptorTalbe(3);	// ライトの明るさの結果出力用
	colorOutput->SetComputeRootDescriptorTalbe(4);	// テクスチャの色情報出力用
	giOutput->SetComputeRootDescriptorTalbe(5);		// giの結果出力用
	denoiseMaskOutput->SetComputeRootDescriptorTalbe(6);// デノイズをする際のマスク出力用

	// パイプラインを設定。
	DirectXBase::Ins()->cmdList->SetPipelineState1(pipline->GetStateObject().Get());

	// レイトレーシングを実行。
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipline->GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList->DispatchRays(&rayDesc);

	// [ノイズを描画]のときはデノイズをかけない。
	if (!constBufferData.debug.isNoiseScene) {

		// デバッグ機能で[法線描画][メッシュ描画][ライトに当たった点のみ描画]のときはデノイズをかけないようにする。
		if (!constBufferData.debug.isMeshScene && !constBufferData.debug.isNormalScene && !constBufferData.debug.isLightHitScene) {

			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				lightOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(1, barrierToUAV);

			// ライトにデノイズをかける。
			Denoiser::Ins()->Denoise(lightOutput->GetUAVIndex(), denoiseLightOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 1, 1);

		}

		// AO情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				aoOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(1, barrierToUAV);

			// AOにデノイズをかける。
			Denoiser::Ins()->Denoise(aoOutput->GetUAVIndex(), denoiseAOOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 100, 6);
		}


		// GI情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				giOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(1, barrierToUAV);

			// GIにデノイズをかける。
			Denoiser::Ins()->Denoise(giOutput->GetUAVIndex(), denoiseGiOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 100, 1);

		}

		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseAOOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseGiOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList->ResourceBarrier(5, barrierToUAV);

		// デノイズをかけたライティング情報と色情報を混ぜる。
		Denoiser::Ins()->MixColorAndLuminance(colorOutput->GetUAVIndex(), denoiseAOOutput->GetUAVIndex(), denoiseLightOutput->GetUAVIndex(), denoiseGiOutput->GetUAVIndex(), denoiseMixTextureOutput->GetUAVIndex());
		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}
	// デノイズしないデバッグ状態の場合は、レイトレ関数から出力された生の値を合成する。
	else {


		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			aoOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			lightOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			giOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList->ResourceBarrier(5, barrierToUAV);

		// デノイズをかけたライティング情報と色情報を混ぜる。
		Denoiser::Ins()->MixColorAndLuminance(colorOutput->GetUAVIndex(), aoOutput->GetUAVIndex(), lightOutput->GetUAVIndex(), giOutput->GetUAVIndex(), denoiseMixTextureOutput->GetUAVIndex());
		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}


	// バックバッファのインデックスを取得する。
	UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
	};
	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(barriers), barriers);

	// デバッグ情報によって描画するデータを変える。
	if (constBufferData.debug.isLightHitScene || constBufferData.debug.isMeshScene || constBufferData.debug.isNormalScene) {

		// デノイズされた通常の描画
		lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), lightOutput->GetRaytracingOutput().Get());
		lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}
	else {

		// デノイズされた通常の描画
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseMixTextureOutput->GetRaytracingOutput().Get());


	}

	// レンダーターゲットのリソースバリアをもとに戻す。
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	// UIを描画
	nowRapCountSprite->Draw();
	maxRapCountSprite->Draw();
	rapSlashSprite->Draw();

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
		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}

}

void GameScene::Input()
{

	/*===== 入力処理 =====*/

	if (Input::Ins()->isKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

	InputImGUI();

}

void GameScene::InputImGUI()
{

	/*===== IMGUI更新 =====*/

	// 太陽の移動速度を更新。
	ImGui::SliderFloat("Sun Speed", &sunSpeed, 0.0f, 0.1f, "%.5f");

	// メッシュを表示する。
	bool isMesh = constBufferData.debug.isMeshScene;
	ImGui::Checkbox("Mesh Scene", &isMesh);
	constBufferData.debug.isMeshScene = isMesh;

	// 法線を表示する。
	bool isNormal = constBufferData.debug.isNormalScene;
	ImGui::Checkbox("Normal Scene", &isNormal);
	constBufferData.debug.isNormalScene = isNormal;

	// ライトがあたった面だけ表示するフラグを更新。
	bool isLightHit = constBufferData.debug.isLightHitScene;
	ImGui::Checkbox("LightHit Scene", &isLightHit);
	constBufferData.debug.isLightHitScene = isLightHit;

	// デバッグ用でノイズ画面を出すためのフラグをセット。
	bool isNoise = constBufferData.debug.isNoiseScene;
	ImGui::Checkbox("Noise Scene", &isNoise);
	constBufferData.debug.isNoiseScene = isNoise;

	// AOを行うかのフラグをセット。
	bool isNoAO = constBufferData.debug.isNoAO;
	ImGui::Checkbox("NoAO Scene", &isNoAO);
	constBufferData.debug.isNoAO = isNoAO;

	// GIを行うかのフラグをセット。
	bool isNoGI = constBufferData.debug.isNoGI;
	ImGui::Checkbox("NoGI Scene", &isNoGI);
	constBufferData.debug.isNoGI = isNoGI;

	// GIのみを描画するかのフラグをセット。
	bool isGIOnlyScene = constBufferData.debug.isGIOnlyScene;
	ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene);
	constBufferData.debug.isGIOnlyScene = isGIOnlyScene;

	// FPSを表示するかのフラグをセット。
	ImGui::Checkbox("Display FPS", &isDisplayFPS);


}
