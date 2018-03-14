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
	virtual bool __thiscall Save(const wchar_t* pass);//������
};
class MIDISS :public ISoundStream {
	CHandle hFile;
	std::unique_ptr<BYTE[]>pcmBuffer;
	std::unique_ptr<BYTE[]>mp3BlockBuffer;
	CHAcmStream hStream;
	CAcmStreamHeader ash;
	DWORD mp3BufSize;//�o�b�t�@�T�C�Y
	DWORD pcmBufSize;//

	DWORD mp3StreamRemaining = 0;//mp3�X�g���[���f�[�^�̎c��

	DWORD pcmBufRemaining = 0;//pcm�o�b�t�@�̎c��o�C�g
	BYTE* pcmBufCurrent = 0;//pcm�o�b�t�@�̎c��o�C�g


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
	//0 = ���s,1 = ����,2 = �I���܂ŒB����
	virtual int __thiscall ReadData(PCM_DATA* dest_buffer, unsigned long bufferReadMax);
};