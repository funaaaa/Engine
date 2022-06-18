#pragma once

#include "Singleton.h"

#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

static const int ALL_KEYS = 256;

static const int XINPUT_THUMB_LEFTVERT = 0;
static const int XINPUT_THUMB_LEFTSIDE = 1;
static const int XINPUT_THUMB_RIGHTVERT = 2;
static const int XINPUT_THUMB_RIGHTSIDE  = 3;

static const int XINPUT_TRIGGER_LEFT = 0;
static const int XINPUT_TRIGGER_RIGHT = 1;

class Input : public Singleton<Input>
{
public:
	BYTE keys[ALL_KEYS];					//現在のフレームのキー情報
	BYTE oldkeys[ALL_KEYS];				//前のフレームのキー情報
	DIMOUSESTATE mouse;					//現在のフレームのマウス情報
	DIMOUSESTATE oldmouse;				//前のフレームのマウス情報
	XINPUT_STATE state;					//現在のフレームのゲームパッド情報
	XINPUT_STATE oldstate;				//前のフレームのゲームパッド情報
	XINPUT_VIBRATION vibration;			//振動用

	//更新処理
	void Update(IDirectInputDevice8* devKeyboard, IDirectInputDevice8* devmouse) {
		for (int i = 0; i < ALL_KEYS; ++i) {
			//前のフレームのキー情報を保存
			oldkeys[i] = keys[i];
		}
		//現在のフレームのキー情報を取得
		devKeyboard->Acquire();
		devKeyboard->GetDeviceState(sizeof(keys), keys);
		//前のフレームのマウス情報を保存
		oldmouse = mouse;
		//現在のフレームのマウス情報を取得
		devmouse->Acquire();
		devmouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse);
		//前のフレームのゲームパッド情報を保存
		oldstate = state;
		//現在のフレームのマウス情報を取得
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD dwResult = XInputGetState(0, &state);

		//振動情報の取得
		XInputSetState(0, &vibration);
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

		//関数が呼ばれてないときは振動が止まるようにする
		vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = 0;

	}

	bool isKey(int keyIndex) {
		if (keyIndex < 0 || keyIndex > 255)return false;
		return keys[keyIndex];
	}

	bool isKeyTrigger(int keyIndex) {
		if (keyIndex < 0 || keyIndex > 255)return false;
		return keys[keyIndex] && !oldkeys[keyIndex];
	}

	bool isKeyRelease(int keyIndex) {
		if (keyIndex < 0 || keyIndex > 255)return false;
		return !keys[keyIndex] && oldkeys[keyIndex];
	}

	bool isMouse(int mouseIndex) {
		if (mouseIndex < 0 || mouseIndex > 255)return false;
		return mouse.rgbButtons[mouseIndex];
	}

	bool isMouseTrigger(int mouseIndex) {
		if (mouseIndex < 0 || mouseIndex > 255)return false;
		return mouse.rgbButtons[mouseIndex] && !oldmouse.rgbButtons[mouseIndex];
	}

	bool isMouseRelease(int mouseIndex) {
		if (mouseIndex < 0 || mouseIndex > 255)return false;
		return !mouse.rgbButtons[mouseIndex] && oldmouse.rgbButtons[mouseIndex];
	}

	bool isPad(int pad)//引数　押すボタンの種類
	{

		return state.Gamepad.wButtons & pad;
	}

	//XINPUT_GAMEPAD_DPAD_UP          0x0001		デジタル方向ボタン上
	//XINPUT_GAMEPAD_DPAD_DOWN        0x0002		デジタル方向ボタン下
	//XINPUT_GAMEPAD_DPAD_LEFT        0x0004		デジタル方向ボタン左
	//XINPUT_GAMEPAD_DPAD_RIGHT       0x0008		デジタル方向ボタン右
	//XINPUT_GAMEPAD_START            0x0010		STARTボタン
	//XINPUT_GAMEPAD_BACK             0x0020		BACKボタン
	//XINPUT_GAMEPAD_LEFT_THUMB       0x0040		LTボタン(判定のみ)
	//XINPUT_GAMEPAD_RIGHT_THUMB      0x0080		RTボタン(判定のみ)
	//XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100		LBボタン
	//XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200		RBボタン
	//XINPUT_GAMEPAD_A                0x1000		Aボタン
	//XINPUT_GAMEPAD_B                0x2000		Bボタン
	//XINPUT_GAMEPAD_X                0x4000		Xボタン
	//XINPUT_GAMEPAD_Y                0x8000		Yボタン



	bool isPadTrigger(int pad)
	{
		return (state.Gamepad.wButtons & pad) && !(oldstate.Gamepad.wButtons & pad);
	}

	bool isPadEnd(int pad)
	{
		return !(state.Gamepad.wButtons & pad) && (oldstate.Gamepad.wButtons & pad);
	}

	//引数　傾けるスティックの左右と向き
	float isPadThumb(int pad)
	{
		switch (pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return (float)state.Gamepad.sThumbLY / (32767.0f);
		case XINPUT_THUMB_LEFTSIDE:
			return (float)state.Gamepad.sThumbLX / 32767.0f;
		case XINPUT_THUMB_RIGHTVERT:
			return (float)state.Gamepad.sThumbRY / 32767.0f;
		case XINPUT_THUMB_RIGHTSIDE:
			return (float)state.Gamepad.sThumbRX / 32767.0f;
		default: return 0;
		}
	}

	float isOldPadThumb(int pad)
	{
		switch (pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return (float)oldstate.Gamepad.sThumbLY / (32767.0f);
		case XINPUT_THUMB_LEFTSIDE:
			return (float)oldstate.Gamepad.sThumbLX / 32767.0f;
		case XINPUT_THUMB_RIGHTVERT:
			return (float)oldstate.Gamepad.sThumbRY / 32767.0f;
		case XINPUT_THUMB_RIGHTSIDE:
			return (float)oldstate.Gamepad.sThumbRX / 32767.0f;
		default: return 0;
		}
	}

	//XINPUT_THUMB_LEFTVERT		左スティックの縦方向
	//XINPUT_THUMB_LEFTSIDE		左スティックの横方向
	//XINPUT_THUMB_RIGHTVERT	右スティックの縦方向
	//XINPUT_THUMB_RIGHTSIDE	右スティックの横方向

	//＜使い方＞
	//ただ入力してるとき移動させたい場合
	//右に倒してるとき正、左に倒してるとき負の値が返るので戻り値の正負でif文を書けばよい

	//入力の幅によって移動量を変えたい場合
	//関数の戻り値を移動量にかけ、プレイヤーの座標に足せばいい
	//プレイヤーの座標 += 関数の戻り値 * 移動量;

	float isPadThumbTrigger(int pad)
	{
		switch (pad)
		{
		case XINPUT_THUMB_LEFTVERT:
			return ((float)state.Gamepad.sThumbLY / 32767) && !((float)oldstate.Gamepad.sThumbLY / 32767);
		case XINPUT_THUMB_LEFTSIDE:
			return ((float)state.Gamepad.sThumbLX / 32767) && !((float)oldstate.Gamepad.sThumbLX / 32767);
		case XINPUT_THUMB_RIGHTVERT:
			return ((float)state.Gamepad.sThumbRY / 32767) && !((float)oldstate.Gamepad.sThumbRY / 32767);
		case XINPUT_THUMB_RIGHTSIDE:
			return ((float)state.Gamepad.sThumbRX / 32767) && !((float)oldstate.Gamepad.sThumbRX / 32767);
		default: return 0;
		}
	}

	float isPadTri(int pad)
	{
		switch (pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return (float)state.Gamepad.bLeftTrigger / 255;
		case XINPUT_TRIGGER_RIGHT:
			return (float)state.Gamepad.bRightTrigger / 255;
		default: return 0;
		}
	}

	//XINPUT_TRIGGER_LEFT		左のトリガー
	//XINPUT_TRIGGER_RIGHT		右のトリガー

	//＜使い方＞
	//基本的にisPadThumbと変わらない
	//押してるとき正の値が返る


	float isPadTriTrigger(int pad)
	{
		switch (pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return ((float)state.Gamepad.bLeftTrigger / 255) && !((float)oldstate.Gamepad.bLeftTrigger / 255);
		case XINPUT_TRIGGER_RIGHT:
			return ((float)state.Gamepad.bRightTrigger / 255) && !((float)oldstate.Gamepad.bRightTrigger / 255);
		default: return 0;
		}
	}

	float isPadTriEnd(int pad)
	{
		switch (pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return !((float)state.Gamepad.bLeftTrigger / 255) && ((float)oldstate.Gamepad.bLeftTrigger / 255);
		case XINPUT_TRIGGER_RIGHT:
			return !((float)state.Gamepad.bRightTrigger / 255) && ((float)oldstate.Gamepad.bRightTrigger / 255);
		default: return 0;
		}
	}

	//値指定版
	float isPadTriEndDefValue(int pad, float value)
	{
		switch (pad)
		{
		case XINPUT_TRIGGER_LEFT:
			return !((float)state.Gamepad.bLeftTrigger / 255 > value) && ((float)oldstate.Gamepad.bLeftTrigger / 255 > value);
		case XINPUT_TRIGGER_RIGHT:
			return !((float)state.Gamepad.bRightTrigger / 255 > value) && ((float)oldstate.Gamepad.bRightTrigger / 255 > value);
		default: return 0;
		}
	}
	
	void StartVibration(int LeftMotorRate,int RightMotorRate)
	{
		if (LeftMotorRate < 0 || LeftMotorRate > 100 || RightMotorRate < 0 || RightMotorRate > 100){
			return;
		}

		vibration.wLeftMotorSpeed = (int)(655.35 * LeftMotorRate);
		vibration.wRightMotorSpeed = (int)(655.35 * RightMotorRate);
		//0～100の範囲で指定
	}
};

enum InputMouseButton {
	DIM_LEFT,
	DIM_RIGHT,
	DIM_CENTER
};