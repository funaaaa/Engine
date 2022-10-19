#include "SceneMgr.h"
#include "SoundManager.h"
#include "FbxLoader.h"
#include "RayDenoiser.h"
#include "Camera.h"
#include "HitGroupMgr.h"
#include "DescriptorHeapMgr.h"
#include "PipelineManager.h"
#include "PolygonInstanceRegister.h"
#include "Input.h"
#include "BLASRegister.h"
#include "RayEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX初期化処理----------*/

	Engine::Ins()->Init();									// DirectX基盤の初期化
	SoundManager::Ins()->SettingSoundManager();	// サウンドマネージャーをセットする

	/*----------パイプライン生成----------*/
	PipelineManager::Ins()->Init();

	/*----------変数宣言----------*/
	srand(static_cast<unsigned int>(time(NULL)));

	// ディスクリプタヒープを初期化。
	DescriptorHeapMgr::Ins()->GenerateDescriptorHeap();

	// デノイズ用のクラスを初期化。
	Denoiser::Ins()->Setting();

	// BLASを準備。
	BLASRegister::Ins()->Setting();

	// インスタンスをセット。
	PolygonInstanceRegister::Ins()->Setting();

	// FBXLoaderを初期化。
	FbxLoader::Ins()->Init();

	// ヒットグループを設定。
	HitGroupMgr::Ins()->Setting();

	// カメラを初期化。
	Camera::Ins()->Init();

	// シーンクラス。
	SceneMgr::Ins()->Init();

	// レイトレエンジンを設定。
	RayEngine::Ins()->Setting();


	/*----------ゲームループ----------*/
	while (true) {

		// 描画前処理
		Engine::Ins()->ProcessBeforeDrawing();


		SceneMgr::Ins()->Update();
		SceneMgr::Ins()->Draw();

		if (Input::Ins()->IsKeyTrigger(DIK_ESCAPE)) {

			exit(0);

		}


		// 描画後処理
		Engine::Ins()->ProcessAfterDrawing();

	}

	return 0;

}