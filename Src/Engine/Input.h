#pragma once

#include "Singleton.h"

#include <dinput.h>
#include <Xinput.h>
#include <array>

static const int ALL_KEYS = 256;

static const int XINPUT_THUMB_LEFTVERT = 0;
static const int XINPUT_THUMB_LEFTSIDE = 1;
static const int XINPUT_THUMB_RIGHTVERT = 2;
static const int XINPUT_THUMB_RIGHTSIDE = 3;

static const int XINPUT_TRIGGER_LEFT = 0;
static const int XINPUT_TRIGGER_RIGHT = 1;

class Input : public Singleton<Input>
{
public:
	std::array<BYTE, ALL_KEYS> keys;	// 現在のフレームのキー情報
	std::array<BYTE, ALL_KEYS> oldkeys;	// 前のフレームのキー情報
	DIMOUSESTATE mouse;					// 現在のフレームのマウス情報
	DIMOUSESTATE oldmouse;				// 前のフレームのマウス情報
	XINPUT_STATE state;					// 現在のフレームのゲームパッド情報
	XINPUT_STATE oldstate;				// 前のフレームのゲームパッド情報
	XINPUT_VIBRATION vibration;			// 振動用

	// 更新処理
	void Update(IDirectInputDevice8* DevKeyboard, IDirectInputDevice8* Devmouse);

	bool IsKey(int KeyIndex) {
		if (KeyIndex < 0 || 255 < KeyIndex)return false;
		return keys[KeyIndex];
	}

	bool IsKeyTrigger(int KeyIndex) {
		if (KeyIndex < 0 || 255 < KeyIndex)return false;
		return keys[KeyIndex] && !oldkeys[KeyIndex];
	}

	bool IsKeyRelease(int KeyIndex) {
		if (KeyIndex < 0 || 255 < KeyIndex)return false;
		return !keys[KeyIndex] && oldkeys[KeyIndex];
	}

	bool IsMouse(int MouseIndex) {
		if (MouseIndex < 0 || 255 < MouseIndex)return false;
		return mouse.rgbButtons[MouseIndex];
	}

	bool IsMouseTrigger(int MouseIndex) {
		if (MouseIndex < 0 || 255 < MouseIndex)return false;
		return mouse.rgbButtons[MouseIndex] && !oldmouse.rgbButtons[MouseIndex];
	}

	bool IsMouseRelease(int MouseIndex) {
		if (MouseIndex < 0 || 255 < MouseIndex)return false;
		return !mouse.rgbButtons[MouseIndex] && oldmouse.rgbButtons[MouseIndex];
	}

	bool IsPadBottom(int Pad)
	{
		return state.Gamepad.wButtons & Pad;
	}

	// XINPUT_GAMEPAD_DPAD_UP          0x0001		デジタル方向ボタン上
	// XINPUT_GAMEPAD_DPAD_DOWN        0x0002		デジタル方向ボタン下
	// XINPUT_GAMEPAD_DPAD_LEFT        0x0004		デジタル方向ボタン左
	// XINPUT_GAMEPAD_DPAD_RIGHT       0x0008		デジタル方向ボタン右
	// XINPUT_GAMEPAD_START            0x0010		STARTボタン
	// XINPUT_GAMEPAD_BACK             0x0020		BACKボタン
	// XINPUT_GAMEPAD_LEFT_THUMB       0x0040		LTボタン(判定のみ)
	// XINPUT_GAMEPAD_RIGHT_THUMB      0x0080		RTボタン(判定のみ)
	// XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100		LBボタン
	// XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200		RBボタン
	// XINPUT_GAMEPAD_A                0x1000		Aボタン
	// XINPUT_GAMEPAD_B                0x2000		Bボタン
	// XINPUT_GAMEPAD_X                0x4000		Xボタン
	// XINPUT_GAMEPAD_Y                0x8000		Yボタン



	bool IsPadBottomTrigger(int Pad)
	{
		return (state.Gamepad.wButtons & Pad) && !(oldstate.Gamepad.wButtons & Pad);
	}

	bool IsPadBottomRelease(int Pad)
	{
		return !(state.Gamepad.wButtons & Pad) && (oldstate.Gamepad.wButtons & Pad);
	}

	// 引数　傾けるスティックの左右と向き
	float PadStick(int Pad)
	{
		const float INPUT_MAX = 32767.0f;
		switch (Pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return static_cast<float>(state.Gamepad.sThumbLY) / INPUT_MAX;
		case XINPUT_THUMB_LEFTSIDE:
			return static_cast<float>(state.Gamepad.sThumbLX) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTVERT:
			return static_cast<float>(state.Gamepad.sThumbRY) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTSIDE:
			return static_cast<float>(state.Gamepad.sThumbRX) / INPUT_MAX;
		default: return 0;
		}
	}

	float OldPadStick(int Pad)
	{
		const float INPUT_MAX = 32767.0f;
		switch (Pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return static_cast<float>(oldstate.Gamepad.sThumbLY) / INPUT_MAX;
		case XINPUT_THUMB_LEFTSIDE:
			return static_cast<float>(oldstate.Gamepad.sThumbLX) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTVERT:
			return static_cast<float>(oldstate.Gamepad.sThumbRY) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTSIDE:
			return static_cast<float>(oldstate.Gamepad.sThumbRX) / INPUT_MAX;
		default: return 0;
		}
	}

	//XINPUT_THUMB_LEFTVERT		左スティックの縦方向
	//XINPUT_THUMB_LEFTSIDE		左スティックの横方向
	//XINPUT_THUMB_RIGHTVERT	右スティックの縦方向
	//XINPUT_THUMB_RIGHTSIDE	右スティックの横方向


	bool IsPadStickTrigger(int Pad)
	{
		const float INPUT_MAX = 32767.0f;
		switch (Pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return (static_cast<float>(state.Gamepad.sThumbLY) / INPUT_MAX) && !(static_cast<float>(oldstate.Gamepad.sThumbLY) / INPUT_MAX);
		case XINPUT_THUMB_LEFTSIDE:
			return (static_cast<float>(state.Gamepad.sThumbLX) / INPUT_MAX) && !(static_cast<float>(oldstate.Gamepad.sThumbLX) / INPUT_MAX);
		case XINPUT_THUMB_RIGHTVERT:
			return (static_cast<float>(state.Gamepad.sThumbRY) / INPUT_MAX) && !(static_cast<float>(oldstate.Gamepad.sThumbRY) / INPUT_MAX);
		case XINPUT_THUMB_RIGHTSIDE:
			return (static_cast<float>(state.Gamepad.sThumbRX) / INPUT_MAX) && !(static_cast<float>(oldstate.Gamepad.sThumbRX) / INPUT_MAX);
		default: return 0;
		}
	}

	float PadTrigger(int Pad)
	{
		const float INPUT_MAX = 255.0f;
		switch (Pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return static_cast<float>(state.Gamepad.bLeftTrigger) / INPUT_MAX;
		case XINPUT_TRIGGER_RIGHT:
			return static_cast<float>(state.Gamepad.bRightTrigger) / INPUT_MAX;
		default: return 0;
		}
	}

	//XINPUT_TRIGGER_LEFT		左のトリガー
	//XINPUT_TRIGGER_RIGHT		右のトリガー


	bool IsPadTriggerTrigger(int Pad)
	{
		const float INPUT_MAX = 255.0f;
		switch (Pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return (static_cast<float>(state.Gamepad.bLeftTrigger) / INPUT_MAX) && !(static_cast<float>(oldstate.Gamepad.bLeftTrigger) / INPUT_MAX);
		case XINPUT_TRIGGER_RIGHT:
			return (static_cast<float>(state.Gamepad.bRightTrigger) / INPUT_MAX) && !(static_cast<float>(oldstate.Gamepad.bRightTrigger) / INPUT_MAX);
		default: return 0;
		}
	}

	bool IsPadTriggerRelease(int Pad)
	{
		const float INPUT_MAX = 255.0f;
		switch (Pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return !(static_cast<float>(state.Gamepad.bLeftTrigger) / INPUT_MAX) && (static_cast<float>(oldstate.Gamepad.bLeftTrigger) / INPUT_MAX);
		case XINPUT_TRIGGER_RIGHT:
			return !(static_cast<float>(state.Gamepad.bRightTrigger) / INPUT_MAX) && (static_cast<float>(oldstate.Gamepad.bRightTrigger) / INPUT_MAX);
		default: return 0;
		}
	}

	void StartVibration(int LeftMotorRate, int RightMotorRate)
	{
		if (LeftMotorRate < 0 || 100 < LeftMotorRate || RightMotorRate < 0 || 100 < RightMotorRate) {
			return;
		}

		vibration.wLeftMotorSpeed = static_cast<WORD>(655.35f * LeftMotorRate);
		vibration.wRightMotorSpeed = static_cast<WORD>(655.35f * RightMotorRate);
		// 0～100の範囲で指定
	}
};

enum InputMouseButton {
	DIM_LEFT,
	DIM_RIGHT,
	DIM_CENTER
};