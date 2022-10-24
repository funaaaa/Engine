#include "WindowsAPI.h"

WindowsAPI::WindowsAPI()
{
	windowClass_ = {};
	wrc_ = {};
	hwnd_ = {};
	msg_ = {};
}

// �E�B���h�E�v���V�[�W���֐�
LRESULT WindowProc(HWND hwnd_, UINT msg_, WPARAM wparam, LPARAM lparam) {
	// ���b�Z�[�W�ŕ���
	switch (msg_) {
	case WM_DESTROY: // �E�B���h�E���j�����ꂽ
		PostQuitMessage(0); // OS�ɑ΂��āA�A�v���̏I����`����
		return 0;
	}

	ImGui_ImplWin32_WndProcHandler(hwnd_,msg_,wparam,lparam);

	return DefWindowProc(hwnd_, msg_, wparam, lparam);	// �W���̏������s��
}

void WindowsAPI::Init()
{
	// �E�B���h�E�N���X��ݒ�
	windowClass_.cbSize = sizeof(WNDCLASSEX);
	windowClass_.lpfnWndProc = (WNDPROC)WindowProc;			// �E�B���h�E�v���V�[�W����ݒ�
	windowClass_.lpszClassName = L"LE3A_20_�t�i�N���x_�^�N�~";	// �E�B���h�E�N���X��
	windowClass_.hInstance = GetModuleHandle(nullptr);		// �E�B���h�E�n���h��
	windowClass_.hIcon = LoadIcon(windowClass_.hInstance, TEXT("icon"));

	// �E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&windowClass_);
	// �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
	wrc_ = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);	// �����ŃT�C�Y�␳

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd_ = CreateWindow(windowClass_.lpszClassName,		// �N���X��
		L"LE3A_20_�t�i�N���x_�^�N�~",						// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,							// �W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,									// �\��X���W�iOS�ɔC����j
		CW_USEDEFAULT,									// �\��Y���W�iOS�ɔC����j
		wrc_.right - wrc_.left,							// �E�B���h�E����
		wrc_.bottom - wrc_.top,							// �E�B���h�E�c��
		nullptr,										// �e�E�B���h�E�n���h��
		nullptr,										// ���j���[�n���h��
		windowClass_.hInstance,							// �Ăяo���A�v���P�[�V�����n���h��
		nullptr);										// �I�v�V����

	// �E�B���h�E�\��
	ShowWindow(hwnd_, SW_SHOW);

}