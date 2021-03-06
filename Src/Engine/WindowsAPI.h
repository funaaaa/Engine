#pragma once
/*
・ウィンドウ生成
・ウィンドウプロシージャ
・Windowsからのメッセージ処理
*/
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable:4820)
#pragma warning(disable:5219)
#pragma warning(disable:5039)

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);

#pragma warning(pop)

// ウィンドウサイズ
const int WINDOW_WIDTH = 1280;  // 横幅
const int WINDOW_HEIGHT = 720;  // 縦幅

class WindowsAPI {

public:

	WNDCLASSEX windowClass_;	// ウィンドウクラス
	RECT wrc_;					// ウィンドウサイズ
	HWND hwnd_;					// ウィンドウオブジェクト
	MSG msg_;					// メッセージ

	// コンストラクタ
	WindowsAPI();
	// 初期化処理
	void Init();
};