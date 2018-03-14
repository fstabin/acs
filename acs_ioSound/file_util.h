#pragma once

inline HANDLE __OpenFile(const wchar_t* pass, unsigned long flag, unsigned long share) {
	//ファイルオープン
	CHandle hFile(CreateFileW(pass, flag, share,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
	if (hFile) {
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
		if (INVALID_HANDLE_VALUE == hFile)return INVALID_HANDLE_VALUE;
		if (INVALID_SET_FILE_POINTER == ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN))return INVALID_HANDLE_VALUE;
		return hFile.Detach();
	}
	return INVALID_HANDLE_VALUE;
}