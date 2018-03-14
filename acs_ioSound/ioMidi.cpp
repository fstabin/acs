#include "stdafx.h"
#include "acs_ioSound.h"
#include "ioMidi.h"
#include<Shlwapi.h>
using namespace acs::sound;

MIDISound::MIDISound(const wchar_t* pass) {

}
MIDISound::MIDISound(const PCM_FORMAT&, const PCM_DATA&) {

}
MIDISound::~MIDISound() {

}

PCM_DATA& __thiscall MIDISound::Data() {

 }
PCM_FORMAT& __thiscall MIDISound::Fomat() {

 }
bool __thiscall MIDISound::SetData(const PCM_DATA&) {

}
bool __thiscall MIDISound::Save(const wchar_t* pass) {
	return false;
}