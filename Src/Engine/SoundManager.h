#pragma once
#include <fstream>
#include "Singleton.h"
#include <wrl/client.h>
#include <xaudio2.h>
#include <array>

class SoundManager : public Singleton<SoundManager> {

public:

	// サウンド関係の構造体
	// チャンクヘッダ
	struct ChunkHeader {
		std::array<char, 4> id;		// チャンク毎のID
		int32_t size;				// チャンクサイズ
	};
	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;			// "RIFF"
		std::array<char, 4> type;	//"WAVE
	};
	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk;			// "fmt"
		WAVEFORMATEX fmt;			// 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		WAVEFORMATEX wfex;			// 波形フォーマット
		BYTE* pBuffer;				// バッファの先頭アドレス
		unsigned int bufferSize;	// バッファのサイズ
		IXAudio2SourceVoice* source;
		float volume;
	};


private:
	friend Singleton<SoundManager>;
	SoundManager() {};

public:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	void SettingSoundManager();
	// サウンド再生
	SoundData SoundLoadWave(char* FileName);
	// 音声データの再生
	void SoundUnload(SoundData* SoundData);
	// 音声再生
	void SoundPlayerWave(SoundData& SoundData, int LoopCount);
};

//コールバック用
class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
public:
	//ボイス処理パスの開始時
	//STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
	//ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
	//バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd)(THIS) {};
	//バッファの使用開始時
	//STDMETHOD_(void, OnBufferStart)(THIS_ void* pBufferContext) {};
	//バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBUfferContext) {
		//バッファを開放する
		delete[] pBUfferContext;
	};
	//再生がループ位置に達した時
	//STDMETHOD_(void, OnLoopEnd)(THIS_ void* pBUfferContext) {};
	//ボイスの実行エラー時
	//STDMETHOD_(void, OnVoiceError)(THIS_ void* pBufferContext, HRESULT Error) {};
};