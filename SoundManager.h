#pragma once
#include <fstream>
#include "DirectXBase.h"
#include "Singleton.h"
#include "Struct.h"

class SoundManager : public Singleton<SoundManager> {
private:
	friend Singleton<SoundManager>;
	SoundManager() {};

public:
	static ComPtr<IXAudio2> xAudio2;
	static IXAudio2MasteringVoice* masterVoice;

	void SettingSoundManager();
	//サウンド再生
	SoundData SoundLoadWave(char* fileName);
	//音声データの再生
	void SoundUnload(SoundData* soundData);
	//音声再生
	void SoundPlayerWave(SoundData& soundData, int loopCount);
};

//コールバック用
class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
public:
	//ボイス処理パスの開始時
	STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
	//ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
	//バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd)(THIS) {};
	//バッファの使用開始時
	STDMETHOD_(void, OnBufferStart)(THIS_ void* pBufferContext) {};
	//バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBUfferContext) {
		//バッファを開放する
		delete[] pBUfferContext;
	};
	//再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd)(THIS_ void* pBUfferContext) {};
	//ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError)(THIS_ void* pBufferContext, HRESULT Error) {};
};