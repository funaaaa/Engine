#include "DevDXR.h"

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

	// 開発用
	DevDXR dev;
	dev.Init();

	/*----------ゲームループ----------*/
	while (true) {

		
		dev.Update();
		dev.Draw();
		

	}

	return 0;
}