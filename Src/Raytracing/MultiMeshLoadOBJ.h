#pragma once
#include "Singleton.h"
#include "Vec.h"
#include <vector>
#include <string>
#include <array>
#include <wtypes.h>

// OBJ�������b�V���N���X
class MultiMeshLoadOBJ : public Singleton<MultiMeshLoadOBJ> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::wstring> texturePath;

	std::vector<std::pair<std::vector<int>, int>> blasID;	// �e�N�X�`���̃n���h���ƁA������g�p����BLAS��ID��ۑ�����R���e�i

	using string = std::string;

	bool isOpaque;		// SPONZA�ǂݍ��ݗp�̃f�o�b�O�@�\


public:

	/*===== �����o�֐� =====*/

	// ���C�g���pOBJ�������b�V���ǂݍ��݊֐� BLAS�̃A�h���X��Ԃ��B
	std::vector<int> RayMultiMeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName);

private:

	// �}�e���A���t�@�C������e�N�X�`�����𔲂��o���B
	void LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName, std::vector<int>& TextureHandle);

	// string��wstring�ɕϊ��B
	std::wstring StringToWString(std::string oString);

};