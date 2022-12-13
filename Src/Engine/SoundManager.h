#pragma once
#include <fstream>
#include "Singleton.h"
#include <wrl/client.h>
#include <xaudio2.h>
#include <array>

// 音読み込み、管理クラス。
class SoundManager : public Singleton<SoundManager> {

public:

	// サウンド関係の構造体
	// チャンクヘッダ
	struct ChunkHeader {
		std::array<char, 4> id_;		// チャンク毎のID
		int32_t size_;				// チャンクサイズ
	};
	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk_;			// "RIFF"
		std::array<char, 4> type_;	//"WAVE
	};
	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk_;			// "fmt"
		WAVEFORMATEX fmt_;			// 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		WAVEFORMATEX wfex_;			// 波形フォーマット
		BYTE* pBuffer_;				// バッファの先頭アドレス
		unsigned int bufferSize_;	// バッファのサイズ
		IXAudio2SourceVoice* source_;
		float volume_;
	};


private:
	friend Singleton<SoundManager>;
	SoundManager() {};

public:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

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
	//ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
	//バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd)(THIS) {};
	//バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBUfferContext) {
		//バッファを開放する
		delete[] pBUfferContext;
	};
};