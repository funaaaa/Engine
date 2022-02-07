#include "SoundManager.h"
#include "LightManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Sprite.h"
#include "FHelper.h"
#include "Enum.h"
#include "Object3D.h"
#include "MultiPathFunction.h"
#include "Camera.h"
#include "LightCamera.h"
#include "Input.h"
#include "TextureManager.h"

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

// fps更新
void FPS();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX初期化処理----------*/
	DirectXBase directXBase;							// DirectX基盤部分
	directXBase.Init();									// DirectX基盤の初期化
	SoundManager::Instance()->SettingSoundManager();	// サウンドマネージャーをセットする

	/*----------パイプライン生成----------*/
	PiplineManager::Instance()->Init();

	/*----------変数宣言----------*/
	srand(time(NULL));

	// ライトを生成
	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
		//LightManager::Instance()->GeneratePointlight({}, { 1,1,1 }, { 0.5f,0.5f,0.5f });
		//LightManager::Instance()->pointlights[i].active = false;
	}
	LightManager::Instance()->GenerateDirlight({ 0,-1,0 }, { 1,1,1 });

	// スポットライトを生成。
	//LightManager::Instance()->GenerateSpotlight(XMFLOAT3(0, -1, 0), XMFLOAT3(0, 1000, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(0, 0, 00), XMFLOAT2(0, 0));

	RenderTarget shadowMapRenderTarget;
	float shadowMapClearColor[4] = { 1,1,1,1 };
	shadowMapRenderTarget.Create(window_width, window_height, 1, 1, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT, shadowMapClearColor);

	// 通常猿オブジェクトを生成。
	Object3D monkey;
	monkey.Generate(XMFLOAT3(0, 0, 0), PROJECTIONID_OBJECT, PIPLINE_OBJECT_LIGHT_ALPHA, "Resource/", "monkey.obj", L"Resource/plane.png", TRUE);
	monkey.ChangeScale(100, 100, 100);

	// 影用猿オブジェクトを生成。
	Object3D shadowMonkey;
	shadowMonkey.Generate(XMFLOAT3(0, 0, 0), PROJECTIONID_OBJECT, PIPLINE_OBJECT_GET_SHADOWMAP, "Resource/", "monkey.obj", L"Resource/plane.png", TRUE);
	shadowMonkey.ChangeScale(100, 100, 100);
	shadowMonkey.ClearTextureID();

	// 床オブジェクトを生成。
	Object3D ground;
	ground.Generate(XMFLOAT3(0, 0, 0), PROJECTIONID_OBJECT, PIPLINE_OBJECT_LIGHT_SHADOWMAP_ALPHA, "Resource/", "plane.obj", L"Resource/plane.png", FALSE);
	ground.ChangeScale(1000, 1000, 1000);
	ground.AddTextureID(shadowMapRenderTarget.textureID);
	ground.ChangePosition(monkey.GetPos().x, monkey.GetPos().y - 100.0f, monkey.GetPos().z);

	// ライトカメラを生成
	LightCamera lightCamera;
	XMFLOAT3 lihghtEye = monkey.GetPos();
	lihghtEye.y += 1000;
	XMVECTOR dir = { 0,-1,0 };
	lightCamera.Generate(&lihghtEye, &dir);
	//lightCamera.GenerateViewMat();

	// ライトカメラ用の定数バッファ
	LightCameraConstBufferData lightCameraConstData;
	lightCameraConstData.lightViewProj = lightCamera.GetViewMat() * lightCamera.GetPerspectiveMat();

	short groundConstDataHandle = ground.AddConstBuffer(sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());
	short shadowMonkeyConstDataHandle = shadowMonkey.AddConstBuffer(sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());

	/*----------ゲームループ----------*/
	while (true) {
		/*----------毎フレーム処理(描画前処理)----------*/
		directXBase.processBeforeDrawing();
		// 画面に表示されるレンダーターゲットに戻す
		//DirectXBase::Instance()->SetRenderTarget();


		/*----- 更新処理 -----*/

		// ビュー行列を生成。
		Camera::GenerateMatView();

		Camera::target = monkey.GetPos();

		monkey.ChangeRotation(0, 0.01f, 0);

		// 通常猿と他の猿の行列を一致させる。
		shadowMonkey.AssignmentWorldMatrix(monkey.GetPositionMat(), monkey.GetScaleMat(), monkey.GetRotationMat());

		ground.TransData(groundConstDataHandle, &lightCameraConstData, sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());
		shadowMonkey.TransData(shadowMonkeyConstDataHandle, &lightCameraConstData, sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());

		/*----- 描画処理 -----*/

		// 影用のレンダーターゲットに治す。
		MultiPathFunction::Instance()->SetRenderTargetAndClear(shadowMapRenderTarget);

		shadowMonkey.Draw();

		// リソースバリアをもとに戻す。
		MultiPathFunction::Instance()->ResourceBarrierAfter(shadowMapRenderTarget);

		// 画面に表示されるレンダーターゲットに戻す。
		DirectXBase::Instance()->SetRenderTarget();

		// どの猿を表示するかのフラグによって描画する猿を変える
		monkey.Draw();

		ground.Draw();

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
		SetWindowText(DirectXBase::Instance()->windowsAPI.hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}
}

/*

実装メモ
・まずはシャドウマップを実装する。
・その状態で通常描画に必要な定数バッファ周りを全てvoid*で管理する。
・その状態でmemcopyでmapして正しく動くかチェック。
・次に影データを追加して正しく動くかチェック。


・定数バッファを追加する処理を追加する。
・定数バッファを転送する処理を追加する。


マップ処理の以前に必要な処理
・D3D12Resourceを生成する。
・つまりD3D12Resourceのvector配列を用意する必要がある。
・生成関数では保存されているIndexを取得する。

マップ処理に必要な情報
・D3D12Resourceみたいな名前のやつのアドレス(Map関数を呼ぶために必要。)
・送りたい情報のアドレス。
・送りたい情報のサイズ。




何故か定数バッファが送られていない。なぜだ。

*/