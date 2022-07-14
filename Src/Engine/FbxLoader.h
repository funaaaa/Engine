#pragma once
#include "Singleton.h"
#include <memory>
#include <vector>
#include <string>
#include <array>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include "Vec.h"

#pragma warning(push)
#pragma warning(disable:4023)
#pragma warning(disable:4099)
#include "fbxsdk.h"
#pragma warning(pop)

// �{�[���ԍ��ƃX�L���E�F�C�g�̃y�A
struct WeightSet {

	UINT index_;
	float weight_;

};

// �{�[���\����
struct Bone {
	// ���O
	std::string name_;
	// �����p�����W�̋t�s��
	DirectX::XMMATRIX invInitialPose_;
	// �N���X�^�[(FBX���̃{�[�����)
	FbxCluster* fbxCluster_;
	// �R���X�g���N�^
	Bone(const std::string& Name) : name_(Name) {};
};

// �m�[�h�\����
struct Node {
	// ���O
	std::string name_;
	// �p�f�B���O
	Vec2 pad_;
	// ���[�J���ϊ��s��
	DirectX::XMMATRIX transform_;
	// �O���[�o���ϊ��s��
	DirectX::XMMATRIX globalTransform_;
	// ���[�J���X�P�[��
	DirectX::XMVECTOR scaling_ = { 1.0f,1.0f,1.0f,0.0f };
	// ���[�J����]�p
	DirectX::XMVECTOR rotation_ = { 0.0f,0.0f,0.0f,0.0f };
	// ���[�J���ړ�
	DirectX::XMVECTOR translation_ = { 0.0f,0.0f,0.0f,1.0f };
	// �e�m�[�h
	Node* parent_;
};

// FBX���f���N���X
class FbxModel {

public:

	// �t�����h�N���X
	friend class FbxLoader;

	// �{�[���̍ő吔
	static const int MAX_BONE_INDICES = 4;

	// ���_�f�[�^�\����
	struct VertexPosNormalUvSkin
	{
		Vec3 pos_;		// ���W
		Vec3 normal_;	// �@��
		Vec2 uv_;		// UV
		std::array<UINT, MAX_BONE_INDICES> boneIndex_;	// �{�[���̔ԍ�
		std::array<float, MAX_BONE_INDICES> boneWeight_;	// �{�[���̏d��
	};

	// ���f����
	std::string modelName_;
	// ���b�V�������m�[�h
	Node* meshNode_;
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUvSkin> vertices_;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices_;
	// �{�[���z��
	std::vector<Bone> bones_;

	FbxTime frameTime_;		// 1�t���[���̎���
	FbxTime startTime_;		// �J�n����
	FbxTime endTime_;		// �I������
	FbxTime currentTime_;	// ���݂̎���
	bool isPlay_ = false;
	bool isInit_ = false;
	bool hasAnimation_ = false;	// �A�j���[�V�����������Ă��邩

public:

	// �A���r�G���g�W��
	Vec3 ambient_ = { 1.0f,1.0f,1.0f };
	// �f�B�t�[�Y�W��
	Vec3 diffuse_ = { 1.0f,1.0f,1.0f };
	// �e�N�X�`��ID
	int textureID_;

private:

	// �m�[�h�z��
	std::vector<Node> nodes_;


public:

	FbxModel() {
		frameTime_.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
	}

	void InitAnimation();
	void PlayAnimation();
	void StopAnimation();

};

// FBX�ǂݍ��݃N���X
class FbxLoader : public Singleton<FbxLoader> {

private:

	/*===== �����o�ϐ� =====*/

	FbxManager* fbxMgr_;
	FbxImporter* fbxImporter_;
	FbxScene* fbxScene_;

	std::string directryPath_;	// �f�B���N�g���[�p�X���ꎟ�ۑ����Ă������߂̕ϐ��B

	std::vector<FbxModel> fbxModelData_;	// �������ꂽ���f���f�[�^


public:

	// �{�[���̍ő吔
	static const int MAX_BONES = 16;
	// �X�L�j���O�p�\����(�R���s���[�g�V�F�[�_�[�ɑ���p)
	struct SkinData {
		std::array<DirectX::XMMATRIX, MAX_BONES> bones_;
	};
	// �X�L�j���O�A�j���[�V�����̍s��v�Z�Ɏg�p����R���s���[�g�V�F�[�_�[�̓��͗p�\����
	struct SkinComputeInput {
		FbxModel::VertexPosNormalUvSkin vertex_;
		FbxLoader::SkinData skinData_;
	};

	struct Vertex {
		Vec3 pos_;
		Vec3 normal_;
		Vec2 uv_;
	};
	void GetFbxData(const int& Index, std::vector<Vertex>& OutputVertex, std::vector<UINT>& OutputVertexIndex);


private:

	using string = std::string;


public:

	/*===== �����o�֐� =====*/

	// ����������
	void Init();

	// ���[�h�֐�
	int LoadModelFromFile(const string& DirectryPath, const string& ModelName);

	// FBX�̍s���XMMATRIX�ɕϊ�
	void ConvertMatrixFromFBX(DirectX::XMMATRIX& Dst, const FbxAMatrix& Src);

	// �X�L�j���O�s����擾�B
	SkinData GetSkinMat(const int& Index);

	// �X�L�j���O�A�j���[�V�����p�R���s���[�g�V�F�[�_�[�̓��͍\���̂��擾�B
	void GetSkinComputeInput(const int& Index, std::vector<SkinComputeInput>& Input);

	// FBX�V�[���̃Q�b�^
	FbxScene* GetFbxScene() { return fbxScene_; }

	// ���f�����̃Q�b�^
	FbxModel& GetFbxModel(const int& Index) { return fbxModelData_[Index]; }


private:

	// �ċA�I�Ƀm�[�h�\������́B
	void ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* Parent = nullptr);

	// ���b�V���̓ǂݎ��
	void ParseMesh(FbxModel& Model, FbxNode* InputFbxNode);

	// ���_���W�ǂݎ��
	void ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh);
	// �ʏ��ǂݎ��
	void ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh);
	// �}�e���A�����ǂݎ��
	void ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode);
	// �e�N�X�`�����ǂݎ��
	//void ParseMeshTexture(FbxModel& Model, const std::string& FilePath);
	// �X�L�j���O���ǂݎ��
	void ParseSkin(FbxModel& Model, FbxMesh* InputFbxMesh);

	// �f�B���N�g�����܂񂾃t�@�C���p�X����t�@�C�����𒊏o����B
	std::string ExtractFileName(const std::string& path);

};