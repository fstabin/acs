#pragma once
#include "acs_ioSound.h"
#include "ioSoundExceptions.h"
#include <atlutil.h>
#include "AtDef.h"
using namespace acs::sound;

class WaveSound:public ISound {
	std::unique_ptr<BYTE[]>pcmBuffer;
	PCM_FORMAT fomat;
	PCM_DATA data;
public:
	WaveSound(const wchar_t* pass);
	WaveSound(const PCM_FORMAT&, const PCM_DATA&);
	ACS_IOSOUND_BASE
	bool ReadWave(HANDLE);

	virtual PCM_DATA& __thiscall Data();
	virtual PCM_FORMAT& __thiscall Fomat();
	virtual bool __thiscall SetData(const PCM_DATA&);
	virtual bool __thiscall Save(const wchar_t* pass);
};
class WSS:public ISoundStream {
	PCM_FORMAT fomat;
	DWORD DataChankLength;
	LONG DataChankHead;
	LONG CurrentDataChankPoint;
	CHandle hFile;
public:
	WSS(const wchar_t* pass);
	ACS_IOSOUND_BASE
	virtual PCM_FORMAT& __thiscall GetFomat();
	virtual bool __thiscall Reset();
	//0 = é∏îs,1 = ê¨å˜,2 = èIóπÇ‹Ç≈íBÇµÇΩ
	virtual int __thiscall ReadData(PCM_DATA* dest_buffer, unsigned long bufferReadMax);
};
