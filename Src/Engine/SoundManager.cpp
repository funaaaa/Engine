#include "SoundManager.h"
#include <cassert>
#include "DirectXBase.h"

#pragma comment(lib,"xaudio2.lib")

//XAudio2VoiceCallback�̃C���X�^���X���O���[�o���ϐ��Ƃ��ăC���X�^���X�����
//XAudio2VoiceCallback voiceCallback{};

void SoundManager::SettingSoundManager()
{
	// xaudio2�̃G���W���𐶐�
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// �}�X�^�[�{�C�X�𐶐�
	result = xAudio2->CreateMasteringVoice(&masterVoice);
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
	if (strncmp(riff.chunk.id.data(), "RIFF", 4) != 0) {
		assert(0);
	}
	// �^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type.data(), "WAVE", 4) != 0) {
		assert(0);
	}
	// Format�`�����N�̓ǂݍ���
	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id.data(), "fmt ", 4) != 0) {
		assert(0);
	}
	// �`�����N�{�̂̓ǂݍ���
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNK�`�����N�����o�����ꍇ
	if (strncmp(data.id.data(), "JUNK ", 4) == 0) {
		// �ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		// �ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id.data(), "LIST ", 4) == 0) {
		// �ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		// �ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id.data(), "data ", 4) != 0) {
		assert(0);
	}
	// Data�`�����N�̃f�[�^��(�g�`�f�[�^)�̓ǂݍ���
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	// Wave�t�@�C�������
	file.close();

	// return���邽�߂̉����f�[�^
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = static_cast<unsigned int>(data.size);
	soundData.volume = 1.0f;

	return soundData;

	////�B�T�E���h�Đ� �ǂݍ���.wav�t�@�C���̏�񂩂特�����Đ�����
	//WAVEFORMATEX wfex{};
	////�g�`�t�H�[�}�b�g�̐ݒ�
	//memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	//wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;
	////�g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
	//IXAudio2SourceVoice* pSourceVoice = nullptr;
	//result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	//if FAILED(result) {
	//	delete[] pBuffer;
	//	return;
	//}
	////�Đ�����g�`�f�[�^�̐ݒ�
	//XAUDIO2_BUFFER buf{};
	//buf.pAudioData = (BYTE*)pBuffer;
	//buf.pContext = pBuffer;
	//buf.Flags = XAUDIO2_END_OF_STREAM;
	//buf.AudioBytes = data.size;
	////�g�`�f�[�^�̍Đ�
	//result = pSourceVoice->SubmitSourceBuffer(&buf);
	//result = pSourceVoice->Start();
}

void SoundManager::SoundUnload(SoundData* SoundData)
{
	// �o�b�t�@�����������
	delete[]  SoundData->pBuffer;

	SoundData->pBuffer = 0;
	SoundData->bufferSize = 0;
	SoundData->wfex = {};
}

void SoundManager::SoundPlayerWave(SoundData& SoundData, int LoopCount)
{
	HRESULT result;

	// �g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &SoundData.wfex);
	assert(SUCCEEDED(result));

	// �Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = SoundData.pBuffer;
	buf.AudioBytes = SoundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = LoopCount;

	pSourceVoice->SetVolume(SoundData.volume);

	SoundData.source = pSourceVoice;

	// �g�`�f�[�^�̍Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}
