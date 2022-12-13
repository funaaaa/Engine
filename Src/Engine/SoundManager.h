#pragma once
#include <fstream>
#include "Singleton.h"
#include <wrl/client.h>
#include <xaudio2.h>
#include <array>

// ���ǂݍ��݁A�Ǘ��N���X�B
class SoundManager : public Singleton<SoundManager> {

public:

	// �T�E���h�֌W�̍\����
	// �`�����N�w�b�_
	struct ChunkHeader {
		std::array<char, 4> id_;		// �`�����N����ID
		int32_t size_;				// �`�����N�T�C�Y
	};
	// RIFF�w�b�_�`�����N
	struct RiffHeader {
		ChunkHeader chunk_;			// "RIFF"
		std::array<char, 4> type_;	//"WAVE
	};
	// FMT�`�����N
	struct FormatChunk {
		ChunkHeader chunk_;			// "fmt"
		WAVEFORMATEX fmt_;			// �g�`�t�H�[�}�b�g
	};

	// �����f�[�^
	struct SoundData {
		WAVEFORMATEX wfex_;			// �g�`�t�H�[�}�b�g
		BYTE* pBuffer_;				// �o�b�t�@�̐擪�A�h���X
		unsigned int bufferSize_;	// �o�b�t�@�̃T�C�Y
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
	// �T�E���h�Đ�
	SoundData SoundLoadWave(char* FileName);
	// �����f�[�^�̍Đ�
	void SoundUnload(SoundData* SoundData);
	// �����Đ�
	void SoundPlayerWave(SoundData& SoundData, int LoopCount);
};

//�R�[���o�b�N�p
class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
public:
	//�{�C�X�����p�X�̏I����
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
	//�o�b�t�@�X�g���[���̍Đ����I��������
	STDMETHOD_(void, OnStreamEnd)(THIS) {};
	//�o�b�t�@�̖����ɒB������
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBUfferContext) {
		//�o�b�t�@���J������
		delete[] pBUfferContext;
	};
};