#include "Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")


void Input::Update(IDirectInputDevice8* DevKeyboard, IDirectInputDevice8* Devmouse) {

	for (int i = 0; i < ALL_KEYS; ++i) {
		// 前のフレームのキー情報を保存
		oldkeys[i] = keys[i];
	}
	// 現在のフレームのキー情報を取得
	DevKeyboard->Acquire();
	DevKeyboard->GetDeviceState(static_cast<DWORD>(keys.size()), keys.data());
	// 前のフレームのマウス情報を保存
	oldmouse = mouse;
	// 現在のフレームのマウス情報を取得
	Devmouse->Acquire();
	Devmouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse);
	// 前のフレームのゲームパッド情報を保存
	oldstate = state;
	// 現在のフレームのマウス情報を取得
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	XInputGetState(0, &state);

	// 振動情報の取得
	XInputSetState(0, &vibration);
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	// 関数が呼ばれてないときは振動が止まるようにする
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;

}