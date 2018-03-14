#pragma once
#include "acs_ioSound.h"
#include "ioSoundExceptions.h"
#include <atlutil.h>
#include "AtDef.h"

#include <mmiscapi.h>
#include <mmsystem.h >

#include<Shlwapi.h>
using namespace acs::sound;


class MIDISound :public ISound {
	std::unique_ptr<BYTE[]>pcmBuffer;
	PCM_FORMAT fomat;
	PCM_DATA data;
public:
	MIDISound(const wchar_t* pass);
	MIDISound(const PCM_FORMAT&, const PCM_DATA&);
	~MIDISound();
	virtual void Destroy() {
		delete this;
	}
	virtual PCM_DATA& __thiscall Data();
	virtual PCM_FORMAT& __thiscall Fomat();
	virtual bool __thiscall SetData(const PCM_DATA&);
	virtual bool __thiscall Save(const wchar_t* pass);//未実装
};
class MIDISS :public ISoundStream {
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
	MIDISS(const wchar_t* pass);
	virtual void Destroy() {
		delete this;
	}
	virtual PCM_FORMAT& __thiscall GetFomat();
	virtual bool __thiscall Reset();
	//0 = 失敗,1 = 成功,2 = 終了まで達した
	virtual int __thiscall ReadData(PCM_DATA* dest_buffer, unsigned long bufferReadMax);
};