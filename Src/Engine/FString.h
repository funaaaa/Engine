#pragma once
#include <string>
#include <Windows.h>
#include <vector>

namespace FString {

	// 旧式
	inline void ConvertStringToWchar_t(std::string STRING, wchar_t* WCHAR_STRING, int ARRAY_SIZE)
	{
		MultiByteToWideChar(CP_ACP, 0, STRING.c_str(), -1, WCHAR_STRING, ARRAY_SIZE);
	}

	// 新型
	/*wchar_t* NewConvertStringToWchar_t(string String)
	{

		vector<wchar_t> data;

		int dataSize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, String.c_str(), -1, nullptr, 0);

		data.resize(dataSize);

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, String.c_str(), -1, data.data(), 0);

		return data.data();

	}*/

	inline std::wstring StringToWString(std::string OString)
	{
		// SJIS → wstring
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, OString.c_str()
			, -1, (wchar_t*)NULL, 0);

		// バッファの取得
		wchar_t* cpUCS2 = new wchar_t[iBufferSize];

		// SJIS → wstring
		MultiByteToWideChar(CP_ACP, 0, OString.c_str(), -1, cpUCS2
			, iBufferSize);

		// stringの生成
		std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

		// バッファの破棄
		delete[] cpUCS2;

		// 変換結果を返す
		return(oRet);
	}

	inline std::string WStringToString(std::wstring OWString)
	{
		// wstring → SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, OWString.c_str()
			, -1, (char*)NULL, 0, NULL, NULL);

		// バッファの取得
		CHAR* cpMultiByte = new CHAR[iBufferSize];

		// wstring → SJIS
		WideCharToMultiByte(CP_OEMCP, 0, OWString.c_str(), -1, cpMultiByte
			, iBufferSize, NULL, NULL);

		// stringの生成
		std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);

		// バッファの破棄
		delete[] cpMultiByte;

		// 変換結果を返す
		return(oRet);
	}

}