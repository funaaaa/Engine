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

#include "FHelper.h"

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
	Vec3 lightPos;
	float lightSize;
	int seed;
	int counter;
	int isNoiseScene;
	int isLightHitScene;
	int isNormalScene;
	int isMeshScene;
	int isNoAO;

};

// デバッグ用のパイプラインを切り替えるやつ。
enum DEGU_PIPLINE_ID {
	DEF_PIPLINE,
	AO_PIPLINE,
};

// 入力操作
void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID);

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

	// AO用のパイプラインを設定。
	vector<RayPiplineShaderData> useShaders;
	useShaders.push_back({ "Resource/ShaderFiles/RayTracing/AOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline aoPipline;
	aoPipline.Setting(useShaders, HitGroupMgr::AO_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// デフォルトのシェーダーを設定。
	vector<RayPiplineShaderData> defShaders;
	defShaders.push_back({ "Resource/ShaderFiles/RayTracing/TriangleShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline defPipline;
	defPipline.Setting(defShaders, HitGroupMgr::DEF_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZAを読み込む。
	std::vector<int> sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP]);

	// ライト用のスフィアを読み込む。
	int sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::AO_HIT_GROUP], { L"Resource/white.png" });
	int sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 3);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLASを生成。
	TLAS tlas;
	tlas.GenerateTLAS(L"TlasDescriptorHeap");

	// レイトレ出力用クラスをセット。
	RaytracingOutput raytracingOutput;
	raytracingOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	RaytracingOutput raytracingOutputBuff;
	raytracingOutputBuff.Setting(DXGI_FORMAT_R32G32B32A32_FLOAT);

	// シェーダーテーブルを生成。
	aoPipline.ConstructionShaderTable();
	defPipline.ConstructionShaderTable();


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
	constBufferData.counter = 0;
	constBufferData.isNoiseScene = false;
	constBufferData.lightPos = Vec3(0, 300, 0);
	constBufferData.lightSize = 30.0f;
	constBufferData.isLightHitScene = false;
	constBufferData.isNormalScene = false;
	constBufferData.isMeshScene = false;
	constBufferData.isNoAO = false;

	DynamicConstBuffer constBuff;
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");

	// デバッグ用でノイズ画面を出すフラグ。
	DEGU_PIPLINE_ID debugPiplineID = AO_PIPLINE;

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

		// 乱数の種を更新。
		constBufferData.seed = FHelper::GetRand(0, 1000);

		// ライトが動いたか
		bool isMoveLight = false;

		Input(constBufferData, isMoveLight, debugPiplineID);

		// カメラを更新。
		Camera::Ins()->Update();

		eye = Camera::Ins()->eye;
		target = Camera::Ins()->target;
		up = Camera::Ins()->up;

		// ライトが動いたときのみ、ワールド行列を再計算してTLASを更新する。
		if (isMoveLight) {

			PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.lightPos);
			PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.lightSize);

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

		auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
		constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
		constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
		// 定数バッファの中身を更新する。
		constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
		auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

		// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Ins()->cmdList->SetComputeRootSignature(setPipline.GetGlobalRootSig()->GetRootSig().Get());
		DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));
		raytracingOutput.SetComputeRootDescriptorTalbe(2);
		raytracingOutputBuff.SetComputeRootDescriptorTalbe(3);
		DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());


		// レイトレーシング結果バッファをUAV状態へ
		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutput.GetRaytracingOutput().Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
			CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutputBuff.GetRaytracingOutput().Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		)
		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(2, barrierToUAV);

		DirectXBase::Ins()->cmdList->SetPipelineState1(setPipline.GetStateObject().Get());

		DirectXBase::Ins()->cmdList->DispatchRays(&setPipline.GetDispatchRayDesc());

		// バックバッファのインデックスを取得する。
		UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

		// バリアを設定し各リソースの状態を遷移させる.
		D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.GetRaytracingOutput().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutputBuff.GetRaytracingOutput().Get(),
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

	// DirLightについて

	// 値を保存する。
	float dirX = constBufferData.lightPos.x;
	float dirY = constBufferData.lightPos.y;
	float dirZ = constBufferData.lightPos.z;
	float lightSize = constBufferData.lightSize;
	float MOVE_LENGTH = 1500.0f;
	ImGui::SliderFloat("PointLightX", &constBufferData.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
	ImGui::SliderFloat("PointLightY", &constBufferData.lightPos.y, 0.0f, 1000.0f);
	ImGui::SliderFloat("PointLightZ", &constBufferData.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
	ImGui::SliderFloat("PointLightRadius", &constBufferData.lightSize, 1.0f, 50.0f);

	// 変わっていたら
	if (dirX != constBufferData.lightPos.x || dirY != constBufferData.lightPos.y || dirZ != constBufferData.lightPos.z || lightSize != constBufferData.lightSize) {

		isMove = true;
		isMoveLight = true;

	}

	if (isMove) {
		constBufferData.counter = 0;
	}
	else {
		++constBufferData.counter;
	}

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
	debugPiplineID = (DEGU_PIPLINE_ID)debugPiplineBuff;

	// AOのパイプラインを選択されていたときのみ、ノイズを出すかのフラグを表示する。
	if (debugPiplineID == AO_PIPLINE) {

		// デバッグ用でノイズ画面を出すためのフラグをセット。
		bool isNoise = constBufferData.isNoiseScene;
		ImGui::Checkbox("Noise Scene", &isNoise);
		constBufferData.isNoiseScene = isNoise;

		// アンビエントオクリュージョンを行うかのフラグをセット。
		bool isNoAO = constBufferData.isNoAO;
		ImGui::Checkbox("NoAO Scene", &isNoAO);
		// フラグが書き換わっていたらデノイズカウンターを初期化する。
		if (isNoAO != constBufferData.isNoAO) {
			constBufferData.counter = 0;
		}
		constBufferData.isNoAO = isNoAO;

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
・軽量化の処理を入れる。
・リアルタイムデノイズを実装する。
　→以前見た輝度に重みをおいたブラーでは実装できないかも。
　→ノイズが01なので、逆に輝度が大きすぎる…
　→逆に言えば、01の輝度の差があるところなんてノイズしかありえない？やってみよう！
・以上のことを今週までに実装する。

*/