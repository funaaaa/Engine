#include "SoundManager.h"
#include <cassert>
#include "Engine.h"

#pragma comment(lib,"xaudio2.lib")

void SoundManager::SettingSoundManager()
{
	// xaudio2�̃G���W���𐶐�
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// �}�X�^�[�{�C�X�𐶐�
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

SoundManager::SoundData SoundManager::SoundLoadWave(char* FileName)
{

	// �@�t�@�C���I�[�v��
	// �t�@�C�����̓X�g���[���̃C���X�^���X
	std::ifstream file{};
	// .wav���o�C�i�����[�h�ŊJ��
	file.open(FileName, std::ios_base::binary);
	// �t�@�C���I�[�v���̎��s�����o����
	assert(file.is_open());

	// �A.wav�̓ǂݍ��݂ƍĐ�
	// RIFF�w�b�_�[�̓ǂݍ���
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// �t�@�C����RIFF���`�F�b�N
	if (strncmp(riff.chunk_.id_.data(), "RIFF", 4) != 0) {
		assert(0);
	}
	// �^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type_.data(), "WAVE", 4) != 0) {
		assert(0);
	}
	// Format�`�����N�̓ǂݍ���
	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk_.id_.data(), "fmt ", 4) != 0) {
		assert(0);
	}
	// �`�����N�{�̂̓ǂݍ���
	assert(format.chunk_.size_ <= sizeof(format.fmt_));
	file.read((char*)&format.fmt_, format.chunk_.size_);
	// Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNK�`�����N�����o�����ꍇ
	if (strncmp(data.id_.data(), "JUNK ", 4) == 0) {
		// �ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size_, std::ios_base::cur);
		// �ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id_.data(), "LIST ", 4) == 0) {
		// �ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size_, std::ios_base::cur);
		// �ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id_.data(), "data ", 4) != 0) {
		assert(0);
	}
	// Data�`�����N�̃f�[�^��(�g�`�f�[�^)�̓ǂݍ���
	char* pBuffer_ = new char[data.size_];
	file.read(pBuffer_, data.size_);
	// Wave�t�@�C�������
	file.close();

	// return���邽�߂̉����f�[�^
	SoundData soundData = {};
	soundData.wfex_ = format.fmt_;
	soundData.pBuffer_ = reinterpret_cast<BYTE*>(pBuffer_);
	soundData.bufferSize_ = static_cast<unsigned int>(data.size_);
	soundData.volume_ = 1.0f;

	return soundData;
}

void SoundManager::SoundUnload(SoundData* SoundData)
{
	// �o�b�t�@�����������
	delete[]  SoundData->pBuffer_;

	SoundData->pBuffer_ = 0;
	SoundData->bufferSize_ = 0;
	SoundData->wfex_ = {};
}

void SoundManager::SoundPlayerWave(SoundData& SoundData, int LoopCount)
{
	HRESULT result;

	// �g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &SoundData.wfex_);
	assert(SUCCEEDED(result));

	// �Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = SoundData.pBuffer_;
	buf.AudioBytes = SoundData.bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = LoopCount;

	pSourceVoice->SetVolume(SoundData.volume_);

	SoundData.source_ = pSourceVoice;

	// �g�`�f�[�^�̍Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}
