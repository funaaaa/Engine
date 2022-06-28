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
	deAOPipline = std::make_shared<RaytracingPipline>();
	deAOPipline->Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 4, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZAを読み込む。
	//sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// 天球用のスフィアを生成する。
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, PorygonInstanceRegister::SHADER_ID_AS);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(100, 100, 100));

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
	stageIns = PorygonInstanceRegister::Ins()->CreateInstance(stageBlas, PorygonInstanceRegister::SHADER_ID_DEF);
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

	// 色出力用クラスをセット。
	colorOutput = std::make_shared<RaytracingOutput>();
	colorOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 明るさ情報出力用クラスをセット。
	lightOutput = std::make_shared<RaytracingOutput>();
	lightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// GI出力用クラスをセット。
	giOutput = std::make_shared<RaytracingOutput>();
	giOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 最終出力用クラスをセット。
	denoiseMixTextureOutput = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// シェーダーテーブルを生成。
	deAOPipline->ConstructionShaderTable();

	// デバッグ用でノイズ画面を出すフラグ。
	debugPiplineID = DENOISE_AO_PIPLINE;

	// ライトが動いたか
	isMoveLight = false;

	// デバッグ用のパイプラインを切り替えるやつ。
	enum DEGU_PIPLINE_ID {
		DEF_PIPLINE,
		AO_PIPLINE,
		DENOISE_AO_PIPLINE,
	};

	isDisplayFPS = false;

	nextScene = SCENE_ID::RESULT;
	isTransition = false;

	isPassedMiddlePoint = false;
	rapCount = 0;

	// スプライトを生成。
	nowRapCountSprite = std::make_shared<Sprite>();
	nowRapCountSprite->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), DirectX::XMFLOAT2(16, 32), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	maxRapCountSprite = std::make_shared<Sprite>();
	maxRapCountSprite->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), DirectX::XMFLOAT2(16, 32), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	rapSlashSprite = std::make_shared<Sprite>();
	rapSlashSprite->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), DirectX::XMFLOAT2(16, 32), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);

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

	// バリアを設定し各リソースの状態を遷移させる.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList->SetComputeRootSignature(deAOPipline->GetGlobalRootSig()->GetRootSig().Get());

	// TLASを設定。
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas->GetDescriptorHeapIndex()));

	// 定数バッファをセット
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, constBuffer->GetBuffer(frameIndex)->GetGPUVirtualAddress());

	// 出力用UAVを設定。
	aoOutput->SetComputeRootDescriptorTalbe(2);
	lightOutput->SetComputeRootDescriptorTalbe(3);
	colorOutput->SetComputeRootDescriptorTalbe(4);
	giOutput->SetComputeRootDescriptorTalbe(5);

	// パイプラインを設定。
	DirectXBase::Ins()->cmdList->SetPipelineState1(deAOPipline->GetStateObject().Get());

	// レイトレーシングを実行。
	D3D12_DISPATCH_RAYS_DESC rayDesc = deAOPipline->GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList->DispatchRays(&rayDesc);

	// デバッグ用のパイプラインがデノイズ用パイプラインだったら、コンピュートシェーダーを使ってデノイズをかける。
	if (debugPiplineID == DENOISE_AO_PIPLINE) {

		// [ノイズを描画]のときはデノイズをかけない。
		if (!constBufferData.debug.isNoiseScene) {

			// デバッグ機能で[法線描画][メッシュ描画][ライトに当たった点のみ描画]のときはデノイズをかけないようにする。
			if (!constBufferData.debug.isMeshScene && !constBufferData.debug.isNormalScene && !constBufferData.debug.isLightHitScene) {

				// ライトにデノイズをかける。
				Denoiser::Ins()->Denoise(lightOutput->GetUAVIndex(), 1, 3);

			}

			// [AOを行わない]のときはデノイズをかけない。
			if (!constBufferData.debug.isNoAO) {

				// AOにデノイズをかける。
				Denoiser::Ins()->Denoise(aoOutput->GetUAVIndex(), 100, 9);

			}

			// [GIを行わない]のときはデノイズをかけない。
			if (!constBufferData.debug.isNoGI) {

				// GIにデノイズをかける。
				Denoiser::Ins()->Denoise(giOutput->GetUAVIndex(), 100, 5);

			}

		}

		// デノイズをかけたライティング情報と色情報を混ぜる。
		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
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
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), lightOutput->GetRaytracingOutput().Get());

	}
	else {

		// デノイズされた通常の描画
		colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseMixTextureOutput->GetRaytracingOutput().Get());

	}

	// レンダーターゲットのリソースバリアをもとに戻す。
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	// バリアを設定し各リソースの状態を遷移させる.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

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

	bool isMove = false;

	if (Input::Ins()->isKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

	InputImGUI(isMove);

}

void GameScene::InputImGUI(bool& IsMove)
{

	/*===== IMGUI更新 =====*/

	// DirLightについて
	if (ImGui::TreeNode("DirLight")) {

		// ライトを表示するかどうかのフラグを更新。
		bool isActive = static_cast<bool>(constBufferData.light.dirLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(constBufferData.light.dirLight.isActive)) IsMove = true;
		constBufferData.light.dirLight.isActive = static_cast<int>(isActive);

		// 値を保存する。
		float dirX = constBufferData.light.dirLight.lihgtDir.x;
		float dirY = constBufferData.light.dirLight.lihgtDir.y;
		float dirZ = constBufferData.light.dirLight.lihgtDir.z;
		ImGui::SliderFloat("DirLightX", &constBufferData.light.dirLight.lihgtDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightY", &constBufferData.light.dirLight.lihgtDir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightZ", &constBufferData.light.dirLight.lihgtDir.z, -1.0f, 1.0f);

		// 変わっていたら
		if (dirX != constBufferData.light.dirLight.lihgtDir.x || dirY != constBufferData.light.dirLight.lihgtDir.y || dirZ != constBufferData.light.dirLight.lihgtDir.z) {

			IsMove = true;

		}

		// 正規化する。
		constBufferData.light.dirLight.lihgtDir.Normalize();

		ImGui::TreePop();

	}

	//// PointLightについて
	//if (ImGui::TreeNode("PointLight")) {

	//	// ライトを表示するかどうかのフラグを更新。
	//	bool isActive = static_cast<bool>(constBufferData.light.pointLight.isActive);
	//	ImGui::Checkbox("IsActive", &isActive);
	//	if (isActive != static_cast<bool>(constBufferData.light.pointLight.isActive)) IsMove = true;
	//	constBufferData.light.pointLight.isActive = static_cast<int>(isActive);

	//	// 値を保存する。
	//	float dirX = constBufferData.light.pointLight.lightPos.x;
	//	float dirY = constBufferData.light.pointLight.lightPos.y;
	//	float dirZ = constBufferData.light.pointLight.lightPos.z;
	//	float lightSize = constBufferData.light.pointLight.lightSize;
	//	float aoSampleCount = static_cast<float>(constBufferData.debug.aoSampleCount);
	//	float pointLightPower = constBufferData.light.pointLight.lightPower;
	//	float MOVE_LENGTH = 1500.0f;
	//	ImGui::SliderFloat("PointLightX", &constBufferData.light.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
	//	ImGui::SliderFloat("PointLightY", &constBufferData.light.pointLight.lightPos.y, 0.0f, 1000.0f);
	//	ImGui::SliderFloat("PointLightZ", &constBufferData.light.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
	//	ImGui::SliderFloat("PointLightRadius", &constBufferData.light.pointLight.lightSize, 1.0f, 50.0f);
	//	ImGui::SliderFloat("PointLightPower", &constBufferData.light.pointLight.lightPower, 100.0f, 1000.0f);
	//	ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
	//	constBufferData.debug.aoSampleCount = static_cast<int>(aoSampleCount);

	//	// 変わっていたら
	//	if (dirX != constBufferData.light.pointLight.lightPos.x || dirY != constBufferData.light.pointLight.lightPos.y || dirZ != constBufferData.light.pointLight.lightPos.z || lightSize != constBufferData.light.pointLight.lightSize || pointLightPower != constBufferData.light.pointLight.lightPower) {

	//		IsMove = true;

	//	}

	//	// ライトの色を設定。
	//	std::array<float, 3> lightColor = { constBufferData.light.pointLight.lightColor.x,constBufferData.light.pointLight.lightColor.y,constBufferData.light.pointLight.lightColor.z };
	//	ImGui::ColorPicker3("LightColor", lightColor.data());
	//	// 色が変わっていたら。
	//	if (lightColor[0] != constBufferData.light.pointLight.lightColor.x || lightColor[1] != constBufferData.light.pointLight.lightColor.y || lightColor[2] != constBufferData.light.pointLight.lightColor.z) {
	//		IsMove = true;
	//	}
	//	constBufferData.light.pointLight.lightColor.x = lightColor[0];
	//	constBufferData.light.pointLight.lightColor.y = lightColor[1];
	//	constBufferData.light.pointLight.lightColor.z = lightColor[2];

	//	ImGui::TreePop();

	//}


	if (IsMove) {
		constBufferData.debug.counter = 0;
	}
	else {
		++constBufferData.debug.counter;
	}

	// 階層構造にする。
	if (ImGui::TreeNode("Debug")) {

		// メッシュを表示する。
		bool isMesh = constBufferData.debug.isMeshScene;
		bool prevIsMesh = isMesh;
		ImGui::Checkbox("Mesh Scene", &isMesh);
		constBufferData.debug.isMeshScene = isMesh;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isMesh != prevIsMesh) {
			constBufferData.debug.counter = 0;
		}

		// 法線を表示する。
		bool isNormal = constBufferData.debug.isNormalScene;
		bool prevIsNormal = isNormal;
		ImGui::Checkbox("Normal Scene", &isNormal);
		constBufferData.debug.isNormalScene = isNormal;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isNormal != prevIsNormal) {
			constBufferData.debug.counter = 0;
		}

		// ライトがあたった面だけ表示するフラグを更新。
		bool isLightHit = constBufferData.debug.isLightHitScene;
		bool prevIsLightHit = isLightHit;
		ImGui::Checkbox("LightHit Scene", &isLightHit);
		constBufferData.debug.isLightHitScene = isLightHit;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isLightHit != prevIsLightHit) {
			constBufferData.debug.counter = 0;
		}

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


		ImGui::TreePop();

	}

	// 階層構造にする。
	if (ImGui::TreeNode("AS")) {

		// 太陽光線の強さを設定する。
		ImGui::SliderFloat("Sun Power", &constBufferData.as.eSun, -10, 100);

		// レイリー散乱定数の値を設定する。
		ImGui::SliderFloat("Rayleigh Scattering Power", &constBufferData.as.kr, -1, 1);

		// ミー散乱定数の値を設定する。
		ImGui::SliderFloat("Mie Scattering Power", &constBufferData.as.km, -1, 1);

		// サンプリング数を設定する。
		ImGui::SliderFloat("Sample Count", &constBufferData.as.samples, 0, 10);

		// 大気圏の一番上の高さ
		ImGui::SliderFloat("Outer Radius", &constBufferData.as.outerRadius, 0, 20000);

		// 地上の高さ
		ImGui::SliderFloat("Inner Radius", &constBufferData.as.innerRadius, 0, 20000);

		// 大気散乱を求める際に使用する定数
		ImGui::SliderFloat("Scattering G", &constBufferData.as.g, -1.0f, 1.0f);

		// 平均大気密度を求めるための高さ
		ImGui::SliderFloat("Aveheight", &constBufferData.as.aveHeight, 0.0f, 1.0f);

		ImGui::TreePop();

	}

}
