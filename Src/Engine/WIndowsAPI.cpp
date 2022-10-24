#include "WindowsAPI.h"

WindowsAPI::WindowsAPI()
{
	windowClass_ = {};
	wrc_ = {};
	hwnd_ = {};
	msg_ = {};
}

// ウィンドウプロシージャ関数
LRESULT WindowProc(HWND hwnd_, UINT msg_, WPARAM wparam, LPARAM lparam) {
	// メッセージで分岐
	switch (msg_) {
	case WM_DESTROY: // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}

	ImGui_ImplWin32_WndProcHandler(hwnd_,msg_,wparam,lparam);

	return DefWindowProc(hwnd_, msg_, wparam, lparam);	// 標準の処理を行う
}

void WindowsAPI::Init()
{
	// ウィンドウクラスを設定
	windowClass_.cbSize = sizeof(WNDCLASSEX);
	windowClass_.lpfnWndProc = (WNDPROC)WindowProc;			// ウィンドウプロシージャを設定
	windowClass_.lpszClassName = L"LE3A_20_フナクラベ_タクミ";	// ウィンドウクラス名
	windowClass_.hInstance = GetModuleHandle(nullptr);		// ウィンドウハンドル
	windowClass_.hIcon = LoadIcon(windowClass_.hInstance, TEXT("icon"));

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&windowClass_);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	wrc_ = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);	// 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	hwnd_ = CreateWindow(windowClass_.lpszClassName,		// クラス名
		L"LE3A_20_フナクラベ_タクミ",						// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,							// 標準的なウィンドウスタイル
		CW_USEDEFAULT,									// 表示X座標（OSに任せる）
		CW_USEDEFAULT,									// 表示Y座標（OSに任せる）
		wrc_.right - wrc_.left,							// ウィンドウ横幅
		wrc_.bottom - wrc_.top,							// ウィンドウ縦幅
		nullptr,										// 親ウィンドウハンドル
		nullptr,										// メニューハンドル
		windowClass_.hInstance,							// 呼び出しアプリケーションハンドル
		nullptr);										// オプション

	// ウィンドウ表示
	ShowWindow(hwnd_, SW_SHOW);

}