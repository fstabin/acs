// acs_ioSound.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "acs_ioSound.h"
#include "ioWave.h"
#include "ioMP3.h"
#include<Shlwapi.h>
using namespace acs::sound;

class _ioSound:public IioSound {
public:
	ACS_IOSOUND_BASE
	virtual bool CreateSound(const wchar_t* pass, SOUND_FILE_FOMAT fmt, ISound** ppSound) {
		try {
			switch (fmt)
			{
			case SOUND_FILE_FOMAT::WAVE:
			WAVE:
				*ppSound = new WaveSound(pass);
				return true;
				break;
			case SOUND_FILE_FOMAT::MP3:
			MP3:

				return true;
				break;
			case SOUND_FILE_FOMAT::AUTO:
			default:
			{
				const wchar_t* ext = ::PathFindExtension(pass);
				if (StrCmpW(ext, L"wave") == 0)goto WAVE;
				if (StrCmpW(ext, L"wav") == 0)goto WAVE;
				if (StrCmpW(ext, L"mp3") == 0)goto MP3;
			}
			break;
			}
			return false;
		}
		catch(const std::exception& exp) {
			OutputDebugStringA(exp.what());
			return false;
		}
	}
	virtual bool CreateSound(const PCM_FORMAT& pfmt, const PCM_DATA& pdata, SOUND_FILE_FOMAT fmt, ISound**ppSound) {
		try {
			switch (fmt)
			{
			break;
			case SOUND_FILE_FOMAT::WAVE:
				*ppSound = new WaveSound(pfmt, pdata);
				return true;
				break;
			case SOUND_FILE_FOMAT::MP3:
				//*ppSound = new WaveSound(pfmt, pdata);
				return true;
				break;
			case SOUND_FILE_FOMAT::AUTO:
			default:
				break;
			}
			return false;
		}
		catch (const std::exception& exp) {
			OutputDebugStringA(exp.what());
			return false;
		}
	}
	virtual bool CrateSoundStream(const wchar_t* pass, SOUND_FILE_FOMAT fmt, ISoundStream** ppSS) {
		try {
			switch (fmt)
			{
				break;
			case SOUND_FILE_FOMAT::WAVE:
			WAVE:
				*ppSS = new WSS(pass);
				return true;
				break;
			case SOUND_FILE_FOMAT::MP3:
			MP3:
				*ppSS = new MP3SS(pass);
				return true;
				break;
			case SOUND_FILE_FOMAT::AUTO:
			default:
			{
				const wchar_t* ext = ::PathFindExtension(pass);
				if (StrCmpW(ext, L".wave") == 0)goto WAVE;
				if (StrCmpW(ext, L".wav") == 0)goto WAVE;
				if (StrCmpW(ext, L".mp3") == 0)goto MP3;
			}
				OutputDebugStringA("no support fomat");
				break;
			}
			return false;
		}
		catch (const std::exception& exp) {
			OutputDebugStringA(exp.what());
			return false;
		}
	}
};


ACS_IOSOUND_API bool __stdcall acs::sound::CreateioSound(IioSound** ppioSound) {
	*ppioSound = new ::_ioSound();
	return true;
}






