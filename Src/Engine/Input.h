#pragma once

#include "Singleton.h"

#define DIRECTINPUT_VERSION 0x0800
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
	std::array<BYTE, ALL_KEYS> keys_;	// 現在のフレームのキー情報
	std::array<BYTE, ALL_KEYS> oldkeys_;	// 前のフレームのキー情報
	DIMOUSESTATE mouse_;					// 現在のフレームのマウス情報
	DIMOUSESTATE oldmouse_;				// 前のフレームのマウス情報
	XINPUT_STATE state_;					// 現在のフレームのゲームパッド情報
	XINPUT_STATE oldstate_;				// 前のフレームのゲームパッド情報
	XINPUT_VIBRATION vibration_;			// 振動用

	// 更新処理
	void Update(IDirectInputDevice8* DevKeyboard, IDirectInputDevice8* Devmouse);

	bool IsKey(int KeyIndex) {
		if (KeyIndex < 0 || 255 < KeyIndex)return false;
		return keys_[KeyIndex];
	}

	bool IsKeyTrigger(int KeyIndex) {
		if (KeyIndex < 0 || 255 < KeyIndex)return false;
		return keys_[KeyIndex] && !oldkeys_[KeyIndex];
	}

	bool IsKeyRelease(int KeyIndex) {
		if (KeyIndex < 0 || 255 < KeyIndex)return false;
		return !keys_[KeyIndex] && oldkeys_[KeyIndex];
	}

	bool IsMouse(int MouseIndex) {
		if (MouseIndex < 0 || 255 < MouseIndex)return false;
		return mouse_.rgbButtons[MouseIndex];
	}

	bool IsMouseTrigger(int MouseIndex) {
		if (MouseIndex < 0 || 255 < MouseIndex)return false;
		return mouse_.rgbButtons[MouseIndex] && !oldmouse_.rgbButtons[MouseIndex];
	}

	bool IsMouseRelease(int MouseIndex) {
		if (MouseIndex < 0 || 255 < MouseIndex)return false;
		return !mouse_.rgbButtons[MouseIndex] && oldmouse_.rgbButtons[MouseIndex];
	}

	bool IsPadBottom(int Pad)
	{
		return state_.Gamepad.wButtons & Pad;
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
		return (state_.Gamepad.wButtons & Pad) && !(oldstate_.Gamepad.wButtons & Pad);
	}

	bool IsPadBottomRelease(int Pad)
	{
		return !(state_.Gamepad.wButtons & Pad) && (oldstate_.Gamepad.wButtons & Pad);
	}

	// 引数　傾けるスティックの左右と向き
	float PadStick(int Pad)
	{
		const float INPUT_MAX = 32767.0f;
		switch (Pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return static_cast<float>(state_.Gamepad.sThumbLY) / INPUT_MAX;
		case XINPUT_THUMB_LEFTSIDE:
			return static_cast<float>(state_.Gamepad.sThumbLX) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTVERT:
			return static_cast<float>(state_.Gamepad.sThumbRY) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTSIDE:
			return static_cast<float>(state_.Gamepad.sThumbRX) / INPUT_MAX;
		default: return 0;
		}
	}

	float OldPadStick(int Pad)
	{
		const float INPUT_MAX = 32767.0f;
		switch (Pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return static_cast<float>(oldstate_.Gamepad.sThumbLY) / INPUT_MAX;
		case XINPUT_THUMB_LEFTSIDE:
			return static_cast<float>(oldstate_.Gamepad.sThumbLX) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTVERT:
			return static_cast<float>(oldstate_.Gamepad.sThumbRY) / INPUT_MAX;
		case XINPUT_THUMB_RIGHTSIDE:
			return static_cast<float>(oldstate_.Gamepad.sThumbRX) / INPUT_MAX;
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
			return (static_cast<float>(state_.Gamepad.sThumbLY) / INPUT_MAX) && !(static_cast<float>(oldstate_.Gamepad.sThumbLY) / INPUT_MAX);
		case XINPUT_THUMB_LEFTSIDE:
			return (static_cast<float>(state_.Gamepad.sThumbLX) / INPUT_MAX) && !(static_cast<float>(oldstate_.Gamepad.sThumbLX) / INPUT_MAX);
		case XINPUT_THUMB_RIGHTVERT:
			return (static_cast<float>(state_.Gamepad.sThumbRY) / INPUT_MAX) && !(static_cast<float>(oldstate_.Gamepad.sThumbRY) / INPUT_MAX);
		case XINPUT_THUMB_RIGHTSIDE:
			return (static_cast<float>(state_.Gamepad.sThumbRX) / INPUT_MAX) && !(static_cast<float>(oldstate_.Gamepad.sThumbRX) / INPUT_MAX);
		default: return 0;
		}
	}

	float PadTrigger(int Pad)
	{
		const float INPUT_MAX = 255.0f;
		switch (Pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return static_cast<float>(state_.Gamepad.bLeftTrigger) / INPUT_MAX;
		case XINPUT_TRIGGER_RIGHT:
			return static_cast<float>(state_.Gamepad.bRightTrigger) / INPUT_MAX;
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
			return (static_cast<float>(state_.Gamepad.bLeftTrigger) / INPUT_MAX) && !(static_cast<float>(oldstate_.Gamepad.bLeftTrigger) / INPUT_MAX);
		case XINPUT_TRIGGER_RIGHT:
			return (static_cast<float>(state_.Gamepad.bRightTrigger) / INPUT_MAX) && !(static_cast<float>(oldstate_.Gamepad.bRightTrigger) / INPUT_MAX);
		default: return 0;
		}
	}

	bool IsPadTriggerRelease(int Pad)
	{
		const float INPUT_MAX = 255.0f;
		switch (Pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return !(static_cast<float>(state_.Gamepad.bLeftTrigger) / INPUT_MAX) && (static_cast<float>(oldstate_.Gamepad.bLeftTrigger) / INPUT_MAX);
		case XINPUT_TRIGGER_RIGHT:
			return !(static_cast<float>(state_.Gamepad.bRightTrigger) / INPUT_MAX) && (static_cast<float>(oldstate_.Gamepad.bRightTrigger) / INPUT_MAX);
		default: return 0;
		}
	}

	void StartVibration(int LeftMotorRate, int RightMotorRate)
	{
		if (LeftMotorRate < 0 || 100 < LeftMotorRate || RightMotorRate < 0 || 100 < RightMotorRate) {
			return;
		}

		vibration_.wLeftMotorSpeed = static_cast<WORD>(655.35f * LeftMotorRate);
		vibration_.wRightMotorSpeed = static_cast<WORD>(655.35f * RightMotorRate);
		// 0～100の範囲で指定
	}
};

enum InputMouseButton {
	DIM_LEFT,
	DIM_RIGHT,
	DIM_CENTER
};