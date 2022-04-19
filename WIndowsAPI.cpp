#include "WindowsAPI.h"

WindowsAPI::WindowsAPI()
{
	windowClass = {};
	wrc = {};
	hwnd = {};
	msg = {};
}

//ウィンドウプロシージャ関数
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// メッセージで分岐
	switch (msg) {
	case WM_DESTROY: // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}

	ImGui_ImplWin32_WndProcHandler(hwnd,msg,wparam,lparam);

	return DefWindowProc(hwnd, msg, wparam, lparam);	//標準の処理を行う
}

void WindowsAPI::Init()
{
	//ウィンドウクラスを設定
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProc;		//ウィンドウプロシージャを設定
	windowClass.lpszClassName = L"LE3A_21_フナクラベ_タクミ 運動の法則 前編 課題";			//ウィンドウクラス名
	windowClass.hInstance = GetModuleHandle(nullptr);	//ウィンドウハンドル
	windowClass.hIcon = LoadIcon(windowClass.hInstance, TEXT("icon"));

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&windowClass);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);	//自動でサイズ補正

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(windowClass.lpszClassName,		//クラス名
		L"LE3A_21_フナクラベ_タクミ 運動の法則 前編 課題",									//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,							//標準的なウィンドウスタイル
		CW_USEDEFAULT,									//表示X座標（OSに任せる）
		CW_USEDEFAULT,									//表示Y座標（OSに任せる）
		wrc.right - wrc.left,							//ウィンドウ横幅
		wrc.bottom - wrc.top,							//ウィンドウ縦幅
		nullptr,										//親ウィンドウハンドル
		nullptr,										//メニューハンドル
		windowClass.hInstance,							//呼び出しアプリケーションハンドル
		nullptr);										//オプション

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
}