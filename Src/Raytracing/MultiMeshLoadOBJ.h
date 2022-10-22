//#pragma once
//#include "Singleton.h"
//#include "Vec.h"
//#include <vector>
//#include <string>
//#include <array>
//#include <wtypes.h>
//#include <memory>
//
//class PolygonMeshInstance;
//class BLAS;
//
//// OBJ�������b�V���N���X ����SPONZA�ǂݍ��ݗp�N���X ���͎g�p���Ă��܂���B
//class MultiMeshLoadOBJ : public Singleton<MultiMeshLoadOBJ> {
//
//private:
//
//	/*===== �����o�ϐ� =====*/
//
//	std::vector<std::wstring> texturePath_;
//
//	std::vector<std::pair<std::vector<int>, std::weak_ptr<BLAS>>> blasID_;	// �e�N�X�`���̃n���h���ƁA������g�p����BLAS��ID��ۑ�����R���e�i
//
//	using string = std::string;
//
//	bool isOpaque_;		// SPONZA�ǂݍ��ݗp�̃f�o�b�O�@�\ �s�����t���O
//	bool isFloor_;		// SPONZA�ǂݍ��ݗp�̃f�o�b�O�@�\ GI�����݂̂Ɏg�p���邽�߁B
//
//
//public:
//
//	/*===== �����o�֐� =====*/
//
//	// ���C�g���pOBJ�������b�V���ǂݍ��݊֐� BLAS�̃A�h���X��Ԃ��B
//	std::vector<std::weak_ptr<PolygonMeshInstance>> RayMultiMeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName);
//
//private:
//
//	// �}�e���A���t�@�C������e�N�X�`�����𔲂��o���B
//	void LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName, std::vector<int>& TextureHandle);
//
//	// string��wstring�ɕϊ��B
//	std::wstring StringToWString(std::string oString);
//
//};