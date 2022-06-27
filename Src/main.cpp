#include "SceneMgr.h"
#include "SoundManager.h"
#include "FbxLoader.h"
#include "RayDenoiser.h"
#include "Camera.h"
#include "HitGroupMgr.h"
#include "DescriptorHeapMgr.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX初期化処理----------*/
	DirectXBase::Ins()->Init();									// DirectX基盤の初期化
	SoundManager::Ins()->SettingSoundManager();	// サウンドマネージャーをセットする

	/*----------変数宣言----------*/
	srand(static_cast<unsigned int>(time(NULL)));

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

	// シーンクラス。
	SceneMgr::Ins()->Init();

	/*----------ゲームループ----------*/
	while (true) {

		// 描画前処理
		DirectXBase::Ins()->processBeforeDrawing();


		SceneMgr::Ins()->Update();
		SceneMgr::Ins()->Draw();


		// 描画後処理
		DirectXBase::Ins()->processAfterDrawing();

	}

	return 0;
}