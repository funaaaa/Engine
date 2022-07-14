#include "Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")


void Input::Update(IDirectInputDevice8* DevKeyboard, IDirectInputDevice8* Devmouse) {

	for (int i = 0; i < ALL_KEYS; ++i) {
		// 前のフレームのキー情報を保存
		oldkeys_[i] = keys_[i];
	}
	// 現在のフレームのキー情報を取得
	DevKeyboard->Acquire();
	DevKeyboard->GetDeviceState(static_cast<DWORD>(keys_.size()), keys_.data());
	// 前のフレームのマウス情報を保存
	oldmouse_ = mouse_;
	// 現在のフレームのマウス情報を取得
	Devmouse->Acquire();
	Devmouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_);
	// 前のフレームのゲームパッド情報を保存
	oldstate_ = state_;
	// 現在のフレームのマウス情報を取得
	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	XInputGetState(0, &state_);

	// 振動情報の取得
	XInputSetState(0, &vibration_);
	ZeroMemory(&vibration_, sizeof(XINPUT_VIBRATION));

	// 関数が呼ばれてないときは振動が止まるようにする
	vibration_.wLeftMotorSpeed = 0;
	vibration_.wRightMotorSpeed = 0;

}