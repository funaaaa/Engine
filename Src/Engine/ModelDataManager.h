#pragma once
#include <string>
#include <map>
#include <vector>
#include "Fbxsdk.h"
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

	// �f�[�^��n���\����
	struct ObjectData {

		std::vector<Vertex> vertex;
		std::vector<UINT> index;

	};

	//�}�e���A��
	struct Material {
		std::string name;								//�}�e���A����
		std::string textureName;						//�e�N�X�`���̖��O
		DirectX::XMFLOAT3 ambient = { 0.3f,0.3f,0.3f };	//�A���r�G���g�e���x
		DirectX::XMFLOAT3 diffuse = { 0.3f,0.3f,0.3f };	//�f�B�t���[�Y�e���x
		DirectX::XMFLOAT3 specular = { 0.3f,0.3f,0.3f };//�X�y�L�����[�e���x
		float alpha;									//�A���t�@
	};

	//model�̃f�[�^�\����
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
	void LoadObjMaterial(const std::string& MaterialFileName, ModelData& ModelData, ObjectData& ObjectData);

	//�@���̕��ς����߂�֐�
	void CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectBuffer, ModelData& ModelData);

	int GetModelCount() { return modelData.size(); }
	const ModelData& GetModelData(const int& Index) { return modelData[Index]; }

};