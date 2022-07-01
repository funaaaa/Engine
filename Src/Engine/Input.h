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
	std::array<BYTE, ALL_KEYS> keys;	// ���݂̃t���[���̃L�[���
	std::array<BYTE, ALL_KEYS> oldkeys;	// �O�̃t���[���̃L�[���
	DIMOUSESTATE mouse;					// ���݂̃t���[���̃}�E�X���
	DIMOUSESTATE oldmouse;				// �O�̃t���[���̃}�E�X���
	XINPUT_STATE state;					// ���݂̃t���[���̃Q�[���p�b�h���
	XINPUT_STATE oldstate;				// �O�̃t���[���̃Q�[���p�b�h���
	XINPUT_VIBRATION vibration;			// �U���p

	// �X�V����
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

	// XINPUT_GAMEPAD_DPAD_UP          0x0001		�f�W�^�������{�^����
	// XINPUT_GAMEPAD_DPAD_DOWN        0x0002		�f�W�^�������{�^����
	// XINPUT_GAMEPAD_DPAD_LEFT        0x0004		�f�W�^�������{�^����
	// XINPUT_GAMEPAD_DPAD_RIGHT       0x0008		�f�W�^�������{�^���E
	// XINPUT_GAMEPAD_START            0x0010		START�{�^��
	// XINPUT_GAMEPAD_BACK             0x0020		BACK�{�^��
	// XINPUT_GAMEPAD_LEFT_THUMB       0x0040		LT�{�^��(����̂�)
	// XINPUT_GAMEPAD_RIGHT_THUMB      0x0080		RT�{�^��(����̂�)
	// XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100		LB�{�^��
	// XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200		RB�{�^��
	// XINPUT_GAMEPAD_A                0x1000		A�{�^��
	// XINPUT_GAMEPAD_B                0x2000		B�{�^��
	// XINPUT_GAMEPAD_X                0x4000		X�{�^��
	// XINPUT_GAMEPAD_Y                0x8000		Y�{�^��



	bool IsPadBottomTrigger(int Pad)
	{
		return (state.Gamepad.wButtons & Pad) && !(oldstate.Gamepad.wButtons & Pad);
	}

	bool IsPadBottomRelease(int Pad)
	{
		return !(state.Gamepad.wButtons & Pad) && (oldstate.Gamepad.wButtons & Pad);
	}

	// �����@�X����X�e�B�b�N�̍��E�ƌ���
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

	//XINPUT_THUMB_LEFTVERT		���X�e�B�b�N�̏c����
	//XINPUT_THUMB_LEFTSIDE		���X�e�B�b�N�̉�����
	//XINPUT_THUMB_RIGHTVERT	�E�X�e�B�b�N�̏c����
	//XINPUT_THUMB_RIGHTSIDE	�E�X�e�B�b�N�̉�����


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

	//XINPUT_TRIGGER_LEFT		���̃g���K�[
	//XINPUT_TRIGGER_RIGHT		�E�̃g���K�[


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
		// 0�`100�͈̔͂Ŏw��
	}
};

enum InputMouseButton {
	DIM_LEFT,
	DIM_RIGHT,
	DIM_CENTER
};