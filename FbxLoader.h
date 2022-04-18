#pragma once
#include "Singleton.h"
#include "fbxsdk.h"
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

// �m�[�h�\����
struct Node {
	// ���O
	std::string name;
	// ���[�J���X�P�[��
	DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
	// ���[�J����]�p
	DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
	// ���[�J���ړ�
	DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,1.0f };
	// ���[�J���ϊ��s��
	DirectX::XMMATRIX transform;
	// �O���[�o���ϊ��s��
	DirectX::XMMATRIX globalTransform;
	// �e�m�[�h
	Node* parent;
};

// FBX���f���N���X
class FbxModel {

public:

	// �t�����h�N���X
	friend class FbxLoader;

	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;		// ���W
		DirectX::XMFLOAT3 normal;	// �@��
		DirectX::XMFLOAT2 uv;		// UV
	};

	// ���b�V�������m�[�h
	std::weak_ptr<Node> meshNode;
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;

private:

	// ���f����
	std::string name;

	// �m�[�h�z��
	std::vector<Node> nodes;

};

// FBX�ǂݍ��݃N���X
class FbxLoader : public Singleton<FbxLoader> {

private:

	/*===== �����o�ϐ� =====*/

	std::unique_ptr<FbxManager> fbxMgr;
	std::unique_ptr<FbxImporter> fbxImporter;


private:

	using string = std::string;


public:

	/*===== �����o�֐� =====*/

	// ����������
	void Init();

	// ���[�h�֐�
	void LoadModelFromFile(const string& directryPath, const string& modelName);


private:

	// �ċA�I�Ƀm�[�h�\������́B
	void ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* parent = nullptr);

	// ���b�V���̓ǂݎ��
	void parseMesh(FbxModel& Model, FbxNode* InputFbxNode);

	// ���_���W�ǂݎ��
	void ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh);
	// �ʏ��ǂݎ��
	void ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh);
	// �}�e���A�����ǂݎ��
	void ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode);
	// �e�N�X�`�����ǂݎ��
	void ParseMeshTexture(FbxModel& Model, const std::string& filePath);

};