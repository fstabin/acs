#pragma once
#include "acs_ioSound.h"
#include "ioSoundExceptions.h"
#include <atlutil.h>
#include "AtDef.h"

#include <mmiscapi.h>
#include <Mmreg.h>
#include <MSAcm.h>

#include<Shlwapi.h>
using namespace acs::sound;

class CHAcmStream {
	HACMSTREAM hStream = NULL;
public:
	HACMSTREAM* operator &() { return &hStream; }
	operator HACMSTREAM&() { return hStream; }
	~CHAcmStream() {
		if (hStream) {
			acmStreamClose(hStream, 0);
		}
	}
};
class CAcmStreamHeader {
	HACMSTREAM hStream = NULL;
	ACMSTREAMHEADER hHeader = { 0 };
public:
	ACMSTREAMHEADER* operator &() { return &hHeader; }
	//CHACMStreamが死ぬまでACMSTREAMHEADERの実態は保持するべし
	void prepare(HACMSTREAM hst) {
		if (hStream == NULL) {
			if (0 != acmStreamPrepareHeader(hst, &hHeader, 0))THROW("acmStreamPrepareHeader Err!!");
			hStream = hst;
			return;
		}
		THROW("CAcmStreamHeader prepare Err!!");
	}
	void unPrepare() {
		if (hStream) {
			if (0 != acmStreamUnprepareHeader(hStream, &hHeader, 0))THROW("acmStreamUnprepareHeader Err!!");
			hStream = nullptr;
			return;
		}
		THROW("CAcmStreamHeader unPrepare Err!!");
	}
	operator ACMSTREAMHEADER&() { return hHeader; }
	ACMSTREAMHEADER& h() {return hHeader; }
	~CAcmStreamHeader() {
		unPrepare();
	}
};

class MP3Sound :public ISound {
	std::unique_ptr<BYTE[]>pcmBuffer;
	PCM_FORMAT fomat;
	PCM_DATA data;
public:
	ACS_IOSOUND_BASE
	MP3Sound(const wchar_t* pass);
	MP3Sound(const PCM_FORMAT&, const PCM_DATA&);
	~MP3Sound();
	virtual PCM_DATA& __thiscall Data();
	virtual PCM_FORMAT& __thiscall Fomat();
	virtual bool __thiscall SetData(const PCM_DATA&);
	virtual bool __thiscall Save(const wchar_t* pass);//未実装
};
class MP3SS :public ISoundStream {
	CHandle hFile;
	std::unique_ptr<BYTE[]>pcmBuffer;
	std::unique_ptr<BYTE[]>mp3BlockBuffer;
	CHAcmStream hStream;
	CAcmStreamHeader ash;
	DWORD mp3BufSize;//バッファサイズ
	DWORD pcmBufSize;//

	DWORD mp3StreamRemaining = 0;//mp3ストリームデータの残り
	
	DWORD pcmBufRemaining = 0;//pcmバッファの残りバイト
	BYTE* pcmBufCurrent = 0;//pcmバッファの残りバイト
	

	DWORD FrameOffs;
	DWORD mp3DataSize;
	PCM_FORMAT fomat;
public:
	ACS_IOSOUND_BASE
	MP3SS(const wchar_t* pass);
	virtual PCM_FORMAT& __thiscall GetFomat();
	virtual bool __thiscall Reset();
	//0 = 失敗,1 = 成功,2 = 終了まで達した
	virtual int __thiscall ReadData(PCM_DATA* dest_buffer, unsigned long bufferReadMax);
};