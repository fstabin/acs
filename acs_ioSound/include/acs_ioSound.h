#pragma once
#ifdef ACS_IOSOUND_EXPORTS
#define ACS_IOSOUND_API __declspec(dllexport)
#else
#define ACS_IOSOUND_API __declspec(dllimport)
#endif
#define ACS_IOSOUND_CLASS_CALL __thiscall
#include <exception>
#include <acs.h>

//use acs_ioSound.lib 
//			& acs_ioSound.dll

namespace acs {
	namespace sound {
		struct PCM_FORMAT {
			unsigned __int16 wFormatTag = 1;//WAVE_FORMAT_PCM;
			unsigned __int16 nChannels = 1;			//�`�����l��
			unsigned __int32 nSamplesPerSec = 44100;	//�T���v�����O���[�g
			unsigned __int32 nAvgBytesPerSec = 2 * 44100;//��b������̕K�v�f�[�^��(�f�[�^�̍ŏ��P�� * �T���v�����O���[�g)
			unsigned __int16 nBlockAlign = 16 / 8 * 1; //�f�[�^�̍ŏ��P��(�P�T���v��������̃r�b�g�� / 8 * �`�����l����)
			unsigned __int16 wBitsPerSample = 16;//�P�T���v��������̃r�b�g��
		};
		struct PCM_DATA {
			acs::byte* buffer;
			unsigned long long byteLength;
		};
		enum struct SOUND_FILE_FOMAT {
			AUTO, WAVE, MP3
		};

		class ISound :public IACS {
		public:
			virtual const PCM_DATA& ACS_IOSOUND_CLASS_CALL Data() = 0;
			virtual PCM_FORMAT& ACS_IOSOUND_CLASS_CALL Fomat() = 0;
			virtual bool ACS_IOSOUND_CLASS_CALL SetData(const PCM_DATA&) = 0;
			virtual bool ACS_IOSOUND_CLASS_CALL Save(const wchar_t* pass) = 0;
		};

		//read only
		class ISoundStream :public IACS {
		public:
			virtual PCM_FORMAT& ACS_IOSOUND_CLASS_CALL GetFomat() = 0;
			virtual bool ACS_IOSOUND_CLASS_CALL Reset() = 0;
			//0 = ���s,1 = ����,2 = �I���܂ŒB����
			virtual int ACS_IOSOUND_CLASS_CALL ReadData(PCM_DATA* dest_buffer, unsigned long bufferReadMax) = 0;
		};
		class IioSound:public IACS {
		public:
			virtual bool ACS_IOSOUND_CLASS_CALL CreateSound(const wchar_t* pass, SOUND_FILE_FOMAT, ISound**) = 0;
			virtual bool ACS_IOSOUND_CLASS_CALL CreateSound(const PCM_FORMAT&, const PCM_DATA&, SOUND_FILE_FOMAT, ISound**) = 0;
			virtual bool ACS_IOSOUND_CLASS_CALL CrateSoundStream(const wchar_t* pass, SOUND_FILE_FOMAT, ISoundStream**) = 0;
		};
		ACS_IOSOUND_API bool __stdcall CreateioSound(IioSound** ppioSound);
	}
}
