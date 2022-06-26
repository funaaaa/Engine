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

class ModelDataManager : public Singleton<ModelDataManager> {

public:

	// ���_�f�[�^
	struct Vertex {

		Vec3 pos;
		Vec3 normal;
		DirectX::XMFLOAT2 uv;

	};

	// �}�e���A��
	struct Material {
		Vec3 ambient = { 0.3f,0.3f,0.3f };	//�A���r�G���g�e���x
		float pad1;
		Vec3 diffuse = { 0.3f,0.3f,0.3f };	//�f�B�t���[�Y�e���x
		float pad2;
		Vec3 specular = { 0.3f,0.3f,0.3f };//�X�y�L�����[�e���x
		float alpha;									//�A���t�@
	};

	// �f�[�^��n���\����
	struct ObjectData {

		std::vector<Vertex> vertex;
		std::vector<UINT> index;
		Material material;

	};

	// model�̃f�[�^�\����
	struct ModelData {
		std::string modelName;				//���f���t�@�C����
		Material material;					//�}�e���A��
		std::vector<unsigned short> index;	//���_�C���f�b�N�X
		std::vector<Vertex> vertex;			//���_
		bool isSmoothing;					//�@�����X���[�Y�ɂ��邩�ǂ���
	};

private:


	std::vector<ModelData> modelData;			//���f���f�[�^��ۑ����Ă����z��

public:

	//obj�t�@�C�������[�h���Ēl���R�s�[����B���[�h�ς݂̃t�@�C���������烍�[�h�����ɃR�s�[�����s���B
	void LoadObj(std::string DirectoryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing);

	//obj�t�@�C���̓ǂݍ��ݎ��Ƀ}�e���A�������[�h���邽�߂̊֐�
	void LoadObjMaterial(const std::string& MaterialFileName, ModelData& ModelData);

	//�@���̕��ς����߂�֐�
	void CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectBuffer, ModelData& ModelData);

	int GetModelCount() { return static_cast<int>(modelData.size()); }
	const ModelData& GetModelData(const int& Index) { return modelData[Index]; }

};