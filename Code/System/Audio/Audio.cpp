#include "Audio.h"

#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")

void Audio::StaticInit(){
	HRESULT result;

	//===================================================================
	// XAudio2 エンジンインスタンス 作成
	//===================================================================
	result = XAudio2Create(&xAudio2_,0,XAUDIO2_DEFAULT_PROCESSOR);

	//===================================================================
	// MasteringVoice 作成
	//===================================================================
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

void Audio::StaticFinalize(){
	xAudio2_.Reset();
	masterVoice_->DestroyVoice();
}

void Audio::Init(const std::string& fileName){
	soundData_ = LoadWave(fileName);
}

void Audio::Play(){
	isLoop_ = false;

	HRESULT result;

	pSourceVoice_ = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice_,&soundData_.wfex);
	assert(SUCCEEDED(result));

	XAUDIO2_BUFFER buffer = {};
	buffer.pAudioData = soundData_.pBuffer;
	buffer.AudioBytes = soundData_.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	result = pSourceVoice_->SubmitSourceBuffer(&buffer);
	result = pSourceVoice_->Start();

}

void Audio::Pause(){
	pSourceVoice_->Stop(0);
}

void Audio::Finalize(){
}

SoundData Audio::LoadWave(const std::string& fileName){
	HRESULT result;

	//===================================================================
	// file Open
	//===================================================================
	std::ifstream file;
	file.open(fileName,std::ios_base::binary);
	assert(file.is_open());

	//===================================================================
	// Load file
	//===================================================================
	RiffHeader riff;
	file.read((char*)&riff,sizeof(riff));

	// ファイルが RIFF か チェック
	if(strncmp(riff.chunk.id,"RIFF",4) != 0){
		assert(false);
	}
	// フォーマットが WAVE か チェック
	if(strncmp(riff.type,"WAVE",4) != 0){
		assert(false);
	}

	FormatChunk format = {};
	file.read((char*)&format,sizeof(ChunkHeader));
	// チャンクヘッダーの確認
	if(strncmp(format.chunk.id,"fmt",4) != 0){
		assert(false);
	}
	// チャンク本体の 確認
	assert(format.chunk.size <= sizeof(format.fmt));

	file.read((char*)&format.chunk.id,format.chunk.size);
	ChunkHeader data;
	file.read((char*)&data,sizeof(data));
	if(strncmp(data.id,"JUNK",4) == 0){
		// 読み取り位置を JUNKチャンク 終了位置まで 進める
		file.seekg(data.size,std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data,sizeof(data));
	}

	if(strncmp(data.id,"data",4) != 0){
		assert(false);
	}

	char* pBuff = new char[data.size];
	file.read(pBuff,data.size);

	file.close();

	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuff);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::SoundUnLoad(){
}
