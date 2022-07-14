#include "Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")


void Input::Update(IDirectInputDevice8* DevKeyboard, IDirectInputDevice8* Devmouse) {

	for (int i = 0; i < ALL_KEYS; ++i) {
		// �O�̃t���[���̃L�[����ۑ�
		oldkeys_[i] = keys_[i];
	}
	// ���݂̃t���[���̃L�[�����擾
	DevKeyboard->Acquire();
	DevKeyboard->GetDeviceState(static_cast<DWORD>(keys_.size()), keys_.data());
	// �O�̃t���[���̃}�E�X����ۑ�
	oldmouse_ = mouse_;
	// ���݂̃t���[���̃}�E�X�����擾
	Devmouse->Acquire();
	Devmouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_);
	// �O�̃t���[���̃Q�[���p�b�h����ۑ�
	oldstate_ = state_;
	// ���݂̃t���[���̃}�E�X�����擾
	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	XInputGetState(0, &state_);

	// �U�����̎擾
	XInputSetState(0, &vibration_);
	ZeroMemory(&vibration_, sizeof(XINPUT_VIBRATION));

	// �֐����Ă΂�ĂȂ��Ƃ��͐U�����~�܂�悤�ɂ���
	vibration_.wLeftMotorSpeed = 0;
	vibration_.wRightMotorSpeed = 0;

}