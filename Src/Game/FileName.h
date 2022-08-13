#pragma once
#include <string>
#include <vector>
#include <wtypes.h>
#include <stdexcept>
#include <stdlib.h>
#include <array>
#include "FString.h"

namespace FFILE {

	bool GetFileNames(std::string FolderPath, std::vector<std::string>& FileNames)
	{
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		std::string searchName = FolderPath + "\\*";

		hFind = FindFirstFile(FString::StringToWString(searchName).c_str(), &win32fd);

		if (hFind == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("file not found");
			return false;
		}

		/* �w��̃f�B���N�g���ȉ��̃t�@�C�������t�@�C�����Ȃ��Ȃ�܂Ŏ擾���� */
		do {
			if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				/* �f�B���N�g���̏ꍇ�͉������Ȃ� */
				printf("directory\n");
			}
			else {
				/* �t�@�C��������������Vector�z��ɕۑ����� */
				std::wstring fileName = win32fd.cFileName;

				FileNames.emplace_back(FString::WStringToString(fileName));
				printf("%s\n", FileNames.back().c_str());
			}
		} while (FindNextFile(hFind, &win32fd));

		FindClose(hFind);

		return true;
	}

}