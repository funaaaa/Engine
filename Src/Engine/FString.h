#pragma once
#include <string>
#include <Windows.h>

using namespace std;

namespace FString {

	void ConvertStringToWchar_t(string STRING, wchar_t* WCHAR_STRING, int ARRAY_SIZE)
	{
		MultiByteToWideChar(CP_ACP, 0, STRING.c_str(), -1, WCHAR_STRING, ARRAY_SIZE);
	}

}