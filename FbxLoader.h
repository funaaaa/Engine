#pragma once
#include "Singleton.h"
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include "fbxsdk.h"
#include "Struct.h"

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

	// ���f����
	std::string modelName;
	// ���b�V�������m�[�h
	Node* meshNode;
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;

public:

	// �A���r�G���g�W��
	DirectX::XMFLOAT3 ambient = { 1.0f,1.0f,1.0f };
	// �f�B�t�[�Y�W��
	DirectX::XMFLOAT3 diffuse = { 1.0f,1.0f,1.0f };
	// �e�N�X�`��ID
	int textureID;

private:

	// �m�[�h�z��
	std::vector<Node> nodes;

};

// FBX�ǂݍ��݃N���X
class FbxLoader : public Singleton<FbxLoader> {

private:

	/*===== �����o�ϐ� =====*/

	FbxManager* fbxMgr;
	FbxImporter* fbxImporter;

	std::string directryPath;	// �f�B���N�g���[�p�X���ꎟ�ۑ����Ă������߂̕ϐ��B

	std::vector<FbxModel> fbxModelData;	// �������ꂽ���f���f�[�^


private:

	using string = std::string;


public:

	/*===== �����o�֐� =====*/

	// ����������
	void Init();

	// ���[�h�֐�
	FbxModel LoadModelFromFile(const string& directryPath, const string& modelName);

	// FbxModel��Object3DDeliveryData�ɕϊ�
	Object3DDeliveryData ConvertObject3DDeliveryData(const FbxModel& modelData);


private:

	// �ċA�I�Ƀm�[�h�\������́B
	void ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* parent = nullptr);

	// ���b�V���̓ǂݎ��
	void ParseMesh(FbxModel& Model, FbxNode* InputFbxNode);

	// ���_���W�ǂݎ��
	void ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh);
	// �ʏ��ǂݎ��
	void ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh);
	// �}�e���A�����ǂݎ��
	void ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode);
	// �e�N�X�`�����ǂݎ��
	void ParseMeshTexture(FbxModel& Model, const std::string& filePath);

	// �f�B���N�g�����܂񂾃t�@�C���p�X����t�@�C�����𒊏o����B
	std::string ExtractFileName(const std::string& path);

};