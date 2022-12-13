#include "SoundManager.h"
#include <cassert>
#include "Engine.h"

#pragma comment(lib,"xaudio2.lib")

void SoundManager::SettingSoundManager()
{
	// xaudio2のエンジンを生成
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

SoundManager::SoundData SoundManager::SoundLoadWave(char* FileName)
{

	// ①ファイルオープン
	// ファイル入力ストリームのインスタンス
	std::ifstream file{};
	// .wavをバイナリモードで開く
	file.open(FileName, std::ios_base::binary);
	// ファイルオープンの失敗を検出する
	assert(file.is_open());

	// ②.wavの読み込みと再生
	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk_.id_.data(), "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type_.data(), "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk_.id_.data(), "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk_.size_ <= sizeof(format.fmt_));
	file.read((char*)&format.fmt_, format.chunk_.size_);
	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id_.data(), "JUNK ", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size_, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id_.data(), "LIST ", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size_, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id_.data(), "data ", 4) != 0) {
		assert(0);
	}
	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer_ = new char[data.size_];
	file.read(pBuffer_, data.size_);
	// Waveファイルを閉じる
	file.close();

	// returnするための音声データ
	SoundData soundData = {};
	soundData.wfex_ = format.fmt_;
	soundData.pBuffer_ = reinterpret_cast<BYTE*>(pBuffer_);
	soundData.bufferSize_ = static_cast<unsigned int>(data.size_);
	soundData.volume_ = 1.0f;

	return soundData;
}

void SoundManager::SoundUnload(SoundData* SoundData)
{
	// バッファメモリを解放
	delete[]  SoundData->pBuffer_;

	SoundData->pBuffer_ = 0;
	SoundData->bufferSize_ = 0;
	SoundData->wfex_ = {};
}

void SoundManager::SoundPlayerWave(SoundData& SoundData, int LoopCount)
{
	HRESULT result;

	// 波形フォーマットをもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &SoundData.wfex_);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = SoundData.pBuffer_;
	buf.AudioBytes = SoundData.bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = LoopCount;

	pSourceVoice->SetVolume(SoundData.volume_);

	SoundData.source_ = pSourceVoice;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}
