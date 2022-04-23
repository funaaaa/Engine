#include "FbxLoader.h"
#include "DirectXBase.h"
#include "Struct.h"
#include <assert.h>
#include "TextureManager.h"

void FbxLoader::Init()
{

	/*===== ���������� =====*/

	// ��d�ŏ��������Ă��Ȃ������`�F�b�N�B
	assert(fbxMgr == nullptr);

	// Fbx�}�l�[�W���[�𐶐��B
	fbxMgr = FbxManager::Create();

	// Fbx�}�l�[�W���[�̓��o�͐ݒ�
	FbxIOSettings* ios = FbxIOSettings::Create(fbxMgr, IOSROOT);
	fbxMgr->SetIOSettings(ios);

	// Fbx�C���|�[�^�̐���
	fbxImporter = FbxImporter::Create(fbxMgr, "");

	// �V�[���𐶐��B
	fbxScene = FbxScene::Create(fbxMgr, "fbxScene");

}

FbxModel FbxLoader::LoadModelFromFile(const string& DirectryPath, const string& ModelName)
{

	/*===== ���[�h�֐� =====*/

	// ���̃��f�������Ƀ��[�h����Ă�����B
	const int MODEL_COUNT = fbxModelData.size();
	for (int index = 0; index < MODEL_COUNT; ++index) {

		// ���O�������t�@�C������������B
		if (fbxModelData[index].modelName == DirectryPath + ModelName) {

			// ���̃f�[�^��Ԃ��B
			return fbxModelData[index];

		}

	}

	// �f�B���N�g���[�p�X��ۑ��B
	this->directryPath = DirectryPath;

	// �t�@�C�������w�肵��FBX�t�@�C����ǂݍ��ށB
	if (!fbxImporter->Initialize((DirectryPath + ModelName).c_str(), -1, fbxMgr->GetIOSettings())) {
		assert(0);
	}

	// �t�@�C�����烍�[�h����FBX�̏����V�[���ɃC���|�[�g�B
	fbxImporter->Import(fbxScene);

	// ���f���𐶐��B
	FbxModel modelData;

	// FBX�m�[�h�̐����擾�B
	int nodeCount = fbxScene->GetNodeCount();

	// �\�ߕK�v���̃��������m�ۂ��邱�ƂŁA�A�h���X�������̂�\�h�B
	modelData.nodes.reserve(nodeCount);

	// ���[�g�m�[�h���珇�ɉ�͂��ă��f���ɗ������ށB
	ParseNodeRecursive(modelData, fbxScene->GetRootNode());

	return modelData;

}

void FbxLoader::ConvertMatrixFromFBX(DirectX::XMMATRIX& Dst, const FbxAMatrix& Src)
{

	/*===== FBX�̍s���XMMATRIX�ɕϊ� =====*/

	// �s
	for (int row = 0; row < 4; ++row) {

		// ��
		for (int str = 0; str < 4; ++str) {

			Dst.r[row].m128_f32[str] = (float)Src.Get(row, str);

		}

	}

}

Object3DDeliveryData FbxLoader::ConvertObject3DDeliveryData(const FbxModel& ModelData)
{
	Object3DDeliveryData returnData;

	// ���_�f�[�^��ϊ��B
	const int VERTEX_COUNT = ModelData.vertices.size();
	returnData.vertex.resize(VERTEX_COUNT);
	for (int index = 0; index < VERTEX_COUNT; ++index) {
		returnData.vertex[index].pos = ModelData.vertices[index].pos;
		returnData.vertex[index].normal = ModelData.vertices[index].normal;
		returnData.vertex[index].uv = ModelData.vertices[index].uv;
	}

	// ���_�C���f�b�N�X�f�[�^��ϊ��B
	const int INDEX_COUNT = ModelData.indices.size();
	returnData.index.resize(INDEX_COUNT);
	for (int index = 0; index < INDEX_COUNT; ++index) {
		returnData.index[index] = ModelData.indices[index];
	}

	// ���C�e�B���O����̒萔�o�b�t�@�f�[�^��ϊ��B
	returnData.constBufferDataB1.alpha = 1.0f;
	returnData.constBufferDataB1.ambient = ModelData.ambient;
	returnData.constBufferDataB1.diffuse = ModelData.diffuse;
	returnData.constBufferDataB1.specular = {};

	return returnData;
}

void FbxLoader::ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* Parent)
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
	if (Parent) {

		node.parent = Parent;
		// �e�̕ό`����Z�B
		node.globalTransform *= Parent->globalTransform;

	}

	// FBX�m�[�h�̃��b�V���������
	FbxNodeAttribute* fbxNodeAttribute = InputFbxNode->GetNodeAttribute();

	if (fbxNodeAttribute) {

		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {

			Model.meshNode = &node;
			ParseMesh(Model, InputFbxNode);

		}

	}

	// �q�m�[�h�ɑ΂��čċA�Ăяo���B
	const int CHILD_COUNT = InputFbxNode->GetChildCount();
	for (int index = 0; index < CHILD_COUNT; ++index) {
		ParseNodeRecursive(Model, InputFbxNode->GetChild(index), &node);
	}

}

void FbxLoader::ParseMesh(FbxModel& Model, FbxNode* InputFbxNode)
{

	/*===== ���b�V���̓ǂݎ�� =====*/

	// �m�[�h�̃��b�V�����擾�B
	FbxMesh* fbxMesh = InputFbxNode->GetMesh();

	// ���_���ǂݎ��B
	ParseMeshVertices(Model, fbxMesh);
	// �ʂ��\������f�[�^�̓ǂݎ��B
	ParseMeshFaces(Model, fbxMesh);
	// �}�e���A���̓ǂݎ��B
	ParseMeshMaterial(Model, InputFbxNode);
	// �X�L�j���O���̓ǂݎ��B
	ParseSkin(Model, fbxMesh);

}

void FbxLoader::ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== ���_���̓ǂݎ�� =====*/

	auto& vertices = Model.vertices;

	// ���_���W�f�[�^�̐��B
	const int controlPointsCount = InputFbxMesh->GetControlPointsCount();

	// �K�v���������_�f�[�^�z����m�ہB
	FbxModel::VertexPosNormalUvSkin vert{};
	Model.vertices.resize(controlPointsCount, vert);

	// FBX���b�V���̒��_���W�z����擾�B
	FbxVector4* pCoord = InputFbxMesh->GetControlPoints();

	// FBX���b�V���̑S���_���W�����f�����̔z��ɃR�s�[����B
	for (int index = 0; index < controlPointsCount; ++index) {

		FbxModel::VertexPosNormalUvSkin& vertex = vertices[index];

		// ���W�̃R�s�[
		vertex.pos.x = (float)pCoord[index][0];
		vertex.pos.y = (float)pCoord[index][1];
		vertex.pos.z = (float)pCoord[index][2];

	}

}

void FbxLoader::ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== �ʏ��̓ǂݎ�� =====*/

	auto& vertices = Model.vertices;
	auto& indices = Model.indices;

	// 1�t�@�C���ɕ������b�V���̃��f���͔�Ή�
	assert(indices.size() == 0);

	// �ʂ̐�
	const int polygonCount = InputFbxMesh->GetPolygonCount();

	// UV�f�[�^�̐�
	const int textureUVCount = InputFbxMesh->GetTextureUVCount();

	// UV�e���X�g
	FbxStringList uvNames;
	InputFbxMesh->GetUVSetNames(uvNames);

	// �ʂ��Ƃ̏��̓ǂݎ��
	for (int index = 0; index < polygonCount; ++index) {

		// �ʂ��\�����钸�_�̐����擾(3�Ȃ�O�p�`�|���S��)
		const int polygonSize = InputFbxMesh->GetPolygonSize(index);
		assert(polygonSize <= 4);

		// 1���_������
		for (int porygonIndex = 0; porygonIndex < polygonSize; ++porygonIndex) {

			// FBX���_�z��̃C���f�b�N�X
			int fbxIndex = InputFbxMesh->GetPolygonVertex(index, porygonIndex);
			assert(fbxIndex >= 0);

			// �@�����ǂݍ���
			FbxModel::VertexPosNormalUvSkin& vertex = vertices[fbxIndex];
			FbxVector4 normal;
			if (InputFbxMesh->GetPolygonVertexNormal(index, porygonIndex, normal)) {
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}

			// �e�N�X�`��UV�ǂݍ���
			if (0 < textureUVCount) {
				FbxVector2 uvs;
				bool lUnmappedUV;
				// 0�Ō��ߑł��œǂݍ���
				if (InputFbxMesh->GetPolygonVertexUV(index, porygonIndex, uvNames[0], uvs, lUnmappedUV)) {
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			// �C���f�b�N�X�z��ɒ��_�C���f�b�N�X��ǉ��B

			// 3���_�܂�
			if (porygonIndex < 3) {

				// 1�_�ǉ����A����2���_�ƎO�p�`���\������B
				indices.emplace_back(fbxIndex);

			}
			// 4���_��
			else {

				// 3�_�ǉ����A�l�p�`��0,1,2,3�̓��A2,3,0�ŎO�p�`���\������B
				int index2 = indices[indices.size() - 1];
				int index3 = fbxIndex;
				int index0 = indices[indices.size() - 3];
				indices.emplace_back(index2);
				indices.emplace_back(index3);
				indices.emplace_back(index0);

			}

		}

	}

}

void FbxLoader::ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode)
{

	/*===== �}�e���A�����ǂݎ�� =====*/

	const int materialCount = InputFbxNode->GetMaterialCount();
	if (0 < materialCount) {

		// �擪�̃}�e���A�����擾�B
		FbxSurfaceMaterial* material = InputFbxNode->GetMaterial(0);

		// �e�N�X�`����ǂݍ��񂾂��ǂ�����\���t���O�B
		bool textureLoaded = false;

		if (material) {

			// FbxSurfaceLambert�N���X���ǂ����𒲂ׂ�B
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {

				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				// �����W��
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				Model.ambient.x = (float)ambient.Get()[0];
				Model.ambient.y = (float)ambient.Get()[1];
				Model.ambient.z = (float)ambient.Get()[2];

				// �g�U���ˌ��W��
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				Model.diffuse.x = (float)diffuse.Get()[0];
				Model.diffuse.y = (float)diffuse.Get()[1];
				Model.diffuse.z = (float)diffuse.Get()[2];

			}

			// �f�B�t�[�Y�e�N�X�`���𒣂�o���B
			const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid()) {

				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture) {

					const char* filePath = texture->GetFileName();
					// �t�@�C���p�X����t�@�C�������o�B
					string path_str(filePath);
					string name = ExtractFileName(path_str);

					// �p�X��ϊ��B
					wchar_t wFilePath[128];
					string fullPath = directryPath + name;
					MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath, _countof(wFilePath));

					// �e�N�X�`���ǂݍ��݁B
					Model.textureID = TextureManager::Instance()->LoadTexture(wFilePath);

					textureLoaded = true;

				}

			}

		}

		// �e�N�X�`�����Ȃ��ꍇ�͔��e�N�X�`����\��B
		if (!textureLoaded) {

			Model.textureID = TextureManager::Instance()->CreateTexture({ 1.0f,1.0f,1.0f,1.0f });

		}

	}

}

void FbxLoader::ParseMeshTexture(FbxModel& Model, const std::string& filePath)
{
}

void FbxLoader::ParseSkin(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== �X�L�j���O���ǂݎ�� =====*/

	FbxSkin* fbxSkin = static_cast<FbxSkin*>(InputFbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	// �X�L�j���O��񂪂Ȃ���ΏI���B
	if (fbxSkin == nullptr) {
		return;
	}

	// �{�[���z��̎Q�ƁB
	std::vector<Bone>& bones = Model.bones;

	// �{�[���̐��B
	const int CLUSTER_COUNT = fbxSkin->GetClusterCount();
	bones.reserve(CLUSTER_COUNT);

	// �S�Ẵ{�[���ɂ��āB
	for (int index = 0; index < CLUSTER_COUNT; ++index) {

		// FBX�{�[�����B
		FbxCluster* fbxCluster = fbxSkin->GetCluster(index);

		// �{�[�����̂̃m�[�h�̖��O���擾�B
		const char* boneName = fbxCluster->GetLink()->GetName();

		// �V�����{�[����ǉ����A�ǉ������{�[���̎Q�Ƃ𓾂�B
		bones.emplace_back(Bone(boneName));
		Bone& bone = bones.back();

		// ����{�[����FBX�̃{�[����R�Â���B
		bone.fbxCluster = fbxCluster;

		// FBX���珉���p���s����擾����B
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		// XMMatrix�^�ɕϊ�����B
		XMMATRIX initialPose;
		ConvertMatrixFromFBX(initialPose, fbxMat);

		// �����p���s��̋t�s��𓾂�B
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);

	}

	// �񎟌��z��(�W���O�z��)
	// list   : ���_���e�����󂯂�{�[���̑S���X�g
	// vector : �����S���_��
	std::vector<std::list<WeightSet>> weightLists(Model.vertices.size());

	// �S�Ẵ{�[���ɂ��āB
	for (int index = 0; index < CLUSTER_COUNT; ++index) {

		// FBX�{�[�����B
		FbxCluster* fbxCluster = fbxSkin->GetCluster(index);

		// ���̃{�[���ɉe�����󂯂钸�_�̐��B
		int CONTROL_POINT_INDICES_COUNT = fbxCluster->GetControlPointIndicesCount();

		// ���̃{�[���ɉe�����󂯂钸�_�̐��B
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();

		// �e�����󂯂�S���_�ɂ��āB
		for (int vertexIndex = 0; vertexIndex < CONTROL_POINT_INDICES_COUNT; ++vertexIndex) {

			// ���_�ԍ�
			int vertIndex = controlPointIndices[vertexIndex];
			// �X�L���E�F�C�g
			float weight = (float)controlPointWeights[vertexIndex];
			// ���̒��_�̉e�����󂯂�{�[�����X�g�ɁA�{�[���ƃE�F�C�g�̃y�A��ǉ��B
			weightLists[vertIndex].emplace_back(WeightSet({ (UINT)index, (weight) }));

		}

	}

	// ���_�z�񏑂������p�̎Q��
	auto& vertices = Model.vertices;

	// �e���_�ɂ��ď���
	const int VERTICES_SIZE = vertices.size();
	for (int index = 0; index < VERTICES_SIZE; ++index) {

		// ���_�̃E�F�C�g����ł��傫��4��I���B
		auto& weightList = weightLists[index];

		// �召��r�p�̃����_�����Z�b�g���č~���Ƀ\�[�g�B
		weightList.sort([](auto const& lhs, auto const& rhs) {
			// �Ђ���̗v�f�̂ق����傫�����true �����łȂ����false��Ԃ��B
			return rhs.weight < lhs.weight;
			});

		int weightArrayIndex = 0;

		// �~���\�[�g�ς݂̃E�F�C�g���X�g����B
		for (auto& weightSet : weightList) {

			// ���_�f�[�^�ɏ������݁B
			vertices[index].boneIndex[weightArrayIndex] = weightSet.index;
			vertices[index].boneWeight[weightArrayIndex] = weightSet.weight;

			// 4�ɒB������I���B
			if (FbxModel::MAX_BONE_INDICES <= ++weightArrayIndex) {

				float weight = 0.0f;

				// 2�Ԗڈȍ~�̃E�F�C�g�����v�B
				for (int boneIndex = 0; boneIndex < FbxModel::MAX_BONE_INDICES; ++boneIndex) {

					weight += vertices[index].boneWeight[boneIndex];

				}

				// ���v��1.0f(100%)�ɂȂ�p�ɒ����B
				vertices[index].boneWeight[0] = 1.0f - weight;
				break;

			}

		}

	}

}

std::string FbxLoader::ExtractFileName(const std::string& path)
{

	/*===== �f�B���N�g�����܂񂾃t�@�C���p�X����t�@�C�����𒊏o =====*/

	size_t pos1;

	// ��؂蕶�� '\\' ���o�Ă����ԍŌ�̕����������B
	pos1 = path.rfind('\\');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	// ��؂蕶��'/'���o�Ă����ԍŌ�̕������Q��
	pos1 = path.rfind('/');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}