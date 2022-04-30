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

	std::vector<std::pair<int, int>> blasID;	// �e�N�X�`���̃n���h���ƁA������g�p����BLAS��ID��ۑ�����R���e�i
	std::vector<Vec3> vertexData;			// BLAS�̐擪�v�f��ۑ��B
	std::vector<float> vertexFirstLength;	// BLAS�̑��v�f�Ƒ��v�f�̒��_����

	using string = std::string;


public:

	/*===== �����o�֐� =====*/

	// ���C�g���pOBJ�������b�V���ǂݍ��݊֐� BLAS�̃A�h���X��Ԃ��B
	std::vector<int> RayMultiLeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName);

private:

	// �}�e���A���t�@�C������e�N�X�`�����𔲂��o���B
	int LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName);

	// string��wstring�ɕϊ��B
	std::wstring StringToWString(std::string oString);

};