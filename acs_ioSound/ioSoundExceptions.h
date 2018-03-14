#pragma once
#include <exception>

struct ioSoundInitErr : std::exception {
	ioSoundInitErr() :std::exception("acs::ioSound Failed to Init!!") {}
};
struct ioSoundStreamReadErr : std::exception {
	ioSoundStreamReadErr() :std::exception("acs::SoundStream Failed to Read!!") {}
};

#define  ACS_IOSOUND_BASE unsigned long long refCnt = 1; virtual void ACS_IOSOUND_CLASS_CALL Addref(){ refCnt++;}	virtual void ACS_IOSOUND_CLASS_CALL Release(){if(refCnt == 1){delete this; return;} refCnt--;};