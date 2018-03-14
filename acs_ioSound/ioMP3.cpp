#include "stdafx.h"
#include "ioMP3.h"
#include "file_util.h"
#pragma comment(lib,"msacm32.lib")

//WAVE_FORMAT_MPEGLAYER3
const WORD bitRateTable[][16] = {
	{ 0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,-1 },//MPEG1/L1
	{ 0,32,48,56,64,80,96,112,128,160,192,224,256,320,384,-1 },//MPEG1/L2
	{ 0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,-1 },//MPEG1/L3
	{ 0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,-1 },//MPEG2/L1
	{ 0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,-1 }//MPEG2/L2,L3 MPEG2.5
};
const WORD sampleRateTable[][4] = {
	{ 44100, 48000, 32000, -1 }, // MPEG1
	{ 22050, 24000, 16000, -1 }, // MPEG2
	{ 11025, 12000, 8000, -1 } // MPEG2.5
};

void ReadMP3DataOffs(HANDLE hFile, DWORD& offs, DWORD& size) {
	DWORD fileSize = GetFileSize(hFile, NULL);
	BYTE header[10];
	DWORD dwRead;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))THROW("File Read Err!!");
	if (0 == ReadFile(hFile, header, 10, &dwRead, NULL))THROW("File Read Err!!");
	//ID3v2
	if (memcmp(header, "ID3", 3) == 0) {
		DWORD tagSize =
			((header[6] << 21) |
			(header[7] << 14) |
				(header[8] << 7) |
				(header[9])) + 10;
		offs = tagSize;
		size = fileSize - offs;
	}
	//ID3v1
	else {
		BYTE tag[3];
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, fileSize - 128, NULL, FILE_BEGIN))THROW("File Read Err!!");
		if (0 == ReadFile(hFile, tag, 3, &dwRead, NULL))THROW("File Read Err!!");

		offs = 0;
		if (memcmp(tag, "TAG", 3) == 0)
			size = fileSize - 128; // 末尾のタグを省く
		else
			size = fileSize; // ファイル全体がMP3データ
	}
}
void ReadMP3FrameFomat(HANDLE hFile, DWORD offs, DWORD& frameOffs, MPEGLAYER3WAVEFORMAT & mf) {
	DWORD header;
	DWORD dwRead;
	BYTE bversion;
	BYTE blayer;
	BYTE bbitRate;
	BYTE bsampleRate;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, offs, NULL, FILE_BEGIN))THROW("File Read Err!!");
	if (0 == ReadFile(hFile, &header, 4, &dwRead, NULL))THROW("File Read Err!!");
	if ((dwRead >> 16) & 0x01)frameOffs = offs + 4 + 16;
	bversion = (header >> 19) & 0x03;
	blayer = (header >> 17) & 0x03;
	bbitRate = (header >> 12) & 0x15;
	bsampleRate = (header >> 10) & 0x03;

	INT index;
	if (bversion == 3) {
		index = 3 - blayer;
	}
	else {
		if (blayer == 3)
			index = 3;
		else
			index = 4;
	}

	WORD bitRate = bitRateTable[index][bbitRate];

	switch (bversion) {
	case 0:
		index = 2;
		break;
	case 2:
		index = 1;
		break;
	case 3:
		index = 0;
		break;
	}
	WORD sampleRate = sampleRateTable[index][bsampleRate];

	BYTE padding = header >> 9 & 0x01;
	BYTE channel = header >> 6 & 0x03;

	// サイズ取得
	WORD blockSize = ((144 * bitRate * 1000) / bsampleRate) + padding;

	// フォーマット取得
	mf.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
	mf.wfx.nChannels = channel == 3 ? 1 : 2;
	mf.wfx.nSamplesPerSec = sampleRate;
	mf.wfx.nAvgBytesPerSec = (bitRate * 1000) / 8;
	mf.wfx.nBlockAlign = 1;
	mf.wfx.wBitsPerSample = 0;
	mf.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;

	mf.wID = MPEGLAYER3_ID_MPEG;
	mf.fdwFlags =
		padding ?
		MPEGLAYER3_FLAG_PADDING_ON :
		MPEGLAYER3_FLAG_PADDING_OFF;
	mf.nBlockSize = blockSize;
	mf.nFramesPerBlock = 1;
	mf.nCodecDelay = 1393;
}


MP3Sound::MP3Sound(const wchar_t* pass) {
	CHandle hFile(__OpenFile(pass, GENERIC_READ, FILE_SHARE_READ));
	if (hFile == INVALID_HANDLE_VALUE)THROW("Open File Err!!");

	DWORD DataOffs, DataSize;
	ReadMP3DataOffs(hFile, DataOffs, DataSize);
	DWORD FrameOffs;
	MPEGLAYER3WAVEFORMAT fmt;
	ReadMP3FrameFomat(hFile, DataOffs, FrameOffs, fmt);

	WAVEFORMATEX wfmf;
	wfmf.wFormatTag = WAVE_FORMAT_PCM;
	acmFormatSuggest(
		NULL,
		&fmt.wfx,
		&wfmf,
		sizeof(WAVEFORMATEX),
		ACM_FORMATSUGGESTF_WFORMATTAG
	);

	DWORD waveDataSize;
	{
		CHAcmStream hStream;
		std::unique_ptr<BYTE[]> ptr;
		CAcmStreamHeader ash;
		if (0 != acmStreamOpen(&hStream, NULL, &fmt.wfx, &wfmf, NULL, 0, 0, 0))THROW("MP3 Decoder Not Supported!!");
		//サイズ取得
		acmStreamSize(
			hStream,
			DataSize,
			&waveDataSize,
			ACM_STREAMSIZEF_SOURCE
		);
		//バッファ作成
		ptr = std::make_unique<BYTE[]>(DataSize);
		pcmBuffer = std::make_unique<BYTE[]>(waveDataSize);
		ash.h().cbStruct = sizeof(ACMSTREAMHEADER);
		ash.h().pbSrc = ptr.get();
		ash.h().cbSrcLength = DataSize;
		ash.h().pbDst = pcmBuffer.get();
		ash.h().cbDstLength = waveDataSize;

		ash.prepare(hStream);

		DWORD dwRead;
		DWORD ReadedSize;
		if(FALSE == ReadFile(hFile, ash.h().pbSrc, DataSize, &dwRead, NULL))THROW("ReadFile Err!!");
		ReadedSize = dwRead;
		if (0 != acmStreamConvert(hStream, &ash, ACM_STREAMCONVERTF_BLOCKALIGN))THROW("acmStreamConvert Err!!");
	}

	// 動的確保したデータを開放
	data.buffer = pcmBuffer.get();
	data.byteLength = waveDataSize;
	fomat.wFormatTag = wfmf.wFormatTag;//WAVE_FORMAT_PCM;
	fomat.nChannels = wfmf.nChannels;			//チャンネル
	fomat.nSamplesPerSec = wfmf.nSamplesPerSec;	//サンプリングレート
	fomat.nAvgBytesPerSec = wfmf.nAvgBytesPerSec;//一秒当たりの必要データ量(データの最小単位 * サンプリングレート)
	fomat.nBlockAlign = wfmf.nBlockAlign; //データの最小単位(１サンプルあたりのビット数 / 8 * チャンネル数)
	fomat.wBitsPerSample = wfmf.wBitsPerSample;//１サンプルあたりのビット数
}
MP3Sound::MP3Sound(const PCM_FORMAT&_fmt, const PCM_DATA&_data) {
	SetData(_data);
	fomat = _fmt;
}

MP3Sound::~MP3Sound() {
}

bool __thiscall MP3Sound::SetData(const PCM_DATA& n_pcm) {
	pcmBuffer = std::make_unique<BYTE[]>(n_pcm.byteLength);
	data.buffer = pcmBuffer.get();
	memcpy(data.buffer, n_pcm.buffer, n_pcm.byteLength);
	data.byteLength = n_pcm.byteLength;
	return true;
}

PCM_DATA& __thiscall MP3Sound::Data() {
	return data;
}
PCM_FORMAT& __thiscall MP3Sound::Fomat() {
	return fomat;
}

//未実装
bool __thiscall MP3Sound::Save(const wchar_t* pass) {
	return false;
	HACMSTREAM hStream;
	CHandle hFile(__OpenFile(pass, GENERIC_WRITE, NULL));
	if (hFile == INVALID_HANDLE_VALUE)THROW("Open File Err!!");

	DWORD DataOffs, DataSize;
	ReadMP3DataOffs(hFile, DataOffs, DataSize);
	DWORD FrameOffs;
	MPEGLAYER3WAVEFORMAT fmt;
	ReadMP3FrameFomat(hFile, DataOffs, FrameOffs, fmt);

	WAVEFORMATEX wfmf;
	wfmf.wFormatTag = WAVE_FORMAT_PCM;
	acmFormatSuggest(
		NULL,
		&fmt.wfx,
		&wfmf,
		sizeof(WAVEFORMATEX),
		ACM_FORMATSUGGESTF_WFORMATTAG
	);
	if (0 != acmStreamOpen(&hStream, NULL, &fmt.wfx, &wfmf, NULL, 0, 0, 0))THROW("MP3 Decoder Not Supported!!");
	DWORD waveDataSize;
	acmStreamSize(
		hStream,
		DataSize,
		&waveDataSize,
		ACM_STREAMSIZEF_SOURCE
	);

	ACMSTREAMHEADER ash = { 0 };
	ash.cbStruct = sizeof(ACMSTREAMHEADER);
	ash.pbSrc = new BYTE[DataSize];
	ash.cbSrcLength = DataSize;
	pcmBuffer = std::make_unique<BYTE[]>(waveDataSize);
	ash.pbDst = pcmBuffer.get();
	ash.cbDstLength = waveDataSize;

	if (0 != acmStreamPrepareHeader(hStream, &ash, 0))THROW("acmStreamPrepareHeader Err!!");

	DWORD dwRead;
	DWORD ReadedSize;
	if(FALSE == ReadFile(hFile, ash.pbSrc, DataSize, &dwRead, NULL))THROW("ReadFile Err!!");
	ReadedSize += dwRead;
	if (0 != acmStreamConvert(hStream, &ash, ACM_STREAMCONVERTF_BLOCKALIGN))THROW("acmStreamConvert Err!!");

	// ACMの後始末
	if (0 != acmStreamUnprepareHeader(hStream, &ash, 0))THROW("acmStreamUnprepareHeader Err!!");
	if (0 != acmStreamClose(hStream, 0))THROW("acmStreamClose Err!!");

	// 動的確保したデータを開放
	delete[] ash.pbSrc;
	data.buffer = pcmBuffer.get();
	data.byteLength = waveDataSize;
	fomat.wFormatTag = wfmf.wFormatTag;//WAVE_FORMAT_PCM;
	fomat.nChannels = wfmf.nChannels;			//チャンネル
	fomat.nSamplesPerSec = wfmf.nSamplesPerSec;	//サンプリングレート
	fomat.nAvgBytesPerSec = wfmf.nAvgBytesPerSec;//一秒当たりの必要データ量(データの最小単位 * サンプリングレート)
	fomat.nBlockAlign = wfmf.nBlockAlign; //データの最小単位(１サンプルあたりのビット数 / 8 * チャンネル数)
	fomat.wBitsPerSample = wfmf.wBitsPerSample;//１サンプルあたりのビット数
}


MP3SS::MP3SS(const wchar_t* pass) {
	hFile.Attach(__OpenFile(pass, GENERIC_READ, FILE_SHARE_READ));
	if (hFile == INVALID_HANDLE_VALUE)THROW("Open File Err!!");

	DWORD DataOffs, DataSize;
	ReadMP3DataOffs(hFile, DataOffs, DataSize);
	MPEGLAYER3WAVEFORMAT fmt;
	ReadMP3FrameFomat(hFile, DataOffs, FrameOffs, fmt);
	mp3StreamRemaining = DataSize - (FrameOffs - DataOffs);
	mp3DataSize = mp3StreamRemaining;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, FrameOffs, NULL, FILE_BEGIN))THROW("File Read Err!!");

	WAVEFORMATEX wfmf;
	wfmf.wFormatTag = WAVE_FORMAT_PCM;
	acmFormatSuggest(
		NULL,
		&fmt.wfx,
		&wfmf,
		sizeof(WAVEFORMATEX),
		ACM_FORMATSUGGESTF_WFORMATTAG
	);
	if (0 != acmStreamOpen(&hStream, NULL,&fmt.wfx, &wfmf, NULL, 0, 0, 0))THROW("MP3 Decoder Not Supported!!");

	DWORD waveBlockSize;
	//サイズ取得
	acmStreamSize(
		hStream,
		fmt.nBlockSize,
		&waveBlockSize,
		ACM_STREAMSIZEF_SOURCE
	);
	mp3BufSize = fmt.nBlockSize;
	pcmBufSize = waveBlockSize;
	mp3BlockBuffer = std::make_unique<BYTE[]>(fmt.nBlockSize);
	pcmBuffer = std::make_unique<BYTE[]>(waveBlockSize);
	//フォーマット設定
	fomat.wFormatTag = wfmf.wFormatTag;//WAVE_FORMAT_PCM;
	fomat.nChannels = wfmf.nChannels;			//チャンネル
	fomat.nSamplesPerSec = wfmf.nSamplesPerSec;	//サンプリングレート
	fomat.nAvgBytesPerSec = wfmf.nAvgBytesPerSec;//一秒当たりの必要データ量(データの最小単位 * サンプリングレート)
	fomat.nBlockAlign = wfmf.nBlockAlign; //データの最小単位(１サンプルあたりのビット数 / 8 * チャンネル数)
	fomat.wBitsPerSample = wfmf.wBitsPerSample;//１サンプルあたりのビット数

	//バッファ作成
	ash.h().cbStruct = sizeof(ACMSTREAMHEADER);
	ash.h().pbSrc = mp3BlockBuffer.get();
	ash.h().cbSrcLength = fmt.nBlockSize;
	ash.h().pbDst = pcmBuffer.get();
	ash.h().cbDstLength = waveBlockSize;

	pcmBufCurrent = pcmBuffer.get();

	ash.prepare(hStream);
}
PCM_FORMAT& __thiscall MP3SS::GetFomat() {
	return fomat;
 }
bool __thiscall MP3SS::Reset() {
	pcmBufCurrent = pcmBuffer.get();
	mp3StreamRemaining = mp3DataSize;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, FrameOffs, NULL, FILE_BEGIN))return false;
	return true;
 }
//0 = 失敗,1 = 成功,2 = 終了まで達した
int __thiscall MP3SS::ReadData(PCM_DATA* dest_buffer, unsigned long bufferReadMax) {
	{
		DWORD dwRead;
		dest_buffer->byteLength = 0;
		BYTE* destBufCurrent = reinterpret_cast<BYTE*>(dest_buffer->buffer);
		while (true)
		{
			//pcmバッファが空の時
			if (pcmBufRemaining == 0) {
				if (FALSE == ReadFile(hFile, ash.h().pbSrc, std::min(ash.h().cbSrcLength, mp3StreamRemaining), &dwRead, NULL))return 0;
				mp3StreamRemaining -= dwRead;
				if (0 != acmStreamConvert(hStream, &ash, ACM_STREAMCONVERTF_BLOCKALIGN))return 0;
				pcmBufRemaining = ash.h().cbDstLengthUsed;
				pcmBufCurrent = pcmBuffer.get();
			}
			{
				//pcmバッファの内容を出力バッファにコピー
				DWORD copySize = std::min(pcmBufRemaining, bufferReadMax);
				memcpy(destBufCurrent, pcmBufCurrent, copySize);
				bufferReadMax -= copySize;//読み込む残りサイズを減らす
				pcmBufRemaining -= copySize;
				destBufCurrent += copySize;//バッファの開始位置変更
				pcmBufCurrent += copySize;
				dest_buffer->byteLength += copySize;//読み取ったサイズを加算
			}
			if (mp3StreamRemaining == 0 && pcmBufRemaining == 0)return 2;//バッファがすべて読み切られているとき
			if (bufferReadMax == 0)return 1;//出力の最大サイズを読み切ったとき
		}
	}

	//data.buffer = pcmBuffer.get();
	//data.byteLength = waveBlockSize;
 }