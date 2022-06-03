#include "SoundManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Input.h"
#include "FbxLoader.h"

#include "RayDenoiser.h"
#include "BakeAO.h"

#include "BLASRegister.h"
#include "PorygonInstance.h"
#include "TLAS.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "DescriptorHeapMgr.h"
#include "Vec.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "RaytracingPipline.h"
#include "RaytracingOutput.h"
#include "MultiMeshLoadOBJ.h"
#include "ComputeShader.h"
#include "BLAS.h"

#include "HitGroup.h"

#include <utilapiset.h>

#include "KariConstBuffer.h"

#include "FHelper.h"

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

// fps更新
void FPS();

// デバッグ用のパイプラインを切り替えるやつ。
enum DEGU_PIPLINE_ID {
	DEF_PIPLINE,
	AO_PIPLINE,
	DENOISE_AO_PIPLINE,
};

// 入力操作
void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID);
void InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID, bool& isMove);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX初期化処理----------*/
	//ImGuiWindow::Ins()->Init();
	DirectXBase::Ins()->Init();									// DirectX基盤の初期化
	SoundManager::Ins()->SettingSoundManager();	// サウンドマネージャーをセットする

	/*----------パイプライン生成----------*/
	PiplineManager::Ins()->Init();

	/*----------変数宣言----------*/
	srand(time(NULL));

	// ディスクリプタヒープを初期化。
	DescriptorHeapMgr::Ins()->GenerateDescriptorHeap();

	// FBXLoaderを初期化。
	FbxLoader::Ins()->Init();

	// ヒットグループを設定。
	HitGroupMgr::Ins()->Setting();

	// デノイズ用のクラスを初期化。
	Denoiser::Ins()->Setting();

	// カメラを初期化。
	Camera::Ins()->Init();

	// 仮の定数バッファを宣言
	KariConstBufferData constBufferData;
	constBufferData.Init();

	// AO用のパイプラインを設定。
	vector<RayPiplineShaderData> useShaders;
	useShaders.push_back({ "Resource/ShaderFiles/RayTracing/AOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline aoPipline;
	aoPipline.Setting(useShaders, HitGroupMgr::AO_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// デノイズAO用のパイプラインを設定。
	vector<RayPiplineShaderData> dAOuseShaders;
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline deAOPipline;
	deAOPipline.Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// デフォルトのシェーダーを設定。
	vector<RayPiplineShaderData> defShaders;
	defShaders.push_back({ "Resource/ShaderFiles/RayTracing/TriangleShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline defPipline;
	defPipline.Setting(defShaders, HitGroupMgr::DEF_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZAを読み込む。
	//std::vector<int> sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP]);

	// ライト用のスフィアを読み込む。
	int sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::AO_HIT_GROUP], { L"Resource/white.png" });
	int sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 3);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLASを生成。
	TLAS tlas;
	tlas.GenerateTLAS(L"TlasDescriptorHeap");

	// AOをベイク。
	RaytracingOutput bakeTex;
	bakeTex.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	BakeAO::Ins()->ExecutionBake(BLASRegister::Ins()->GetBLASCount(), tlas, constBufferData.constBuff.GetBuffer(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex()), bakeTex);

	// ベイクした結果のUAVをBLASに追加。
	int counter = 0;
	std::vector<std::shared_ptr<BLAS>>& blasBuff = BLASRegister::Ins()->GetBLAS();
	for (auto& index : blasBuff) {
		//BakeAO::Ins()->GetBakeTex()[counter]->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		index->AddTex(BakeAO::Ins()->GetBakeTex()[counter]->GetUAVIndex());
		++counter;
	}

	// レイトレ出力用クラスをセット。
	RaytracingOutput raytracingOutput;
	raytracingOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// レイトレ出力用クラスをセット。
	RaytracingOutput colorOutput;
	colorOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// デノイズの結果出力用クラスをセット。
	RaytracingOutput denoiseResultOutput;
	denoiseResultOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 累積デノイズ用での保存用クラス。
	RaytracingOutput raytracingOutputData;
	raytracingOutputData.Setting(DXGI_FORMAT_R32G32B32A32_FLOAT);

	// シェーダーテーブルを生成。
	aoPipline.ConstructionShaderTable();
	deAOPipline.ConstructionShaderTable();
	defPipline.ConstructionShaderTable();

	// デノイザー受け取り用
	RaytracingOutput denoiseOutput;
	denoiseOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	RaytracingOutput denoiseOutput1;
	denoiseOutput1.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	RaytracingOutput denoiseOutput2;
	denoiseOutput2.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);


	// デバッグ用でノイズ画面を出すフラグ。
	DEGU_PIPLINE_ID debugPiplineID = DENOISE_AO_PIPLINE;

	// ライトが動いたか
	bool isMoveLight = false;

	for (int index = 0; index < PorygonInstanceRegister::Ins()->GetRegisterSize(); ++index) {

		PorygonInstanceRegister::Ins()->AddRotate(index, { 0.1f,0.1f,0.1f });

	}

	/*----------ゲームループ----------*/
	while (true) {

		// IMGUI系
		//ImGuiWindow::Ins()->processBeforeDrawing();

		// ウィンドウの名前を再設定。
		//SetWindowText(ImGuiWindow::Ins()->windowsAPI.hwnd, L"ImGuiWindow");

		//isMoveLight = false;
		//Input(constBufferData, isMoveLight, debugPiplineID);

		//ImGuiWindow::Ins()->processAfterDrawing();


		/*----------毎フレーム処理(描画前処理)----------*/
		DirectXBase::Ins()->processBeforeDrawing();

		/*----- 更新処理 -----*/

		// ビュー行列を生成。
		Camera::Ins()->GenerateMatView();

		FPS();

		// 乱数の種を更新。
		constBufferData.seed = FHelper::GetRand(0, 1000);

		// カメラを更新。
		Camera::Ins()->Update();

		constBufferData.eye = Camera::Ins()->eye;
		constBufferData.target = Camera::Ins()->target;
		constBufferData.up = Camera::Ins()->up;

		// ライトが動いたときのみ、ワールド行列を再計算してTLASを更新する。
		if (isMoveLight) {

			// 点光源の位置を更新。
			PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.pointLight.lightPos);
			PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.pointLight.lightSize);

			tlas.Update();

		}

		/*----- 描画処理 -----*/

		// 画面に表示されるレンダーターゲットに戻す。
		//DirectXBase::Ins()->SetRenderTarget();

		RaytracingPipline setPipline = {};

		// デバッグ用のパイプラインIDに応じたパイプラインをセットする。
		if (debugPiplineID == DEF_PIPLINE) {

			constBufferData.counter = 0;
			setPipline = defPipline;

		}
		else if (debugPiplineID == AO_PIPLINE) {

			setPipline = aoPipline;

		}
		else if (debugPiplineID == DENOISE_AO_PIPLINE) {

			setPipline = deAOPipline;

		}

		auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
		constBufferData.mtxView = XMMatrixLookAtLH(constBufferData.eye.ConvertXMVECTOR(), constBufferData.target.ConvertXMVECTOR(), constBufferData.up.ConvertXMVECTOR());
		constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
		// 定数バッファの中身を更新する。
		constBufferData.constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
		auto sceneConstantBuffer = constBufferData.constBuff.GetBuffer(frameIndex);

		// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Ins()->cmdList->SetComputeRootSignature(setPipline.GetGlobalRootSig()->GetRootSig().Get());
		DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));
		raytracingOutput.SetComputeRootDescriptorTalbe(2);
		// デノイズパイプライン以外だったら
		if (debugPiplineID != DENOISE_AO_PIPLINE) {
			raytracingOutputData.SetComputeRootDescriptorTalbe(3);
		}
		else {
			colorOutput.SetComputeRootDescriptorTalbe(3);
		}
		DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());


		// レイトレーシング結果バッファをUAV状態へ
		raytracingOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		raytracingOutputData.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		DirectXBase::Ins()->cmdList->SetPipelineState1(setPipline.GetStateObject().Get());

		DirectXBase::Ins()->cmdList->DispatchRays(&setPipline.GetDispatchRayDesc());

		// デバッグ用のパイプラインがデノイズ用パイプラインだったら、コンピュートシェーダーを使ってデノイズをかける。
		if (debugPiplineID == DENOISE_AO_PIPLINE) {

			// デノイズをかける。
			Denoiser::Ins()->ApplyGaussianBlur(raytracingOutput.GetUAVIndex(), denoiseOutput.GetUAVIndex(), 10);
			Denoiser::Ins()->ApplyGaussianBlur(denoiseOutput.GetUAVIndex(), denoiseOutput1.GetUAVIndex(), 10);
			Denoiser::Ins()->ApplyGaussianBlur(denoiseOutput1.GetUAVIndex(), denoiseOutput2.GetUAVIndex(), 10);

			// デノイズをかけたライティング情報と色情報を混ぜる。
			denoiseResultOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			Denoiser::Ins()->MixColorAndLuminance(colorOutput.GetUAVIndex(), bakeTex.GetUAVIndex(), denoiseResultOutput.GetUAVIndex());
			denoiseResultOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

		}


		// バックバッファのインデックスを取得する。
		UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

		// バリアを設定し各リソースの状態を遷移させる.
		raytracingOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		raytracingOutputData.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_DEST),
		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(barriers), barriers);

		// 通常パイプラインだったら
		if (debugPiplineID != DENOISE_AO_PIPLINE) {
			DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), raytracingOutput.GetRaytracingOutput().Get());
		}
		else {
			DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseResultOutput.GetRaytracingOutput().Get());
		}

		// レンダーターゲットのリソースバリアをもとに戻す。
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET)

		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

		DirectXBase::Ins()->processAfterDrawing();

	}

	return 0;
}


// タイトルバーのFPSの更新
void FPS()
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

void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID) {

	bool isMove = false;

	float speed = 5.0f;
	float rot = 0.03f;
	if (Input::isKey(DIK_W)) {
		Camera::Ins()->Move(speed);
		isMove = true;
	}
	if (Input::isKey(DIK_S)) {
		Camera::Ins()->Move(-speed);
		isMove = true;
	}
	if (Input::isKey(DIK_A)) {
		Camera::Ins()->MoveRight(speed);
		isMove = true;
	}
	if (Input::isKey(DIK_D)) {
		Camera::Ins()->MoveRight(-speed);
		isMove = true;
	}
	if (Input::isKey(DIK_UP)) {
		Camera::Ins()->forwardVec.y += rot;
		isMove = true;
	}
	if (Input::isKey(DIK_DOWN)) {
		Camera::Ins()->forwardVec.y -= rot;
		isMove = true;
	}
	if (Input::isKey(DIK_LEFT)) {
		Camera::Ins()->AddRotationXZ(rot);
		isMove = true;
	}
	if (Input::isKey(DIK_RIGHT)) {
		Camera::Ins()->AddRotationXZ(-rot);
		isMove = true;
	}
	if (Input::isKey(DIK_LSHIFT)) {
		Camera::Ins()->eye.y -= 10.0f;
		isMove = true;
	}
	if (Input::isKey(DIK_SPACE)) {
		Camera::Ins()->eye.y += 10.0f;
		isMove = true;
	}

	//InputImGUI(constBufferData, isMoveLight, debugPiplineID, isMove);

}

void InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID, bool& isMove)
{

	// DirLightについて
	if (ImGui::TreeNode("DirLight")) {

		// ライトを表示するかどうかのフラグを更新。
		bool isActive = static_cast<bool>(constBufferData.dirLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != constBufferData.dirLight.isActive) isMove = true;
		constBufferData.dirLight.isActive = static_cast<int>(isActive);

		// 値を保存する。
		float dirX = constBufferData.dirLight.lihgtDir.x;
		float dirZ = constBufferData.dirLight.lihgtDir.z;
		ImGui::SliderFloat("DirLightX", &constBufferData.dirLight.lihgtDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightZ", &constBufferData.dirLight.lihgtDir.z, -1.0f, 1.0f);

		// 変わっていたら
		if (dirX != constBufferData.dirLight.lihgtDir.x || dirZ != constBufferData.dirLight.lihgtDir.z) {

			isMove = true;
			isMoveLight = true;

		}

		// 正規化する。
		constBufferData.dirLight.lihgtDir.Normalize();

		// ライトの色を設定。
		array<float, 3> lightColor = { constBufferData.dirLight.lightColor.x,constBufferData.dirLight.lightColor.y,constBufferData.dirLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// 色が変わっていたら。
		if (lightColor[0] != constBufferData.dirLight.lightColor.x || lightColor[1] != constBufferData.dirLight.lightColor.y || lightColor[2] != constBufferData.dirLight.lightColor.z) {
			isMove = true;
		}
		constBufferData.dirLight.lightColor.x = lightColor[0];
		constBufferData.dirLight.lightColor.y = lightColor[1];
		constBufferData.dirLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}

	// PointLightについて
	if (ImGui::TreeNode("PointLight")) {

		// ライトを表示するかどうかのフラグを更新。
		bool isActive = static_cast<bool>(constBufferData.pointLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != constBufferData.pointLight.isActive) isMove = true;
		constBufferData.pointLight.isActive = static_cast<int>(isActive);

		// 値を保存する。
		float dirX = constBufferData.pointLight.lightPos.x;
		float dirY = constBufferData.pointLight.lightPos.y;
		float dirZ = constBufferData.pointLight.lightPos.z;
		float lightSize = constBufferData.pointLight.lightSize;
		float aoSampleCount = constBufferData.aoSampleCount;
		float pointLightPower = constBufferData.pointLight.lightPower;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("PointLightX", &constBufferData.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightY", &constBufferData.pointLight.lightPos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("PointLightZ", &constBufferData.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightRadius", &constBufferData.pointLight.lightSize, 1.0f, 50.0f);
		ImGui::SliderFloat("PointLightPower", &constBufferData.pointLight.lightPower, 300.0f, 1000.0f);
		ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
		constBufferData.aoSampleCount = aoSampleCount;

		// 変わっていたら
		if (dirX != constBufferData.pointLight.lightPos.x || dirY != constBufferData.pointLight.lightPos.y || dirZ != constBufferData.pointLight.lightPos.z || lightSize != constBufferData.pointLight.lightSize || pointLightPower != constBufferData.pointLight.lightPower) {

			isMove = true;
			isMoveLight = true;

		}

		// ライトの色を設定。
		array<float, 3> lightColor = { constBufferData.pointLight.lightColor.x,constBufferData.pointLight.lightColor.y,constBufferData.pointLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// 色が変わっていたら。
		if (lightColor[0] != constBufferData.pointLight.lightColor.x || lightColor[1] != constBufferData.pointLight.lightColor.y || lightColor[2] != constBufferData.pointLight.lightColor.z) {
			isMove = true;
		}
		constBufferData.pointLight.lightColor.x = lightColor[0];
		constBufferData.pointLight.lightColor.y = lightColor[1];
		constBufferData.pointLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}

	// SpotLightについて
	if (ImGui::TreeNode("SpotLight")) {

		// ライトを表示するかどうかのフラグを更新。
		bool isActive = static_cast<bool>(constBufferData.spotLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != constBufferData.spotLight.isActive) isMove = true;
		constBufferData.spotLight.isActive = static_cast<int>(isActive);

		// 値を保存する。
		float posX = constBufferData.spotLight.pos.x;
		float posY = constBufferData.spotLight.pos.y;
		float posZ = constBufferData.spotLight.pos.z;
		float dirX = constBufferData.spotLight.dir.x;
		float dirY = constBufferData.spotLight.dir.y;
		float dirZ = constBufferData.spotLight.dir.z;
		float angle = constBufferData.spotLight.angle;
		float power = constBufferData.spotLight.power;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("SpotLightX", &constBufferData.spotLight.pos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("SpotLightY", &constBufferData.spotLight.pos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("SpotLightZ", &constBufferData.spotLight.pos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("SpotLightDirX", &constBufferData.spotLight.dir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("SpotLightDirY", &constBufferData.spotLight.dir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("SpotLightDirZ", &constBufferData.spotLight.dir.z, -1.0f, 1.0f);
		ImGui::SliderFloat("SpotLightAngle", &constBufferData.spotLight.angle, 0.001f, DirectX::XM_PI * 2.0f);
		ImGui::SliderFloat("SpotLightPower", &constBufferData.spotLight.power, 300.0f, 1000.0f);

		// 変わっていたら
		if (posX != constBufferData.spotLight.pos.x || posY != constBufferData.spotLight.pos.y || posZ != constBufferData.spotLight.pos.z ||
			power != constBufferData.spotLight.power || angle != constBufferData.spotLight.angle ||
			dirX != constBufferData.spotLight.dir.x || dirY != constBufferData.spotLight.dir.y || dirZ != constBufferData.spotLight.dir.z) {

			isMove = true;
			isMoveLight = true;
			constBufferData.spotLight.dir.Normalize();

		}

		// ライトの色を設定。
		array<float, 3> lightColor = { constBufferData.spotLight.color.x,constBufferData.spotLight.color.y,constBufferData.spotLight.color.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// 色が変わっていたら。
		if (lightColor[0] != constBufferData.spotLight.color.x || lightColor[1] != constBufferData.spotLight.color.y || lightColor[2] != constBufferData.spotLight.color.z) {
			isMove = true;
		}
		constBufferData.spotLight.color.x = lightColor[0];
		constBufferData.spotLight.color.y = lightColor[1];
		constBufferData.spotLight.color.z = lightColor[2];

		ImGui::TreePop();

	}

	if (isMove) {
		constBufferData.counter = 0;
	}
	else {
		++constBufferData.counter;
	}

	// 階層構造にする。
	if (ImGui::TreeNode("Debug")) {

		// メッシュを表示する。
		bool isMesh = constBufferData.isMeshScene;
		bool prevIsMesh = isMesh;
		ImGui::Checkbox("Mesh Scene", &isMesh);
		constBufferData.isMeshScene = isMesh;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isMesh != prevIsMesh) {
			constBufferData.counter = 0;
		}

		// 法線を表示する。
		bool isNormal = constBufferData.isNormalScene;
		bool prevIsNormal = isNormal;
		ImGui::Checkbox("Normal Scene", &isNormal);
		constBufferData.isNormalScene = isNormal;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isNormal != prevIsNormal) {
			constBufferData.counter = 0;
		}

		// ライトがあたった面だけ表示するフラグを更新。
		bool isLightHit = constBufferData.isLightHitScene;
		bool prevIsLightHit = isLightHit;
		ImGui::Checkbox("LightHit Scene", &isLightHit);
		constBufferData.isLightHitScene = isLightHit;
		// 値が書き換えられていたら、サンプリングを初期化する。
		if (isLightHit != prevIsLightHit) {
			constBufferData.counter = 0;
		}

		// パイプラインを選択。
		int debugPiplineBuff = debugPiplineID;
		ImGui::RadioButton("DEF PIPLINE", &debugPiplineBuff, 0);
		ImGui::SameLine();
		ImGui::RadioButton("AO PIPLINE", &debugPiplineBuff, 1);
		ImGui::SameLine();
		ImGui::RadioButton("DENOISE AO PIPLINE", &debugPiplineBuff, 2);
		debugPiplineID = (DEGU_PIPLINE_ID)debugPiplineBuff;

		// AOのパイプラインを選択されていたときのみ、ノイズを出すかのフラグを表示する。
		if (debugPiplineID == AO_PIPLINE) {

			// デバッグ用でノイズ画面を出すためのフラグをセット。
			bool isNoise = constBufferData.isNoiseScene;
			ImGui::Checkbox("Noise Scene", &isNoise);
			constBufferData.isNoiseScene = isNoise;

			// デバッグ用でノイズ画面のみを出すためのフラグをセット。
			bool isNoiseOnly = constBufferData.isNoiseOnlyScene;
			ImGui::Checkbox("NoiseOnly Scene", &isNoiseOnly);
			// フラグが書き換わっていたらデノイズカウンターを初期化する。
			if (isNoiseOnly != constBufferData.isNoiseOnlyScene) {
				constBufferData.counter = 0;
			}
			constBufferData.isNoiseOnlyScene = isNoiseOnly;

			// アンビエントオクリュージョンを行うかのフラグをセット。
			bool isNoAO = constBufferData.isNoAO;
			ImGui::Checkbox("NoAO Scene", &isNoAO);
			// フラグが書き換わっていたらデノイズカウンターを初期化する。
			if (isNoAO != constBufferData.isNoAO) {
				constBufferData.counter = 0;
			}
			constBufferData.isNoAO = isNoAO;

		}

		ImGui::TreePop();

	}

}


/*

◯　やりたいこと・やらなければいけないこと
//・ソフトシャドウのデバッグ情報をわかりやすくする。
//　→光源の位置の表示非表示をわかりやすくする。
//　→光源によって照らされた位置のみを表示できるようにする。
//・そのためにはパイプラインをきちんと複数作れるようにする必要がある？
//・AOのバグを修正。明るさが場所によって違う？
//・上が終わったら、正規化ランバートについて学ぶ。
//　→どうして正規化ランバートを使うのかまできちんと理解する。
//・リニアワークフローを実装する。
//　→ガンマ値補正？
//・ライティングのバグを修正する。
//・軽量化の処理を入れる。
・リアルタイムデノイズを実装する。
//　→なぜサンプル数を増やして平均しても色が変わらないのか。そもそも変わっているのか？ → 乱数の種が変わってなかったから。
　→以前見た輝度に重みをおいたブラーでは実装できないかも。
　→ノイズが01なので、逆に輝度が大きすぎる…
　→逆に言えば、01の輝度の差があるところなんてノイズしかありえない？やってみよう！
・以上のことを今週までに実装する。



・UAVを書き込む位置？
・まずはUAVにしてPIXでみる。

*/