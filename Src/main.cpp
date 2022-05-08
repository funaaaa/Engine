#include "SoundManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Input.h"
#include "FbxLoader.h"

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

#include "HitGroup.h"

#include <utilapiset.h>

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

// fps更新
void FPS();

struct KariConstBufferData {

	XMMATRIX mtxView;			// ビュー行列。
	XMMATRIX mtxProj;			// プロジェクション行列。
	XMMATRIX mtxViewInv;		// ビュー逆行列。
	XMMATRIX mtxProjInv;		// プロジェクション逆行列。
	XMVECTOR lightDirection;	// 平行光源の向き。
	XMVECTOR lightColor;		// 平行光源色。
	XMVECTOR ambientColor;		// 環境光。

};


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX初期化処理----------*/
	DirectXBase directXBase;							// DirectX基盤部分
	directXBase.Init();									// DirectX基盤の初期化
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

	// 使用するシェーダーを列挙。
	vector<RayPiplineShaderData> useShaders;
	useShaders.push_back({ "Resource/ShaderFiles/RayTracing/TriangleShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS"} });

	// レイトレパイプラインを設定。
	RaytracingPipline pipline;
	pipline.Setting(useShaders);

	// SPONZAを読み込む。
	std::vector<int> sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiLeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP]);

	// 猿のBLASを生成。
	//int boneBlas = BLASRegister::Ins()->GenerateFbx("Resource/", "boneTest.fbx", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP], L"Resource/backGround.png");

	// 猿のBLASを生成。
	//int monkeyBlas = BLASRegister::Ins()->GenerateObj("Resource/", "monkey.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP], L"Resource/backGround.png");

	// 床のBLASを生成。
	//int groundBlas = BLASRegister::Ins()->GenerateObj("Resource/", "ground.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP], L"Resource/Fine_Basin.jpg");

	// インスタンスを生成
	//int boneA = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int boneB = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int boneC = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int boneD = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int monkey = PorygonInstanceRegister::Ins()->CreateInstance(monkeyBlas, 2);
	//int ground = PorygonInstanceRegister::Ins()->CreateInstance(groundBlas, 2);

	// 移動させる。
	float addTrans = 200.0f;
	//PorygonInstanceRegister::Ins()->AddTrans(boneA, -addTrans, 0.0f, 0);
	//PorygonInstanceRegister::Ins()->AddTrans(boneB, addTrans, 0.0f, 0);
	//PorygonInstanceRegister::Ins()->AddTrans(boneC, 0.0f, 0.0f, addTrans);
	//PorygonInstanceRegister::Ins()->AddTrans(boneD, 0.0f, 0.0f, -addTrans);
	//PorygonInstanceRegister::Ins()->AddTrans(monkey, 0.0f, 0.0f, 0);
	//PorygonInstanceRegister::Ins()->AddTrans(ground, 0.0f, -1.0f, 0);

	// ある程度回転させる。
	//PorygonInstanceRegister::Ins()->AddRotate(boneA, 0.0f, -0.5f, 0);
	//PorygonInstanceRegister::Ins()->AddRotate(boneB, 0.0f, -0.5f, 0);
	//PorygonInstanceRegister::Ins()->AddRotate(boneC, 0.0f, -0.5f, 0);
	//PorygonInstanceRegister::Ins()->AddRotate(boneD, 0.0f, -0.5f, 0);

	// 拡大させる。
	//PorygonInstanceRegister::Ins()->AddScale(boneA, Vec3(70));
	//PorygonInstanceRegister::Ins()->AddScale(boneB, Vec3(70));
	//PorygonInstanceRegister::Ins()->AddScale(boneC, Vec3(70));
	//PorygonInstanceRegister::Ins()->AddScale(boneD, Vec3(70));

	// TLASを生成。
	TLAS tlas;
	tlas.GenerateTLAS(L"TlasDescriptorHeap");

	// レイトレ出力用クラスをセット。
	RaytracingOutput raytracingOutput;
	raytracingOutput.Setting();

	// シェーダーテーブルを生成。
	pipline.ConstructionShaderTable();


	// 仮の定数バッファを宣言
	KariConstBufferData constBufferData;
	constBufferData.ambientColor = { 1,1,1,1 };
	constBufferData.lightColor = { 1,1,1,1 };
	constBufferData.lightDirection = { 0,1,-0.27f,0 };
	constBufferData.lightDirection = XMVector4Normalize(constBufferData.lightDirection);
	constBufferData.mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//画角(60度)
		(float)window_width / window_height,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);
	constBufferData.mtxProjInv = XMMatrixInverse(nullptr, constBufferData.mtxProj);
	Vec3 eye = { 0,0,-10 };
	Vec3 target = { 0,0,0 };
	Vec3 up = { 0,1,0 };
	constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);

	DynamicConstBuffer constBuff;
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");

	// カメラを初期化。
	Camera::Ins()->Init();


	/*----------ゲームループ----------*/
	while (true) {
		/*----------毎フレーム処理(描画前処理)----------*/
		directXBase.processBeforeDrawing();



		/*----- 更新処理 -----*/

		// ビュー行列を生成。
		Camera::Ins()->GenerateMatView();

		FPS();

		//Camera::target = triangle.GetPos();

		// スキニングアニメーションさせる。
		//BLASRegister::Ins()->ComputeSkin(boneBlas);

		float speed = 5.0f;
		float rot = 0.03f;
		if (Input::isKey(DIK_W)) Camera::Ins()->Move(speed);
		if (Input::isKey(DIK_S)) Camera::Ins()->Move(-speed);
		if (Input::isKey(DIK_A)) Camera::Ins()->MoveRight(speed);
		if (Input::isKey(DIK_D)) Camera::Ins()->MoveRight(-speed);
		if (Input::isKey(DIK_UP)) Camera::Ins()->forwardVec.y += rot;
		if (Input::isKey(DIK_DOWN)) Camera::Ins()->forwardVec.y -= rot;
		if (Input::isKey(DIK_LEFT)) Camera::Ins()->AddRotationXZ(rot);
		if (Input::isKey(DIK_RIGHT)) Camera::Ins()->AddRotationXZ(-rot);
		if (Input::isKey(DIK_LSHIFT)) Camera::Ins()->eye.y -= 10.0f;
		if (Input::isKey(DIK_SPACE)) Camera::Ins()->eye.y += 10.0f;

		//if (Input::isKey(DIK_I)) porygonInstance[0].AddTrans(0.0f, 0.0f, -0.1f);
		//if (Input::isKey(DIK_K)) porygonInstance[0].AddTrans(0.0f, 0.0f, 0.1f);
		//if (Input::isKey(DIK_J)) porygonInstance[0].AddTrans(0.1f, 0.0f, 0.0f);
		//if (Input::isKey(DIK_L)) porygonInstance[0].AddTrans(-0.1f, 0.0f, 0.0f);

		if (Input::isKey(DIK_1)) {

			//BLASRegister::Ins()->InitAnimation(boneBlas);

		}
		if (Input::isKey(DIK_2)) {

			//BLASRegister::Ins()->PlayAnimation(boneBlas);

		}
		if (Input::isKey(DIK_3)) {

			//BLASRegister::Ins()->StopAnimation(boneBlas);

		}

		//BLASRegister::Ins()->Update(boneBlas);

		// TLASを更新。
		//tlas.Update();

		// カメラを更新。
		Camera::Ins()->Update();

		eye = Camera::Ins()->eye;
		target = Camera::Ins()->target;
		up = Camera::Ins()->up;


		/*----- 描画処理 -----*/

		// 画面に表示されるレンダーターゲットに戻す。
		//DirectXBase::Ins()->SetRenderTarget();

		auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
		constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
		constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
		// 定数バッファの中身を更新する。
		constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
		auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

		// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Ins()->cmdList->SetComputeRootSignature(pipline.GetGlobalRootSig()->GetRootSig().Get());
		DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));
		raytracingOutput.SetComputeRootDescriptorTalbe(2);
		DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());


		// レイトレーシング結果バッファをUAV状態へ
		auto barrierToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutput.GetRaytracingOutput().Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		DirectXBase::Ins()->cmdList->ResourceBarrier(1, &barrierToUAV);

		DirectXBase::Ins()->cmdList->SetPipelineState1(pipline.GetStateObject().Get());

		DirectXBase::Ins()->cmdList->DispatchRays(&pipline.GetDispatchRayDesc());

		// バックバッファのインデックスを取得する。
		UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

		// バリアを設定し各リソースの状態を遷移させる.
		D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.GetRaytracingOutput().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(barriers), barriers);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), raytracingOutput.GetRaytracingOutput().Get());

		// レンダーターゲットのリソースバリアをもとに戻す。
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET)

		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

		directXBase.processAfterDrawing();

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

