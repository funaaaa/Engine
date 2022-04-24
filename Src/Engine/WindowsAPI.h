#pragma once
/*
・ウィンドウ生成
・ウィンドウプロシージャ
・Windowsからのメッセージ処理
*/
#include <Windows.h>
#include "imgui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);

// ウィンドウサイズ
const int window_width = 1280;  //横幅
const int window_height = 720;  //縦幅

class WindowsAPI {
public:
	WNDCLASSEX windowClass;		//ウィンドウクラス
	RECT wrc;					//ウィンドウサイズ
	HWND hwnd;					//ウィンドウオブジェクト
	MSG msg;					//メッセージ

	//コンストラクタ
	WindowsAPI();
	//初期化処理
	void Init();
};