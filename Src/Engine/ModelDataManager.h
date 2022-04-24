#pragma once
#include "Struct.h"
#include <string>
#include <map>
#include "Fbxsdk.h"

class ModelDataManager {
private:
	static vector<ModelData> modelData;			//���f���f�[�^��ۑ����Ă����z��

public:
	//fbx�t�@�C�������[�h���Ēl���R�s�[����B���[�h�ς݂̃t�@�C���������烍�[�h�����ɃR�s�[�����s���B
	static void LoadFbx(const char* fileName, Object3DDeliveryData& objectBuffer);

	//obj�t�@�C�������[�h���Ēl���R�s�[����B���[�h�ς݂̃t�@�C���������烍�[�h�����ɃR�s�[�����s���B
	static void LoadObj(string directoryPath, string fileName, Object3DDeliveryData& objectBuffer, bool isSmoothing);

	//fbx�t�@�C����ǂݍ��ލہA���ׂẴm�[�h�ɃA�N�Z�X���邽�߂̊֐�
	static void CollectMeshNode(FbxNode* node, vector<pair<string, FbxMesh*>>& list);

	//obj�t�@�C���̓ǂݍ��ݎ��Ƀ}�e���A�������[�h���邽�߂̊֐�
	static void LoadObjMaterial(const string& materialFileName, ModelData& modelData, Object3DDeliveryData& spriteData);

	//�@���̕��ς����߂�֐�
	static void CalculateSmoothedVertexNormals(map<unsigned short, vector<unsigned short>>& smoothData, Object3DDeliveryData& objectBuffer, ModelData& modelData);

	static int GetModelCount() { return modelData.size(); }
	static const ModelData& GetModelData(const int& index) { return modelData[index]; }

};