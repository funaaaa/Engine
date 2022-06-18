#include "DevDXR.h"

void DevDXR::Init() {


	constBufferData.Init();
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");
	constBuff.Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(KariConstBufferData));

	// デノイズAO用のパイプラインを設定。
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	deAOPipline.Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 4, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZAを読み込む。
	//sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// 天球用のスフィアを生成する。
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, 1);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(10000000, 10000000, 10000000));

	// ライト用のスフィアを読み込む。
	sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 0);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLASを生成。
	tlas.GenerateTLAS();

	// レイトレ出力用クラスをセット。
	aoOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// レイトレ出力用クラスをセット。
	colorOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 明るさ情報出力用クラスをセット。
	lightOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// gi出力用クラスをセット。
	giOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// デノイズの結果出力用クラスをセット。
	denoiseMixTextureOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// シェーダーテーブルを生成。
	deAOPipline.ConstructionShaderTable();

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

}

void DevDXR::Update() {

	/*----------毎フレーム処理(描画前処理)----------*/
	DirectXBase::Ins()->processBeforeDrawing();

	/*----- 更新処理 -----*/

	// 天球を回転させる。
	//PorygonInstanceRegister::Ins()->AddRotate(skyDomeIns, { 0.01,0.01,0.01 });

	isMoveLight = false;
	Input(constBufferData, isMoveLight, debugPiplineID);

	// ウィンドウの名前を更新。
	if (isDisplayFPS) {

		FPS();

	}
	else {

		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, L"LE3A_21_フナクラベ_タクミ");

	}

	// ビュー行列を生成。
	Camera::Ins()->GenerateMatView();

	// 乱数の種を更新。
	constBufferData.seed = FHelper::GetRand(0, 1000);

	// カメラを更新。
	Camera::Ins()->Update();

	constBufferData.eye = Camera::Ins()->eye;
	constBufferData.target = Camera::Ins()->target;
	constBufferData.up = Camera::Ins()->up;

	// ライトが動いたときのみ、ワールド行列を再計算してTLASを更新する。
	//if (isMoveLight) {

		// 点光源の位置を更新。
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.pointLight.lightPos);
	PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.pointLight.lightSize);

	tlas.Update();

	//}

	/*----- 描画処理 -----*/

	// 画面に表示されるレンダーターゲットに戻す。
	//DirectXBase::Ins()->SetRenderTarget();

}

void DevDXR::Draw() {


	RaytracingPipline setPipline = {};

	// TLAS更新。
	//tlas.Update();

	// デバッグ用のパイプラインIDに応じたパイプラインをセットする。
	setPipline = deAOPipline;

	// カメラ行列を更新。
	auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
	constBufferData.mtxView = DirectX::XMMatrixLookAtLH(constBufferData.eye.ConvertXMVECTOR(), constBufferData.target.ConvertXMVECTOR(), constBufferData.up.ConvertXMVECTOR());
	constBufferData.mtxViewInv = DirectX::XMMatrixInverse(nullptr, constBufferData.mtxView);

	// 定数バッファの中身を更新する。
	constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
	auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

	// バリアを設定し各リソースの状態を遷移させる.
	aoOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList->SetComputeRootSignature(setPipline.GetGlobalRootSig()->GetRootSig().Get());

	// TLASを設定。
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));

	// 出力用UAVを設定。
	lightOutput.SetComputeRootDescriptorTalbe(2);
	aoOutput.SetComputeRootDescriptorTalbe(3);
	colorOutput.SetComputeRootDescriptorTalbe(4);
	giOutput.SetComputeRootDescriptorTalbe(5);
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());

	// パイプラインを設定。
	DirectXBase::Ins()->cmdList->SetPipelineState1(setPipline.GetStateObject().Get());

	// レイトレーシングを実行。
	D3D12_DISPATCH_RAYS_DESC rayDesc = setPipline.GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList->DispatchRays(&rayDesc);

	// デバッグ用のパイプラインがデノイズ用パイプラインだったら、コンピュートシェーダーを使ってデノイズをかける。
	if (debugPiplineID == DENOISE_AO_PIPLINE) {

		// [ノイズを描画]のときはデノイズをかけない。
		if (!constBufferData.isNoiseScene) {

			// デバッグ機能で[法線描画][メッシュ描画][ライトに当たった点のみ描画]のときはデノイズをかけないようにする。
			if (!constBufferData.isMeshScene && !constBufferData.isNormalScene && !constBufferData.isLightHitScene) {

				// ライトにデノイズをかける。
				Denoiser::Ins()->Denoise(lightOutput.GetUAVIndex(), 1, 3);

			}

			// [AOを行わない]のときはデノイズをかけない。
			if (!constBufferData.isNoAO) {

				// AOにデノイズをかける。
				Denoiser::Ins()->Denoise(aoOutput.GetUAVIndex(), 100, 9);

			}

			// [GIを行わない]のときはデノイズをかけない。
			if (!constBufferData.isNoGI) {

				// GIにデノイズをかける。
				Denoiser::Ins()->Denoise(giOutput.GetUAVIndex(), 100, 5);

			}

		}

		// デノイズをかけたライティング情報と色情報を混ぜる。
		denoiseMixTextureOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		Denoiser::Ins()->MixColorAndLuminance(colorOutput.GetUAVIndex(), aoOutput.GetUAVIndex(), lightOutput.GetUAVIndex(), giOutput.GetUAVIndex(), denoiseMixTextureOutput.GetUAVIndex());
		denoiseMixTextureOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

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
	if (constBufferData.isLightHitScene || constBufferData.isMeshScene || constBufferData.isNormalScene) {

		// デノイズされた通常の描画
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), lightOutput.GetRaytracingOutput().Get());

	}
	else {

		// デノイズされた通常の描画
		colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseMixTextureOutput.GetRaytracingOutput().Get());

	}

	// レンダーターゲットのリソースバリアをもとに戻す。
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	// バリアを設定し各リソースの状態を遷移させる.
	aoOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	DirectXBase::Ins()->processAfterDrawing();

}

void DevDXR::Input(KariConstBufferData& ConstBufferData, bool& IsMoveLight, DEGU_PIPLINE_ID& DebugPiplineID) {

	bool isMove = false;

	float speed = 10.0f;
	float rot = 0.03f;
	if (Input::Ins()->isKey(DIK_W)) {
		Camera::Ins()->Move(speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_S)) {
		Camera::Ins()->Move(-speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_A)) {
		Camera::Ins()->MoveRight(speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_D)) {
		Camera::Ins()->MoveRight(-speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_UP)) {
		Camera::Ins()->forwardVec.y += rot;
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_DOWN)) {
		Camera::Ins()->forwardVec.y -= rot;
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_LEFT)) {
		Camera::Ins()->AddRotationXZ(rot);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_RIGHT)) {
		Camera::Ins()->AddRotationXZ(-rot);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_LSHIFT)) {
		Camera::Ins()->eye.y -= 10.0f;
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_SPACE)) {
		Camera::Ins()->eye.y += 10.0f;
		isMove = true;
	}

	InputImGUI(ConstBufferData, IsMoveLight, DebugPiplineID, isMove);

}

void DevDXR::InputImGUI(KariConstBufferData& ConstBufferData, bool& IsMoveLight, DEGU_PIPLINE_ID& DebugPiplineID, bool& IsMove)
{

	// DirLightについて
	if (ImGui::TreeNode("DirLight")) {

		// ライトを表示するかどうかのフラグを更新。
		bool isActive = static_cast<bool>(ConstBufferData.dirLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(ConstBufferData.dirLight.isActive)) IsMove = true;
		ConstBufferData.dirLight.isActive = static_cast<int>(isActive);

		// 値を保存する。
		float dirX = ConstBufferData.dirLight.lihgtDir.x;
		float dirY = ConstBufferData.dirLight.lihgtDir.y;
		float dirZ = ConstBufferData.dirLight.lihgtDir.z;
		ImGui::SliderFloat("DirLightX", &ConstBufferData.dirLight.lihgtDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightY", &ConstBufferData.dirLight.lihgtDir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightZ", &ConstBufferData.dirLight.lihgtDir.z, -1.0f, 1.0f);

		// 変わっていたら
		if (dirX != ConstBufferData.dirLight.lihgtDir.x || dirY != ConstBufferData.dirLight.lihgtDir.y || dirZ != ConstBufferData.dirLight.lihgtDir.z) {

			IsMove = true;
			IsMoveLight = true;

		}

		DebugPiplineID;

		// 正規化する。
		ConstBufferData.dirLight.lihgtDir.Normalize();

		// ライトの色を設定。
		std::array<float, 3> lightColor = { ConstBufferData.dirLight.lightColor.x,ConstBufferData.dirLight.lightColor.y,ConstBufferData.dirLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// 色が変わっていたら。
		if (lightColor[0] != ConstBufferData.dirLight.lightColor.x || lightColor[1] != ConstBufferData.dirLight.lightColor.y || lightColor[2] != ConstBufferData.dirLight.lightColor.z) {
			IsMove = true;
		}
		ConstBufferData.dirLight.lightColor.x = lightColor[0];
		ConstBufferData.dirLight.lightColor.y = lightColor[1];
		ConstBufferData.dirLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}

	// PointLightについて
	if (ImGui::TreeNode("PointLight")) {

		// ライトを表示するかどうかのフラグを更新。
		bool isActive = static_cast<bool>(ConstBufferData.pointLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(ConstBufferData.pointLight.isActive)) IsMove = true;
		ConstBufferData.pointLight.isActive = static_cast<int>(isActive);

		// 値を保存する。
		float dirX = ConstBufferData.pointLight.lightPos.x;
		float dirY = ConstBufferData.pointLight.lightPos.y;
		float dirZ = ConstBufferData.pointLight.lightPos.z;
		float lightSize = ConstBufferData.pointLight.lightSize;
		float aoSampleCount = static_cast<float>(ConstBufferData.aoSampleCount);
		float pointLightPower = ConstBufferData.pointLight.lightPower;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("PointLightX", &ConstBufferData.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightY", &ConstBufferData.pointLight.lightPos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("PointLightZ", &ConstBufferData.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightRadius", &ConstBufferData.pointLight.lightSize, 1.0f, 50.0f);
		ImGui::SliderFloat("PointLightPower", &ConstBufferData.pointLight.lightPower, 300.0f, 1000.0f);
		ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
		ConstBufferData.aoSampleCount = static_cast<int>(aoSampleCount);

		// 変わっていたら
		if (dirX != ConstBufferData.pointLight.lightPos.x || dirY != ConstBufferData.pointLight.lightPos.y || dirZ != ConstBufferData.pointLight.lightPos.z || lightSize != ConstBufferData.pointLight.lightSize || pointLightPower != ConstBufferData.pointLight.lightPower) {

			IsMove = true;
			IsMoveLight = true;

		}

		// ライトの色を設定。
		std::array<float, 3> lightColor = { ConstBufferData.pointLight.lightColor.x,ConstBufferData.pointLight.lightColor.y,ConstBufferData.pointLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// 色が変わっていたら。
		if (lightColor[0] != ConstBufferData.pointLight.lightColor.x || lightColor[1] != ConstBufferData.pointLight.lightColor.y || lightColor[2] != ConstBufferData.pointLight.lightColor.z) {
			IsMove = true;
		}
		ConstBufferData.pointLight.lightColor.x = lightColor[0];
		ConstBufferData.pointLight.lightColor.y = lightColor[1];
		ConstBufferData.pointLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}


	if (IsMove) {
		ConstBufferData.counter = 0;
	}
	else {
		++ConstBufferData.counter;
	}

	// 階層構造にする。
	if (ImGui::TreeNode("Debug")) {

		// メッシュを表示する。
		bool isMesh = ConstBufferData.isMeshScene;
		bool prevIsMesh = isMesh;
		ImGui::Checkbox("Mesh Scene", &isMesh);
		ConstBufferData.isMeshScene = isMesh;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isMesh != prevIsMesh) {
			ConstBufferData.counter = 0;
		}

		// 法線を表示する。
		bool isNormal = ConstBufferData.isNormalScene;
		bool prevIsNormal = isNormal;
		ImGui::Checkbox("Normal Scene", &isNormal);
		ConstBufferData.isNormalScene = isNormal;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isNormal != prevIsNormal) {
			ConstBufferData.counter = 0;
		}

		// ライトがあたった面だけ表示するフラグを更新。
		bool isLightHit = ConstBufferData.isLightHitScene;
		bool prevIsLightHit = isLightHit;
		ImGui::Checkbox("LightHit Scene", &isLightHit);
		ConstBufferData.isLightHitScene = isLightHit;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isLightHit != prevIsLightHit) {
			ConstBufferData.counter = 0;
		}

		// デバッグ用でノイズ画面を出すためのフラグをセット。
		bool isNoise = ConstBufferData.isNoiseScene;
		ImGui::Checkbox("Noise Scene", &isNoise);
		ConstBufferData.isNoiseScene = isNoise;

		// AOを行うかのフラグをセット。
		bool isNoAO = ConstBufferData.isNoAO;
		ImGui::Checkbox("NoAO Scene", &isNoAO);
		ConstBufferData.isNoAO = isNoAO;

		// GIを行うかのフラグをセット。
		bool isNoGI = ConstBufferData.isNoGI;
		ImGui::Checkbox("NoGI Scene", &isNoGI);
		ConstBufferData.isNoGI = isNoGI;

		// GIのみを描画するかのフラグをセット。
		bool isGIOnlyScene = ConstBufferData.isGIOnlyScene;
		ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene);
		ConstBufferData.isGIOnlyScene = isGIOnlyScene;

		// FPSを表示するかのフラグをセット。
		ImGui::Checkbox("Display FPS", &isDisplayFPS);


		ImGui::TreePop();

	}

	// 階層構造にする。
	if (ImGui::TreeNode("AS")) {

		// 太陽光線の強さを設定する。
		ImGui::SliderFloat("Sun Power", &ConstBufferData.AS.eSun, -10, 100);

		// レイリー散乱定数の値を設定する。
		ImGui::SliderFloat("Rayleigh Scattering Power", &ConstBufferData.AS.kr, -1, 1);

		// ミー散乱定数の値を設定する。
		ImGui::SliderFloat("Mie Scattering Power", &ConstBufferData.AS.km, -1, 1);

		// サンプリング数を設定する。
		ImGui::SliderFloat("Sample Count", &ConstBufferData.AS.samples, 0, 10);

		// 大気圏の一番上の高さ
		ImGui::SliderFloat("Outer Radius", &ConstBufferData.AS.outerRadius, 0, 20000);

		// 地上の高さ
		ImGui::SliderFloat("Inner Radius", &ConstBufferData.AS.innerRadius, 0, 20000);

		// 大気散乱を求める際に使用する定数
		ImGui::SliderFloat("Scattering G", &ConstBufferData.AS.g, -1.0f, 1.0f);

		// 平均大気密度を求めるための高さ
		ImGui::SliderFloat("Aveheight", &ConstBufferData.AS.aveHeight, 0.0f, 1.0f);

		ImGui::TreePop();

	}

}

#pragma comment (lib, "winmm.lib")

// タイトルバーのFPSの更新
void DevDXR::FPS()
{
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