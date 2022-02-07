#include "SoundManager.h"
#include <cassert>
#include "Struct.h"

ComPtr<IXAudio2> SoundManager::xAudio2{};
IXAudio2MasteringVoice* SoundManager::masterVoice{};

//XAudio2VoiceCallbackのインスタンスをグローバル変数としてインスタンスを作る
XAudio2VoiceCallback voiceCallback{};

void SoundManager::SettingSoundManager()
{
	//xaudio2のエンジンを生成
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
}

SoundData SoundManager::SoundLoadWave(char* fileName)
{
	HRESULT result;

	//①ファイルオープン
	//ファイル入力ストリームのインスタンス
	ifstream file{};
	//.wavをバイナリモードで開く
	file.open(fileName, ios_base::binary);
	//ファイルオープンの失敗を検出する
	assert(file.is_open());

	//②.wavの読み込みと再生
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//Formatチャンクの読み込み
	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "LIST ", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data ", 4) != 0) {
		assert(0);
	}
	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//Waveファイルを閉じる
	file.close();

	//returnするための音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.volume = 1.0f;

	return soundData;

	////③サウンド再生 読み込んだ.wavファイルの情報から音声を再生する
	//WAVEFORMATEX wfex{};
	////波形フォーマットの設定
	//memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	//wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;
	////波形フォーマットをもとにSourceVoiceの生成
	//IXAudio2SourceVoice* pSourceVoice = nullptr;
	//result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	//if FAILED(result) {
	//	delete[] pBuffer;
	//	return;
	//}
	////再生する波形データの設定
	//XAUDIO2_BUFFER buf{};
	//buf.pAudioData = (BYTE*)pBuffer;
	//buf.pContext = pBuffer;
	//buf.Flags = XAUDIO2_END_OF_STREAM;
	//buf.AudioBytes = data.size;
	////波形データの再生
	//result = pSourceVoice->SubmitSourceBuffer(&buf);
	//result = pSourceVoice->Start();
}

void SoundManager::SoundUnload(SoundData* soundData)
{
	//バッファメモリを解放
	delete[]  soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void SoundManager::SoundPlayerWave(SoundData& soundData, int loopCount)
{
	HRESULT result;

	//波形フォーマットをもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopCount;

	pSourceVoice->SetVolume(soundData.volume);

	soundData.source = pSourceVoice;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}
