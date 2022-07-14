#include "FbxLoader.h"
#include "DirectXBase.h"
#include <assert.h>
#include "TextureManager.h"

void FbxLoader::GetFbxData(const int& Index, std::vector<Vertex>& OutputVertex, std::vector<UINT>& OutputVertexIndex)
{

	// �w�肳�ꂽ�C���f�b�N�X���͈͊O�������牽���Ԃ��Ȃ��B
	if (fbxModelData_.size() - 1 < Index || Index < 0) return;

	// �w�肳�ꂽ�C���f�b�N�X���擾�B
	FbxModel modelData_ = fbxModelData_[Index];

	// ���_�f�[�^��ϊ��B
	const int VERTEX_COUNT = static_cast<int>(modelData_.vertices_.size());
	OutputVertex.resize(VERTEX_COUNT);
	for (int index_ = 0; index_ < VERTEX_COUNT; ++index_) {
		OutputVertex[index_].pos_ = modelData_.vertices_[index_].pos_;
		OutputVertex[index_].normal_ = modelData_.vertices_[index_].normal_;
		OutputVertex[index_].uv_ = modelData_.vertices_[index_].uv_;
	}

	// ���_�C���f�b�N�X�f�[�^��ϊ��B
	const int INDEX_COUNT = static_cast<int>(modelData_.indices_.size());
	OutputVertexIndex.resize(INDEX_COUNT);
	for (int index_ = 0; index_ < INDEX_COUNT; ++index_) {
		OutputVertexIndex[index_] = modelData_.indices_[index_];
	}

}

void FbxLoader::Init()
{

	/*===== ���������� =====*/

	// ��d�ŏ��������Ă��Ȃ������`�F�b�N�B
	assert(fbxMgr_ == nullptr);

	// Fbx�}�l�[�W���[�𐶐��B
	fbxMgr_ = FbxManager::Create();

	// Fbx�}�l�[�W���[�̓��o�͐ݒ�
	FbxIOSettings* ios = FbxIOSettings::Create(fbxMgr_, IOSROOT);
	fbxMgr_->SetIOSettings(ios);

	// Fbx�C���|�[�^�̐���
	fbxImporter_ = FbxImporter::Create(fbxMgr_, "");

	// �V�[���𐶐��B
	fbxScene_ = FbxScene::Create(fbxMgr_, "fbxScene");

}

int FbxLoader::LoadModelFromFile(const string& DirectryPath, const string& ModelName)
{

	/*===== ���[�h�֐� =====*/

	// ���̃��f�������Ƀ��[�h����Ă�����B
	const int MODEL_COUNT = static_cast<int>(fbxModelData_.size());
	for (int index_ = 0; index_ < MODEL_COUNT; ++index_) {

		// ���O�������t�@�C������������B
		if (fbxModelData_[index_].modelName_ == DirectryPath + ModelName) {

			// ���̃f�[�^��Ԃ��B
			return index_;

		}

	}

	// �f�B���N�g���[�p�X��ۑ��B
	this->directryPath_ = DirectryPath;

	// �t�@�C�������w�肵��FBX�t�@�C����ǂݍ��ށB
	if (!fbxImporter_->Initialize((DirectryPath + ModelName).c_str(), -1, fbxMgr_->GetIOSettings())) {
		assert(0);
	}

	// �t�@�C�����烍�[�h����FBX�̏����V�[���ɃC���|�[�g�B
	fbxImporter_->Import(fbxScene_);

	// ���f���𐶐��B
	FbxModel modelData_;

	// FBX�m�[�h�̐����擾�B
	int nodeCount = fbxScene_->GetNodeCount();

	// �\�ߕK�v���̃��������m�ۂ��邱�ƂŁA�A�h���X�������̂�\�h�B
	modelData_.nodes_.reserve(nodeCount);

	// ���[�g�m�[�h���珇�ɉ�͂��ă��f���ɗ������ށB
	ParseNodeRecursive(modelData_, fbxScene_->GetRootNode());

	// �f�[�^��ۑ��B
	fbxModelData_.emplace_back(modelData_);

	return static_cast<int>(fbxModelData_.size()) - 1;

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

FbxLoader::SkinData FbxLoader::GetSkinMat(const int& Index)
{

	/*===== �X�L�j���O�s����擾 =====*/

	// �w�肳�ꂽ�C���f�b�N�X���͈͊O�������牽���Ԃ��Ȃ��B
	if (static_cast<int>(fbxModelData_.size()) - 1 < Index || Index < 0) return {};

	// �{�[���z��
	std::vector<Bone>& bones_ = fbxModelData_[Index].bones_;

	SkinData returnData = {};

	const int BONE_SIZE = static_cast<int>(bones_.size());
	for (int index_ = 0; index_ < BONE_SIZE; ++index_) {

		if (16 <= index_)break;

		// ���̎p���s��
		DirectX::XMMATRIX matCurrentPose;

		// ���̎p���s����擾�B
		FbxAMatrix fbxCurrentPose = bones_[index_].fbxCluster_->GetLink()->EvaluateGlobalTransform(fbxModelData_[Index].currentTime_);

		// XMMATRIX�ɕϊ��B
		ConvertMatrixFromFBX(matCurrentPose, fbxCurrentPose);

		// �������ăX�L�j���O�s��ɂ���B
		returnData.bones_[index_] = bones_[index_].invInitialPose_ * matCurrentPose;

	}

	return returnData;

}

void FbxLoader::GetSkinComputeInput(const int& Index, std::vector<SkinComputeInput>& Input)
{

	/*===== �X�L�j���O�A�j���[�V�����p�R���s���[�g�V�F�[�_�[�̓��͍\���̂��擾 =====*/

	// �w�肳�ꂽ�C���f�b�N�X���͈͊O�������牽���Ԃ��Ȃ��B
	if (static_cast<int>(fbxModelData_.size()) - 1 < Index || Index < 0) return;

	FbxModel indexModel = fbxModelData_[Index];
	SkinData skinData_ = GetSkinMat(Index);

	const int VERTEX_COUNT = static_cast<int>(indexModel.vertices_.size());
	for (int index_ = 0; index_ < VERTEX_COUNT; ++index_) {

		Input[index_].vertex_ = indexModel.vertices_[index_];
		Input[index_].skinData_ = skinData_;

	}

}

void FbxLoader::ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* Parent)
{

	/*===== �ċA�I�Ƀm�[�h����� =====*/

	// null�`�F�b�N�B
	if (InputFbxNode == nullptr) return;

	// ���f���Ƀm�[�h��ǉ��B
	Model.nodes_.emplace_back();
	Node& node = Model.nodes_.back();

	// �m�[�h�����擾�B
	node.name_ = InputFbxNode->GetName();

	// FBX�m�[�h�̃��[�J���ړ����
	FbxDouble3 rotation_ = InputFbxNode->LclRotation.Get();
	FbxDouble3 scaling_ = InputFbxNode->LclScaling.Get();
	FbxDouble3 translation_ = InputFbxNode->LclTranslation.Get();

	// �`���ύX���đ���B
	node.rotation_ = { (float)rotation_[0], (float)rotation_[1], (float)rotation_[2], 0.0f };
	node.scaling_ = { (float)scaling_[0], (float)scaling_[1], (float)scaling_[2], 0.0f };
	node.translation_ = { (float)translation_[0], (float)translation_[1], (float)translation_[2], 1.0f };

	// ��]�p��Degree(�x)���烉�W�A���ɕϊ�
	node.rotation_.m128_f32[0] = DirectX::XMConvertToRadians(node.rotation_.m128_f32[0]);
	node.rotation_.m128_f32[1] = DirectX::XMConvertToRadians(node.rotation_.m128_f32[1]);
	node.rotation_.m128_f32[2] = DirectX::XMConvertToRadians(node.rotation_.m128_f32[2]);

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	DirectX::XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = DirectX::XMMatrixScalingFromVector(node.scaling_);
	matRotation = DirectX::XMMatrixScalingFromVector(node.rotation_);
	matTranslation = DirectX::XMMatrixScalingFromVector(node.translation_);

	// ���[�J���ό`�s��̌v�Z
	node.transform_ = DirectX::XMMatrixIdentity();
	node.transform_ *= matScaling;
	node.transform_ *= matRotation;
	node.transform_ *= matTranslation;

	// �O���[�o���ό`�s��̌v�Z
	node.globalTransform_ = node.transform_;
	if (Parent) {

		node.parent_ = Parent;
		// �e�̕ό`����Z�B
		node.globalTransform_ *= Parent->globalTransform_;

	}

	// FBX�m�[�h�̃��b�V���������
	FbxNodeAttribute* fbxNodeAttribute = InputFbxNode->GetNodeAttribute();

	if (fbxNodeAttribute) {

		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {

			Model.meshNode_ = &node;
			ParseMesh(Model, InputFbxNode);

		}

	}

	// �q�m�[�h�ɑ΂��čċA�Ăяo���B
	const int CHILD_COUNT = InputFbxNode->GetChildCount();
	for (int index_ = 0; index_ < CHILD_COUNT; ++index_) {
		ParseNodeRecursive(Model, InputFbxNode->GetChild(index_), &node);
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

	auto& vertices_ = Model.vertices_;

	// ���_���W�f�[�^�̐��B
	const int controlPointsCount = InputFbxMesh->GetControlPointsCount();

	// �K�v���������_�f�[�^�z����m�ہB
	FbxModel::VertexPosNormalUvSkin vert{};
	Model.vertices_.resize(controlPointsCount, vert);

	// FBX���b�V���̒��_���W�z����擾�B
	FbxVector4* pCoord = InputFbxMesh->GetControlPoints();

	// FBX���b�V���̑S���_���W�����f�����̔z��ɃR�s�[����B
	for (int index_ = 0; index_ < controlPointsCount; ++index_) {

		FbxModel::VertexPosNormalUvSkin& vertex_ = vertices_[index_];

		// ���W�̃R�s�[
		vertex_.pos_.x_ = (float)pCoord[index_][0];
		vertex_.pos_.y_ = (float)pCoord[index_][1];
		vertex_.pos_.z_ = (float)pCoord[index_][2];

	}

}

void FbxLoader::ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== �ʏ��̓ǂݎ�� =====*/

	auto& vertices_ = Model.vertices_;
	auto& indices_ = Model.indices_;

	// 1�t�@�C���ɕ������b�V���̃��f���͔�Ή�
	assert(indices_.size() == 0);

	// �ʂ̐�
	const int polygonCount = InputFbxMesh->GetPolygonCount();

	// UV�f�[�^�̐�
	const int textureUVCount = InputFbxMesh->GetTextureUVCount();

	// UV�e���X�g
	FbxStringList uvNames;
	InputFbxMesh->GetUVSetNames(uvNames);

	// �ʂ��Ƃ̏��̓ǂݎ��
	for (int index_ = 0; index_ < polygonCount; ++index_) {

		// �ʂ��\�����钸�_�̐����擾(3�Ȃ�O�p�`�|���S��)
		const int polygonSize = InputFbxMesh->GetPolygonSize(index_);
		assert(polygonSize <= 4);

		// 1���_������
		for (int porygonIndex = 0; porygonIndex < polygonSize; ++porygonIndex) {

			// FBX���_�z��̃C���f�b�N�X
			int fbxIndex = InputFbxMesh->GetPolygonVertex(index_, porygonIndex);
			assert(0 <= fbxIndex);

			// �@�����ǂݍ���
			FbxModel::VertexPosNormalUvSkin& vertex_ = vertices_[fbxIndex];
			FbxVector4 normal_;
			if (InputFbxMesh->GetPolygonVertexNormal(index_, porygonIndex, normal_)) {
				vertex_.normal_.x_ = static_cast<float>(normal_[0]);
				vertex_.normal_.y_ = static_cast<float>(normal_[1]);
				vertex_.normal_.z_ = static_cast<float>(normal_[2]);
			}

			// �e�N�X�`��UV�ǂݍ���
			if (0 < textureUVCount) {
				FbxVector2 uvs;
				bool lUnmappedUV;
				// 0�Ō��ߑł��œǂݍ���
				if (InputFbxMesh->GetPolygonVertexUV(index_, porygonIndex, uvNames[0], uvs, lUnmappedUV)) {
					vertex_.uv_.x_ = static_cast<float>(uvs[0]);
					vertex_.uv_.y_ = static_cast<float>(uvs[1]);
				}
			}

			// �C���f�b�N�X�z��ɒ��_�C���f�b�N�X��ǉ��B

			// 3���_�܂�
			if (porygonIndex < 3) {

				// 1�_�ǉ����A����2���_�ƎO�p�`���\������B
				indices_.emplace_back(static_cast<unsigned short>(fbxIndex));

			}
			// 4���_��
			else {

				// 3�_�ǉ����A�l�p�`��0,1,2,3�̓��A2,3,0�ŎO�p�`���\������B
				int index2 = indices_[indices_.size() - 1];
				int index3 = fbxIndex;
				int index0 = indices_[indices_.size() - 3];
				indices_.emplace_back(static_cast<unsigned short>(index2));
				indices_.emplace_back(static_cast<unsigned short>(index3));
				indices_.emplace_back(static_cast<unsigned short>(index0));

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
		FbxSurfaceMaterial* material_ = InputFbxNode->GetMaterial(0);

		// �e�N�X�`����ǂݍ��񂾂��ǂ�����\���t���O�B
		bool textureLoaded = false;

		if (material_) {

			// FbxSurfaceLambert�N���X���ǂ����𒲂ׂ�B
			if (material_->GetClassId().Is(FbxSurfaceLambert::ClassId)) {

				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material_);

				// �����W��
				FbxPropertyT<FbxDouble3> ambient_ = lambert->Ambient;
				Model.ambient_.x_ = static_cast<float>(ambient_.Get()[0]);
				Model.ambient_.y_ = static_cast<float>(ambient_.Get()[1]);
				Model.ambient_.z_ = static_cast<float>(ambient_.Get()[2]);

				// �g�U���ˌ��W��
				FbxPropertyT<FbxDouble3> diffuse_ = lambert->Diffuse;
				Model.diffuse_.x_ = static_cast<float>(diffuse_.Get()[0]);
				Model.diffuse_.y_ = static_cast<float>(diffuse_.Get()[1]);
				Model.diffuse_.z_ = static_cast<float>(diffuse_.Get()[2]);

			}

			// �f�B�t�[�Y�e�N�X�`���𒣂�o���B
			const FbxProperty diffuseProperty = material_->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid()) {

				const FbxFileTexture* texture_ = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture_) {

					const char* filePath = texture_->GetFileName();
					// �t�@�C���p�X����t�@�C�������o�B
					string path_str(filePath);
					string name_ = ExtractFileName(path_str);

					// �p�X��ϊ��B
					std::array<wchar_t, 128> wFilePath;
					string fullPath = directryPath_ + name_;
					MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath.data(), static_cast<int>(wFilePath.size()));

					// �e�N�X�`���ǂݍ��݁B
					Model.textureID_ = TextureManager::Ins()->LoadTexture(wFilePath.data());

					textureLoaded = true;

				}

			}

		}

		// �e�N�X�`�����Ȃ��ꍇ�͔��e�N�X�`����\��B
		if (!textureLoaded) {

			Model.textureID_ = TextureManager::Ins()->CreateTexture({ 1.0f,1.0f,1.0f,1.0f });

		}

	}

}

//void FbxLoader::ParseMeshTexture(FbxModel& Model, const std::string& filePath)
//{
//}

void FbxLoader::ParseSkin(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== �X�L�j���O���ǂݎ�� =====*/

	FbxSkin* fbxSkin = static_cast<FbxSkin*>(InputFbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	// �X�L�j���O��񂪂Ȃ���ΏI���B
	if (fbxSkin == nullptr) {
		return;
	}

	// �A�j���[�V�����������Ă���t���O�𗧂Ă�B
	Model.hasAnimation_ = true;

	// �{�[���z��̎Q�ƁB
	std::vector<Bone>& bones_ = Model.bones_;

	// �{�[���̐��B
	const int CLUSTER_COUNT = fbxSkin->GetClusterCount();
	bones_.reserve(CLUSTER_COUNT);

	// �S�Ẵ{�[���ɂ��āB
	for (int index_ = 0; index_ < CLUSTER_COUNT; ++index_) {

		// FBX�{�[�����B
		FbxCluster* fbxCluster_ = fbxSkin->GetCluster(index_);

		// �{�[�����̂̃m�[�h�̖��O���擾�B
		const char* boneName = fbxCluster_->GetLink()->GetName();

		// �V�����{�[����ǉ����A�ǉ������{�[���̎Q�Ƃ𓾂�B
		bones_.emplace_back(Bone(boneName));
		Bone& bone = bones_.back();

		// ����{�[����FBX�̃{�[����R�Â���B
		bone.fbxCluster_ = fbxCluster_;

		// FBX���珉���p���s����擾����B
		FbxAMatrix fbxMat;
		fbxCluster_->GetTransformLinkMatrix(fbxMat);

		// XMMatrix�^�ɕϊ�����B
		DirectX::XMMATRIX initialPose;
		ConvertMatrixFromFBX(initialPose, fbxMat);

		// �����p���s��̋t�s��𓾂�B
		bone.invInitialPose_ = DirectX::XMMatrixInverse(nullptr, initialPose);

	}

	// �񎟌��z��(�W���O�z��)
	// list   : ���_���e�����󂯂�{�[���̑S���X�g
	// vector : �����S���_��
	std::vector<std::list<WeightSet>> weightLists(Model.vertices_.size());

	// �S�Ẵ{�[���ɂ��āB
	for (int index_ = 0; index_ < CLUSTER_COUNT; ++index_) {

		// FBX�{�[�����B
		FbxCluster* fbxCluster_ = fbxSkin->GetCluster(index_);

		// ���̃{�[���ɉe�����󂯂钸�_�̐��B
		int CONTROL_POINT_INDICES_COUNT = fbxCluster_->GetControlPointIndicesCount();

		// ���̃{�[���ɉe�����󂯂钸�_�̐��B
		int* controlPointIndices = fbxCluster_->GetControlPointIndices();
		double* controlPointWeights = fbxCluster_->GetControlPointWeights();

		// �e�����󂯂�S���_�ɂ��āB
		for (int vertexIndex = 0; vertexIndex < CONTROL_POINT_INDICES_COUNT; ++vertexIndex) {

			// ���_�ԍ�
			int vertIndex = controlPointIndices[vertexIndex];
			// �X�L���E�F�C�g
			float weight_ = (float)controlPointWeights[vertexIndex];
			// ���̒��_�̉e�����󂯂�{�[�����X�g�ɁA�{�[���ƃE�F�C�g�̃y�A��ǉ��B
			weightLists[vertIndex].emplace_back(WeightSet({ static_cast<UINT>(index_), (weight_) }));

		}

	}

	// ���_�z�񏑂������p�̎Q��
	auto& vertices_ = Model.vertices_;

	// �e���_�ɂ��ď���
	const int VERTICES_SIZE = static_cast<int>(vertices_.size());
	for (int index_ = 0; index_ < VERTICES_SIZE; ++index_) {

		// ���_�̃E�F�C�g����ł��傫��4��I���B
		auto& weightList = weightLists[index_];

		// �召��r�p�̃����_�����Z�b�g���č~���Ƀ\�[�g�B
		weightList.sort([](auto const& lhs, auto const& rhs) {
			// �Ђ���̗v�f�̂ق����傫�����true �����łȂ����false��Ԃ��B
			return rhs.weight_ < lhs.weight_;
			});

		int weightArrayIndex = 0;

		// �~���\�[�g�ς݂̃E�F�C�g���X�g����B
		for (auto& weightSet : weightList) {

			// ���_�f�[�^�ɏ������݁B
			vertices_[index_].boneIndex_[weightArrayIndex] = weightSet.index_;
			vertices_[index_].boneWeight_[weightArrayIndex] = weightSet.weight_;

			// 4�ɒB������I���B
			if (FbxModel::MAX_BONE_INDICES <= ++weightArrayIndex) {

				float weight_ = 0.0f;

				// 2�Ԗڈȍ~�̃E�F�C�g�����v�B
				for (int boneIndex_ = 1; boneIndex_ < FbxModel::MAX_BONE_INDICES; ++boneIndex_) {

					weight_ += vertices_[index_].boneWeight_[boneIndex_];

				}

				// ���v��1.0f(100%)�ɂȂ�p�ɒ����B
				vertices_[index_].boneWeight_[0] = 1.0f - weight_;
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

void FbxModel::InitAnimation()
{

	/*===== FBX�A�j���[�V�����̍Đ� =====*/

	FbxScene* fbxScene_ = FbxLoader::Ins()->GetFbxScene();

	// 0�ԃA�j���[�V�����̍Đ��B
	FbxAnimStack* animstack = fbxScene_->GetSrcObject<FbxAnimStack>(0);

	// �A�j���[�V�����̖��O�擾�B
	const char* animstackName = animstack->GetName();

	// �A�j���[�V�����̎��ԏ��B
	FbxTakeInfo* takeInfo = fbxScene_->GetTakeInfo(animstackName);

	// �J�n���Ԏ擾�B
	startTime_ = takeInfo->mLocalTimeSpan.GetStart();

	// �I�����Ԏ擾�B
	endTime_ = takeInfo->mLocalTimeSpan.GetStop();

	// �J�n���Ԃɍ��킹��B
	currentTime_ = startTime_;

	isInit_ = true;

}

void FbxModel::PlayAnimation()
{

	/*===== FBX�A�j���[�V�����̍Đ� =====*/

	// ����������Ă����珉�����������Ă΂Ȃ��B
	if (!isInit_) {

		FbxScene* fbxScene_ = FbxLoader::Ins()->GetFbxScene();

		// 0�ԃA�j���[�V�����̍Đ��B
		FbxAnimStack* animstack = fbxScene_->GetSrcObject<FbxAnimStack>(0);

		// �A�j���[�V�����̖��O�擾�B
		const char* animstackName = animstack->GetName();

		// �A�j���[�V�����̎��ԏ��B
		FbxTakeInfo* takeInfo = fbxScene_->GetTakeInfo(animstackName);

		// �J�n���Ԏ擾�B
		startTime_ = takeInfo->mLocalTimeSpan.GetStart();

		// �I�����Ԏ擾�B
		endTime_ = takeInfo->mLocalTimeSpan.GetStop();

		// �J�n���Ԃɍ��킹��B
		currentTime_ = startTime_;

		isInit_ = true;

	}

	// �Đ�����Ԃɂ���B
	isPlay_ = true;

}

void FbxModel::StopAnimation()
{

	/*===== FBX�A�j���[�V�����̒�~ =====*/

	isPlay_ = false;

}
