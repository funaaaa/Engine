#pragma once
#include <string>
#include <Windows.h>
#include <vector>

using namespace std;

namespace FString {

	// ãåéÆ
	void ConvertStringToWchar_t(string STRING, wchar_t* WCHAR_STRING, int ARRAY_SIZE)
	{
		MultiByteToWideChar(CP_ACP, 0, STRING.c_str(), -1, WCHAR_STRING, ARRAY_SIZE);
	}

	// êVå^
	/*wchar_t* NewConvertStringToWchar_t(string String)
	{

		vector<wchar_t> data;

		int dataSize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, String.c_str(), -1, nullptr, 0);

		data.resize(dataSize);

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, String.c_str(), -1, data.data(), 0);

		return data.data();

	}*/

}