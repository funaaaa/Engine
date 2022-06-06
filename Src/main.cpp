#include "DevDXR.h"

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX初期化処理----------*/
	ImGuiWindow::Ins()->Init();
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