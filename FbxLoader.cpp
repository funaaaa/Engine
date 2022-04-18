#include "FbxLoader.h"
#include "DirectXBase.h"
#include "ModelDataManager.h"
#include "Struct.h"
#include <assert.h>

void FbxLoader::Init()
{

	/*===== ���������� =====*/

	// ��d�ŏ��������Ă��Ȃ������`�F�b�N�B
	assert(fbxMgr == nullptr);

	// Fbx�}�l�[�W���[�𐶐��B
	fbxMgr = std::make_unique<FbxManager>((FbxManager::Create()));

	// Fbx�}�l�[�W���[�̓��o�͐ݒ�
	std::unique_ptr<FbxIOSettings> ios = std::make_unique<FbxIOSettings>(FbxIOSettings::Create(fbxMgr.get(), IOSROOT));
	fbxMgr->SetIOSettings(ios.get());

	// Fbx�C���|�[�^�̐���
	fbxImporter = std::make_unique<FbxImporter>(FbxImporter::Create(fbxMgr.get(), ""));

}

void FbxLoader::LoadModelFromFile(const string& directryPath, const string& modelName)
{

	/*===== ���[�h�֐� =====*/

	// ���̃��f�������Ƀ��[�h����Ă�����B
	const int MODEL_COUNT = ModelDataManager::GetModelCount();
	for (int index = 0; index < MODEL_COUNT; ++index) {

		// ���O�������t�@�C������������B
		ModelData buff = ModelDataManager::GetModelData(index);
		if (buff.modelName == directryPath + modelName) {

			// ���̃f�[�^��Ԃ��B

		}

	}

	// �V�[���𐶐��B
	std::shared_ptr<FbxScene> fbxScene = std::make_unique<FbxScene>(FbxScene::Create(fbxMgr.get(), "fbxScene"));

	// �t�@�C�����烍�[�h����FBX�̏����V�[���ɃC���|�[�g�B
	fbxImporter->Import(fbxScene.get());

	// ���f���𐶐��B
	FbxModel modelData;

	// FBX�m�[�h�̐����擾�B
	int nodeCount = fbxScene->GetNodeCount();

	// �\�ߕK�v���̃��������m�ۂ��邱�ƂŁA�A�h���X�������̂�\�h�B
	modelData.nodes.reserve(nodeCount);

	// ���[�g�m�[�h���珇�ɉ�͂��ă��f���ɗ������ށB
	ParseNodeRecursive(modelData, fbxScene->GetRootNode());

}

void FbxLoader::ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* parent)
{

	/*===== �ċA�I�Ƀm�[�h����� =====*/

	// null�`�F�b�N�B
	if (InputFbxNode == nullptr) return;

	// ���f���Ƀm�[�h��ǉ��B
	Model.nodes.emplace_back();
	Node& node = Model.nodes.back();

	// �m�[�h�����擾�B
	node.name = InputFbxNode->GetName();

	// FBX�m�[�h�̃��[�J���ړ����
	FbxDouble3 rotation = InputFbxNode->LclRotation.Get();
	FbxDouble3 scaling = InputFbxNode->LclScaling.Get();
	FbxDouble3 translation = InputFbxNode->LclTranslation.Get();

	// �`���ύX���đ���B
	node.rotation = { (float)rotation[0], (float)rotation[1], (float)rotation[2], 0.0f };
	node.scaling = { (float)scaling[0], (float)scaling[1], (float)scaling[2], 0.0f };
	node.translation = { (float)translation[0], (float)translation[1], (float)translation[2], 1.0f };

	// ��]�p��Degree(�x)���烉�W�A���ɕϊ�
	node.rotation.m128_f32[0] = XMConvertToRadians(node.rotation.m128_f32[0]);
	node.rotation.m128_f32[1] = XMConvertToRadians(node.rotation.m128_f32[1]);
	node.rotation.m128_f32[2] = XMConvertToRadians(node.rotation.m128_f32[2]);

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = XMMatrixScalingFromVector(node.scaling);
	matRotation = XMMatrixScalingFromVector(node.rotation);
	matTranslation = XMMatrixScalingFromVector(node.translation);

	// ���[�J���ό`�s��̌v�Z
	node.transform = XMMatrixIdentity();
	node.transform *= matScaling;
	node.transform *= matRotation;
	node.transform *= matTranslation;

	// �O���[�o���ό`�s��̌v�Z
	node.globalTransform = node.transform;
	if (parent) {

		node.parent = parent;
		// �e�̕ό`����Z�B
		node.globalTransform *= parent->globalTransform;

	}

	// FBX�m�[�h�̃��b�V���������
	std::unique_ptr<FbxNodeAttribute> fbxNodeAttribute = std::make_unique<FbxNodeAttribute>(InputFbxNode->GetNodeAttribute());

	if (fbxNodeAttribute) {

		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {

			Model.meshNode = &node;
			parseMesh(Model, InputFbxNode);

		}

	}

	// �q�m�[�h�ɑ΂��čċA�Ăяo���B
	const int CHILD_COUNT = InputFbxNode->GetChildCount();
	for (int index = 0; index < CHILD_COUNT; ++index) {
		ParseNodeRecursive(Model, InputFbxNode->GetChild(index), &node);
	}

}

void FbxLoader::parseMesh(FbxModel& Model, FbxNode* InputFbxNode)
{

	/*===== ���b�V���̓ǂݎ�� =====*/

	// �m�[�h�̃��b�V�����擾�B
	std::unique_ptr<FbxMesh> fbxMesh = std::make_unique<FbxMesh>(InputFbxNode->GetMesh());

	// ���_���ǂݎ��B
	ParseMeshVertices(Model, fbxMesh.get());
	// �ʂ��\������f�[�^�̓ǂݎ��B
	ParseMeshFaces(Model, fbxMesh.get());
	// �}�e���A���̓ǂݎ��B
	ParseMeshMaterial(Model, InputFbxNode);

}

void FbxLoader::ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== ���_���̓ǂݎ�� =====*/

	auto& vertices = Model.vertices;

	// ���_���W�f�[�^�̐��B
	const int controlPointsCount = InputFbxMesh->GetControlPointsCount();

	// �K�v���������_�f�[�^�z����m�ہB
	FbxModel::VertexPosNormalUv vert{};
	Model.vertices.resize(controlPointsCount, vert);

	// FBX���b�V���̒��_���W�z����擾�B
	FbxVector4* pCoord = InputFbxMesh->GetControlPoints();

	// FBX���b�V���̑S���_���W�����f�����̔z��ɃR�s�[����B
	for(int index = 0; index < controlPointsCount; ++index){

		FbxModel::VertexPosNormalUv& vertex = vertices[index];

		// ���W�̃R�s�[
		vertex.pos.x = (float)pCoord[index][0];
		vertex.pos.y = (float)pCoord[index][1];
		vertex.pos.z = (float)pCoord[index][2];

	}

}

void FbxLoader::ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== �ʏ��̓ǂݎ�� =====*/

}

void FbxLoader::ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode)
{
}

void FbxLoader::ParseMeshTexture(FbxModel& Model, const std::string& filePath)
{
}
