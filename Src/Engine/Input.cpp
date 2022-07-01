#include "Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")


void Input::Update(IDirectInputDevice8* DevKeyboard, IDirectInputDevice8* Devmouse) {

	for (int i = 0; i < ALL_KEYS; ++i) {
		// �O�̃t���[���̃L�[����ۑ�
		oldkeys[i] = keys[i];
	}
	// ���݂̃t���[���̃L�[�����擾
	DevKeyboard->Acquire();
	DevKeyboard->GetDeviceState(static_cast<DWORD>(keys.size()), keys.data());
	// �O�̃t���[���̃}�E�X����ۑ�
	oldmouse = mouse;
	// ���݂̃t���[���̃}�E�X�����擾
	Devmouse->Acquire();
	Devmouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse);
	// �O�̃t���[���̃Q�[���p�b�h����ۑ�
	oldstate = state;
	// ���݂̃t���[���̃}�E�X�����擾
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	XInputGetState(0, &state);

	// �U�����̎擾
	XInputSetState(0, &vibration);
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	// �֐����Ă΂�ĂȂ��Ƃ��͐U�����~�܂�悤�ɂ���
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;

}