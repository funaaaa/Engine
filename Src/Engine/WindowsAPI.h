#pragma once
/*
�E�E�B���h�E����
�E�E�B���h�E�v���V�[�W��
�EWindows����̃��b�Z�[�W����
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

// �E�B���h�E�T�C�Y
const int window_width = 1280;  //����
const int window_height = 720;  //�c��

class WindowsAPI {
public:
	WNDCLASSEX windowClass;		//�E�B���h�E�N���X
	RECT wrc;					//�E�B���h�E�T�C�Y
	HWND hwnd;					//�E�B���h�E�I�u�W�F�N�g
	MSG msg;					//���b�Z�[�W

	//�R���X�g���N�^
	WindowsAPI();
	//����������
	void Init();
};