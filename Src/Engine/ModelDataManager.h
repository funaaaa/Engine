#pragma once
#include <string>
#include <map>
#include <vector>
#pragma warning(push)
#pragma warning(disable:4023)
#pragma warning(disable:4099)
#include "Fbxsdk.h"
#pragma warning(pop)
#include "Vec.h"
#include <wtypes.h>
#include "Singleton.h"
#include "GLTF.h"

class ModelDataManager : public Singleton<ModelDataManager> {

public:

	// ���_�f�[�^
	struct Vertex {

		Vec3 pos_;
		Vec3 normal_;
		Vec2 uv_;

	};

	// �}�e���A��
	struct Material {
		Vec3 baseColor_;
		float metalness_;
		float specular_;
		float roughness_;
		int textureHandle_;
		int mapParam_;		// �}�e���A���̒ǉ����B���C�g���ł͒ǉ��̃e�N�X�`�����ǂ��g�p���邩�Ŏg�p����B��:�@���}�b�v��X�y�L�����}�b�v���B
		int pad_;
		Material() :baseColor_(Vec3(1, 1, 1)), metalness_(0.1f), specular_(0.1f), roughness_(0.0f), textureHandle_(-1), mapParam_(0) {};
	};

	// �f�[�^��n���\����
	struct ObjectData {

		std::vector<Vertex> vertex_;
		std::vector<UINT> index_;
		Material material_;
		Vec3 vertexMax_;	// ���_�ɂ�����e���̍ő�̈ʒu
		Vec3 vertexMin_;	// ���_�ɂ�����e���̍ŏ��̈ʒu
		int textureHandle_;

	};

	// model�̃f�[�^�\����
	struct ModelData {
		std::string modelName_;				//���f���t�@�C����
		Material material_;					//�}�e���A��
		std::vector<UINT> index_;	//���_�C���f�b�N�X
		std::vector<Vertex> vertex_;			//���_
		Vec3 vertexMax_;	// ���_�ɂ�����e���̍ő�̈ʒu
		Vec3 vertexMin_;	// ���_�ɂ�����e���̍ŏ��̈ʒu
		bool isSmoothing_;					//�@�����X���[�Y�ɂ��邩�ǂ���
	};

private:


	std::vector<ModelData> modelData_;			// ���f���f�[�^��ۑ����Ă����z��
	std::vector<std::wstring> filePath_;		// �t�@�C���p�X�ۑ��p
	std::vector<GLTF> gltf_;

public:

	// obj�t�@�C�������[�h���Ēl���R�s�[����B���[�h�ς݂̃t�@�C���������烍�[�h�����ɃR�s�[�����s���B
	void LoadObj(std::string DirectoryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing);
	void LoadGLTF(std::wstring Path, ObjectData& ObjectBuffer);

	// obj�t�@�C���̓ǂݍ��ݎ��Ƀ}�e���A�������[�h���邽�߂̊֐�
	void LoadObjMaterial(std::string DirectoryPath, const std::string& MaterialFileName, ModelData& ModelData, ObjectData& ObjectBuffer);

	// �@���̕��ς����߂�֐�
	void CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectBuffer, ModelData& ModelData);

	int GetModelCount() { return static_cast<int>(modelData_.size()); }
	const ModelData& GetModelData(int Index) { return modelData_[Index]; }

	// �ő�ŏ��̒��_��ۑ��B
	void SaveVertexMinMaxInfo(ObjectData& ObjectBuffer, const Vec3& Pos);

};