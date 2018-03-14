#include "stdafx.h"
#include "ioWave.h"
#include "file_util.h"
#include <Mmreg.h>
#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
#define HR_CHECK(hr) if (FAILED(hr))return false;

typedef struct Chunk {
	unsigned __int32 size = 0;
	void* data = 0;
}Chank;
typedef Chank FMTChunk;
typedef Chank DataChunk;

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{

	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;
	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());
		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());
		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}
		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}
		dwOffset += dwChunkDataSize;
		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}
	return S_OK;
}
HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset) {
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}

//WaveSound
WaveSound::WaveSound(const wchar_t* pass) {
	bool re;
	//ファイルオープン
	CHandle hFile(__OpenFile(pass, GENERIC_READ, FILE_SHARE_READ));
	//wave読み込み作成
	re = ReadWave(hFile);
	return;
}
WaveSound::WaveSound(const PCM_FORMAT& _fmt, const PCM_DATA& _data) {
	SetData(_data);
	fomat = _fmt;
}

PCM_DATA& WaveSound::Data() {
	return data;
}
PCM_FORMAT& WaveSound::Fomat() {
	return fomat;
}

 bool __thiscall WaveSound::SetData(const PCM_DATA& n_pcm) {
	pcmBuffer = std::make_unique<BYTE[]>(n_pcm.byteLength);
	data.buffer = pcmBuffer.get();
	memcpy(data.buffer, n_pcm.buffer, n_pcm.byteLength);
	data.byteLength = n_pcm.byteLength;
	 return true;
}

 bool WaveSound::ReadWave(HANDLE hFile) {
	 HRESULT hr;

	 DWORD dwChunkSize; DWORD dwChunkPosition;
	 //ファイルタイプ取得
	 hr = (FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition));
	 HR_CHECK(hr);
	 DWORD filetype;
	 hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	 HR_CHECK(hr);
	 if (filetype != fourccWAVE)return false;

	 //フォーマット取得
	 hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	 HR_CHECK(hr);
	 WAVEFORMATEX waveformat;
	 FMTChunk fc;
	 fc.size = sizeof(WAVEFORMATEX);
	 fc.data = &waveformat;
	 HR_CHECK(hr);

	 hr = ReadChunkData(hFile, fc.data, fc.size, dwChunkPosition);
	 HR_CHECK(hr);

	 //データサイズ取得
	 hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	 HR_CHECK(hr);
	 //データ読み込み
	 pcmBuffer = std::make_unique<BYTE[]>(dwChunkSize);
	 DataChunk dc;
	 dc.data = pcmBuffer.get();
	 dc.size = dwChunkSize;
	 HR_CHECK(hr);
	 hr = ReadChunkData(hFile, dc.data, dwChunkSize, dwChunkPosition);
	 HR_CHECK(hr);


	 data.buffer = pcmBuffer.get();
	 data.byteLength = dc.size;

	 fomat.nAvgBytesPerSec = waveformat.nAvgBytesPerSec;
	 fomat.nBlockAlign = waveformat.nBlockAlign;
	 fomat.nChannels = waveformat.nChannels;
	 fomat.nSamplesPerSec = waveformat.nSamplesPerSec;
	 fomat.wBitsPerSample = waveformat.wBitsPerSample;
	 fomat.wFormatTag = waveformat.wFormatTag;

	 return true;
 }
 bool __thiscall WaveSound::Save(const wchar_t* pass) {
	 CHandle hFile(__OpenFile(pass, GENERIC_WRITE, NULL));

	 unsigned long Writebyte;
	 if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		 return SUCCEEDED(HRESULT_FROM_WIN32(GetLastError()));
	 const unsigned __int32 riff = fourccRIFF;
	 unsigned __int32 filesize = 0;
	 const unsigned __int32 wave = fourccWAVE;
	if(FALSE == WriteFile(hFile, &riff, 4, &Writebyte, NULL))return false;
	filesize += Writebyte;
	if (FALSE == WriteFile(hFile, &filesize, 4, &Writebyte, NULL))return false;
	filesize += Writebyte;
	if (FALSE == WriteFile(hFile, &wave, 4, &Writebyte, NULL))return false;
	 filesize += Writebyte;

	 const unsigned __int32 tagFMT = fourccFMT;
	 WORD size_wf = sizeof(WAVEFORMATEX);
	 WAVEFORMATEX waveformat;
	 waveformat.cbSize = 0;
	 waveformat.nAvgBytesPerSec = fomat.nAvgBytesPerSec;
	 waveformat.nBlockAlign = fomat.nBlockAlign;
	 waveformat.nChannels = fomat.nChannels;
	 waveformat.nSamplesPerSec = fomat.nSamplesPerSec;
	 waveformat.wBitsPerSample = fomat.wBitsPerSample;
	 waveformat.wFormatTag = fomat.wFormatTag;
	 if (FALSE == WriteFile(hFile, &tagFMT, 4, &Writebyte, NULL))return false;
	 filesize += Writebyte;
	 if (FALSE == WriteFile(hFile, &size_wf, 4, &Writebyte, NULL))return false;
	 filesize += Writebyte;
	 if (FALSE == WriteFile(hFile, &waveformat, size_wf, &Writebyte, NULL))return false;
	 filesize += Writebyte;

	 const unsigned __int32 tagDATA = fourccDATA;
	 if (FALSE == WriteFile(hFile, &tagDATA, 4, &Writebyte, NULL))return false;
	 filesize += Writebyte;
	 DWORD data_size = 0;
	 if (FALSE == WriteFile(hFile, &data_size, 4, &Writebyte, NULL))return false;
	 data.byteLength = data_size;
	 filesize += Writebyte;
	 if (FALSE == WriteFile(hFile, data.buffer, data_size, &Writebyte, NULL))return false;
	 filesize += Writebyte;
	 if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 4, NULL, FILE_BEGIN))
		 return SUCCEEDED(HRESULT_FROM_WIN32(GetLastError()));
	 if (FALSE == WriteFile(hFile, &filesize, 4, &Writebyte, NULL))return false;

	 return true;
 }


 bool GetFomatAndDataHead(HANDLE hFile, PCM_FORMAT& fomat, DWORD& DataChankLen, LONG& DataChankHead) {
	 //ファイルタイプ取得
	 HRESULT hr;
	 DWORD dwChunkSize; DWORD dwChunkPosition;
	 hr = (FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition));
	 HR_CHECK(hr);
	 DWORD filetype;
	 hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	 HR_CHECK(hr);
	 if (filetype != fourccWAVE)return false;

	 //フォーマット取得
	 hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	 HR_CHECK(hr);
	 WAVEFORMATEX waveformat;
	 FMTChunk fc;
	 fc.size = sizeof(WAVEFORMATEX);
	 fc.data = &waveformat;
	 HR_CHECK(hr);

	 hr = ReadChunkData(hFile, fc.data, fc.size, dwChunkPosition);
	 HR_CHECK(hr);

	 //データサイズ取得
	 hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	 HR_CHECK(hr);

	 fomat.nAvgBytesPerSec = waveformat.nAvgBytesPerSec;
	 fomat.nBlockAlign = waveformat.nBlockAlign;
	 fomat.nChannels = waveformat.nChannels;
	 fomat.nSamplesPerSec = waveformat.nSamplesPerSec;
	 fomat.wBitsPerSample = waveformat.wBitsPerSample;
	 fomat.wFormatTag = waveformat.wFormatTag;
	 DataChankLen = dwChunkSize;
	 DataChankHead = dwChunkPosition;
	 return true;
 }

 //WaveSoundStream
 WSS::WSS(const wchar_t* pass) {
	 //ファイルオープン
	hFile = CHandle(__OpenFile(pass, GENERIC_READ, FILE_SHARE_READ));
	if (hFile == INVALID_HANDLE_VALUE) THROW("Not found!!");
	if (!GetFomatAndDataHead(hFile, fomat, DataChankLength, DataChankHead))THROW("Filedata is Broken!!");
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, DataChankHead, NULL, FILE_BEGIN))THROW("SetFilePointer Err!!");
	 return;
 }
 PCM_FORMAT& __thiscall WSS::GetFomat() {
	 return fomat;
 }
 bool __thiscall WSS::Reset() {
	 CurrentDataChankPoint = DataChankHead;
	 if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, DataChankHead, NULL, FILE_BEGIN))return false;
	 return true;
 }
 //0 = 失敗,1 = 成功,2 = 終了まで達した
 int __thiscall WSS::ReadData(PCM_DATA* dest_buffer, unsigned long bufferReadMax) {
	 HRESULT hr = S_OK;
	
	 int resultval = 2;
	 DWORD dwRead;
	 DWORD ReadMax = DataChankLength - (CurrentDataChankPoint - DataChankHead);
	 if (ReadMax > bufferReadMax) {
		 resultval = 1;
		 ReadMax = bufferReadMax;
	 }
	 if (0 == ReadFile(hFile, dest_buffer->buffer, ReadMax, &dwRead, NULL))return 0;
	 dest_buffer->byteLength = dwRead;
	 CurrentDataChankPoint += dwRead;
	 return resultval;
 }


