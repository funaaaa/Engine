#include "ModelDataManager.h"
#include <array>
#pragma warning(push)
#pragma warning(disable:4099)
#pragma warning(disable:4023)
#include "Fbxsdk.h"
#pragma warning(pop)
#include <sstream>
#include <fstream>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "GLTF.h"

void ModelDataManager::LoadObj(std::string DirectryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing)
{
	// obj�t�@�C�������[�h�ς݂��ǂ���
	bool isLoad = false;
	// ���[�h�ς݂������ꍇ�A���Ԗڂ̗v�f�ɕۑ�����Ă���̂����擾����ϐ�
	int dataNumber = 0;

	const int MODELDATA_SIZE = static_cast<int>(modelData_.size());
	for (int index = 0; index < MODELDATA_SIZE; ++index) {
		if (modelData_[index].modelName_ == DirectryPath + FileName && modelData_[index].isSmoothing_ == IsSmoothing) {
			isLoad = true;
			dataNumber = index;
			break;
		}
	}

	// obj�t�@�C���������[�h�������烍�[�h����
	if (!isLoad) {
		// �������_���w�������Ă��钸�_�C���f�b�N�X��ۑ����Ă������߂̃}�b�v
		std::map<unsigned short, std::vector<unsigned short>> smoothData;

		modelData_.push_back({});
		ModelDataManager::modelData_.at(ModelDataManager::modelData_.size() - 1).modelName_ = DirectryPath + FileName;
		ModelDataManager::modelData_.at(ModelDataManager::modelData_.size() - 1).isSmoothing_ = IsSmoothing;

		// �ϐ����������B
		ObjectBuffer.vertexMax_ = Vec3();
		ObjectBuffer.vertexMin_ = Vec3();

		// �t�B���X�g���[��
		std::ifstream file;
		// .obj�t�@�C�����J��
		file.open(DirectryPath + FileName);
		// �t�@�C���I�[�v�����s���`�F�b�N
		if (file.fail()) {
			assert(0);
		}
		// ��s���ǂݍ���
		std::string line;
		std::vector<Vec3> position_;			// ���W��ۑ����Ă���
		std::vector<Vec2> uv_;				// uv��ۑ����Ă���
		std::vector<Vec3> normal_;			// �@���x�N�g����ۑ����Ă���
		std::vector<unsigned short> index_;
		bool isFace = false;	// face�����݂������ǂ����̃t���O
		while (getline(file, line)) {
			// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
			std::istringstream lineStream(line);
			// ���p�X�y�[�X��؂�ōs�̐擪�������擾
			std::string key;
			std::getline(lineStream, key, ' ');

			// �擪������v�Ȃ璸�_���W
			if (key == "v") {
				// X,Y,Z���W�ǂݍ���
				Vec3 pos_{};
				lineStream >> pos_.x_;
				lineStream >> pos_.y_;
				lineStream >> pos_.z_;
				// ���W����U�ۑ�
				position_.push_back(pos_);
				// ���_�̍ő�ŏ��v�f��ۑ��B
				SaveVertexMinMaxInfo(ObjectBuffer, pos_);
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertexMin_ = ObjectBuffer.vertexMin_;
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertexMax_ = ObjectBuffer.vertexMax_;
			}
			// �擪������vt�Ȃ�e�N�X�`��
			if (key == "vt") {
				// U,V�����ǂݍ���
				Vec2 texcoord{};
				lineStream >> texcoord.x_;
				lineStream >> texcoord.y_;
				// V�������]
				texcoord.y_ = 1.0f - texcoord.y_;

				// �e�N�X�`�����W�f�[�^�ɒǉ�
				uv_.push_back(texcoord);
			}
			// �擪������vn�Ȃ�@���x�N�g��
			if (key == "vn") {
				// X,Y,Z�����ǂݍ���
				Vec3 norm{};
				lineStream >> norm.x_;
				lineStream >> norm.y_;
				lineStream >> norm.z_;
				// �@���x�N�g���f�[�^�ɒǉ�
				normal_.push_back(norm);
			}
			// �擪������f�Ȃ�|���S��(�O�p�`)
			if (key == "f") {
				isFace = true;
				// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
				std::string indexString;
				unsigned short indexPosition;		// �󂯎M
				unsigned short indexNormal;			// �󂯎M
				unsigned short indexTexcoord;		// �󂯎M
				while (std::getline(lineStream, indexString, ' ')) {
					// ���_�C���f�b�N�X����̕�������X�g���[���ɕϊ����ĉ�͂��₷������
					std::istringstream indexStream(indexString);
					indexStream >> indexPosition;
					indexStream.seekg(1, std::ios_base::cur);	// �X���b�V�����΂�
					indexStream >> indexTexcoord;
					indexStream.seekg(1, std::ios_base::cur);	// �X���b�V�����΂�
					indexStream >> indexNormal;
					// ���_�f�[�^�̒ǉ�
					Vertex vert{};
					vert.pos_ = position_[indexPosition - 1];
					vert.normal_ = normal_[indexNormal - 1];
					vert.uv_ = uv_[indexTexcoord - 1];
					// ���f���f�[�^�ɒǉ�
					modelData_[(static_cast<int>(modelData_.size()) - 1)].vertex_.push_back(vert);
					modelData_[(static_cast<int>(modelData_.size()) - 1)].index_.push_back(static_cast<unsigned short>(modelData_[(static_cast<int>(modelData_.size())) - 1].index_.size()));
					// proSprite�ɂ��ǉ�
					ObjectBuffer.vertex_.push_back(vert);
					ObjectBuffer.index_.push_back(static_cast<int>(ObjectBuffer.index_.size()));
					// isSmoothing��true�Ȃ璸�_����ǉ�����
					if (IsSmoothing == true) {
						smoothData[indexPosition].push_back(static_cast<unsigned short>(ObjectBuffer.vertex_.size()) - 1);
					}
				}
			}
			// �擪������mtllib�Ȃ�}�e���A��
			if (key == "mtllib") {
				// �}�e���A���̃t�@�C�����ǂݍ���
				std::string materialFileName;
				lineStream >> materialFileName;
				// �}�e���A���̓ǂݍ���
				LoadObjMaterial(DirectryPath, DirectryPath + materialFileName, modelData_.at(modelData_.size() - 1), ObjectBuffer);
				ObjectBuffer.material_ = modelData_[modelData_.size() - 1].material_;
				ObjectBuffer.material_.textureHandle_ = ObjectBuffer.textureHandle_;
				modelData_[modelData_.size() - 1].material_.textureHandle_ = ObjectBuffer.textureHandle_;

			}
		}
		// �t�@�C�������
		file.close();

		// isSmoothing��true��������@�������Ȃ߂炩�ɂ���
		if (IsSmoothing) {
			CalculateSmoothedVertexNormals(smoothData, ObjectBuffer, modelData_.at(modelData_.size() - 1));
		}

		// face�����݂��Ȃ������璸�_�f�[�^��ۑ�����B
		if (!isFace) {
			for (auto& index : position_) {
				ObjectBuffer.vertex_.emplace_back();
				ObjectBuffer.vertex_.back().pos_ = index;
			}
			for (auto& index : position_) {
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertex_.emplace_back();
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertex_.back().pos_ = index;
			}
		}

		return;
	}

	// obj�̃f�[�^��ObjectData�ɓ����
	for (auto& index_ : modelData_[dataNumber].index_) {
		ObjectBuffer.index_.push_back(index_);
	}
	for (auto& index_ : modelData_[dataNumber].vertex_) {
		ObjectBuffer.vertex_.push_back(index_);
	}
	ObjectBuffer.textureHandle_ = modelData_[dataNumber].material_.textureHandle_;
	ObjectBuffer.material_ = modelData_[dataNumber].material_;
	ObjectBuffer.vertexMin_ = modelData_[dataNumber].vertexMin_;
	ObjectBuffer.vertexMax_ = modelData_[dataNumber].vertexMax_;

}

#include "FString.h"
void ModelDataManager::LoadGLTF(std::wstring Path, ObjectData& ObjectBuffer) {

	/*===== GLTF�����[�h =====*/

	// gltf�t�@�C�������[�h�ς݂��ǂ���
	bool isLoad = false;
	// ���[�h�ς݂������ꍇ�A���Ԗڂ̗v�f�ɕۑ�����Ă���̂����擾����ϐ�
	int dataNumber = 0;

	const int MODELDATA_SIZE = static_cast<int>(modelData_.size());
	for (int index = 0; index < MODELDATA_SIZE; ++index) {
		if (modelData_[index].modelName_ == FString::WStringToString(Path)) {
			isLoad = true;
			dataNumber = index;
			break;
		}
	}

	// gltf�t�@�C���������[�h�������烍�[�h����
	if (!isLoad) {

		modelData_.push_back({});
		ModelDataManager::modelData_.at(ModelDataManager::modelData_.size() - 1).modelName_ = FString::WStringToString(Path);

		gltf_.emplace_back(GLTF());
		gltf_.back().LoadModel(Path);

		// ���f���̃f�[�^���擾�B
		GLTF::VertexAttributeVisitor vertexInfo = gltf_.back().GetVertexData();
		filePath_.emplace_back(gltf_.back().GetFileName());
		for (auto& index : vertexInfo.positionBuffer) {

			Vertex buff;
			buff.pos_ = index;
			buff.normal_ = vertexInfo.normalBuffer[static_cast<int>(&index - &vertexInfo.positionBuffer[0])];
			buff.uv_ = vertexInfo.texcoordBuffer[static_cast<int>(&index - &vertexInfo.positionBuffer[0])];

			// ���_�̍ő�ƍŏ���ۑ����Ă����B
			SaveVertexMinMaxInfo(ObjectBuffer, buff.pos_);
			SaveVertexMinMaxInfo(ObjectBuffer, buff.pos_);

			ObjectBuffer.vertex_.emplace_back(buff);
			modelData_.back().vertex_.emplace_back(buff);

		}
		for (auto& index : vertexInfo.indexBuffer) {

			ObjectBuffer.index_.emplace_back(index);
			modelData_.back().index_.emplace_back(index);

		}
		ObjectBuffer.textureHandle_ = gltf_.back().GetTextureIndex();

		// �}�e���A����ۑ��B
		ObjectBuffer.material_.baseColor_ = gltf_.back().GetMaterial().baseColor_;
		ObjectBuffer.material_.metalness_ = gltf_.back().GetMaterial().metalness_;
		ObjectBuffer.material_.roughness_ = gltf_.back().GetMaterial().roughness_;
		ObjectBuffer.material_.specular_ = gltf_.back().GetMaterial().specular_;

		modelData_.back().material_.baseColor_ = gltf_.back().GetMaterial().baseColor_;
		modelData_.back().material_.metalness_ = gltf_.back().GetMaterial().metalness_;
		modelData_.back().material_.roughness_ = gltf_.back().GetMaterial().roughness_;
		modelData_.back().material_.specular_ = gltf_.back().GetMaterial().specular_;
		modelData_.back().material_.textureHandle_ = ObjectBuffer.textureHandle_;


		return;

	}

	// gltf�̃f�[�^��ObjectData�ɓ����
	for (auto& index_ : modelData_[dataNumber].index_) {
		ObjectBuffer.index_.push_back(index_);
	}
	for (auto& index_ : modelData_[dataNumber].vertex_) {
		ObjectBuffer.vertex_.push_back(index_);
	}
	ObjectBuffer.textureHandle_ = modelData_[dataNumber].material_.textureHandle_;
	ObjectBuffer.material_ = modelData_[dataNumber].material_;
	ObjectBuffer.vertexMin_ = modelData_[dataNumber].vertexMin_;
	ObjectBuffer.vertexMax_ = modelData_[dataNumber].vertexMax_;

}

#include "TextureManager.h"
void ModelDataManager::LoadObjMaterial(std::string DirectoryPath, const std::string& MaterialFileName, ModelData& ModelData, ObjectData& ObjectBuffer)
{

	// �t�@�C���X�g���[��
	std::ifstream file;
	// �}�e���A���t�@�C�����J��
	file.open(MaterialFileName);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}
	// ��s���ǂݍ���
	std::string line;
	while (getline(file, line)) {
		// �ʒu�s���̕�������X�g���[���ɕϊ�
		std::istringstream lineStream(line);
		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		std::getline(lineStream, key, ' ');
		// �擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin());	// �擪�̕������폜
		}
		// �擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {
			// �}�e���A�����̓ǂݍ���
			// lineStream >> ModelData.material.name;
		}
		//// �擪������Ka�Ȃ�A���r�G���g�F
		//if (key == "Ka") {
		//	lineStream >> ModelData.material_.ambient_.x_;
		//	lineStream >> ModelData.material_.ambient_.y_;
		//	lineStream >> ModelData.material_.ambient_.z_;
		//}
		//// �擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			lineStream >> ModelData.material_.roughness_;
			//	lineStream >> ModelData.material_.diffuse_.x_;
			//	lineStream >> ModelData.material_.diffuse_.y_;
			//	lineStream >> ModelData.material_.diffuse_.z_;
		}
		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			lineStream >> ModelData.material_.metalness_;
			//lineStream >> ModelData.material_.specular.y_;
			//lineStream >> ModelData.material_.specular.z_;
		}
		// �擪������map_Kd�Ȃ�e�N�X�`��
		if (key == "map_Kd") {

			std::array<wchar_t, 128> wFilePath;

			std::string fullPath;
			lineStream >> fullPath;
			fullPath = DirectoryPath + fullPath;

			MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath.data(), static_cast<int>(wFilePath.size()));

			// �e�N�X�`�������[�h�B
			ObjectBuffer.textureHandle_ = TextureManager::Ins()->LoadTexture(wFilePath);

		}
	}
}

void ModelDataManager::CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectData, ModelData& ModelData)
{
	auto itr = SmoothData.begin();
	for (; itr != SmoothData.end(); ++itr) {
		// �e�ʗp�̋��ʒ��_�R���N�V����
		std::vector<unsigned short>& v = itr->second;
		// �S���_�̖@���𕽋ς���
		Vec3 normal_ = {};
		for (unsigned short index_ : v) {
			normal_ += ObjectData.vertex_[index_].normal_;
		}
		normal_ = normal_ / (float)v.size();
		normal_.Normalize();

		for (unsigned short index_ : v) {
			ObjectData.vertex_[index_].normal_ = normal_;
			ModelData.vertex_[index_].normal_ = normal_;
		}
	}
}

void ModelDataManager::SaveVertexMinMaxInfo(ObjectData& ObjectBuffer, const Vec3& Pos)
{

	/*===== ���_�̍ő�ŏ��̏���ۑ� =====*/

	// �ő��ۑ��B

	// Pos�̊e�������ۑ�����Ă���l�����傫��������B
	if (ObjectBuffer.vertexMax_.x_ < Pos.x_) {
		ObjectBuffer.vertexMax_.x_ = Pos.x_;
	}
	if (ObjectBuffer.vertexMax_.y_ < Pos.y_) {
		ObjectBuffer.vertexMax_.y_ = Pos.y_;
	}
	if (ObjectBuffer.vertexMax_.z_ < Pos.z_) {
		ObjectBuffer.vertexMax_.z_ = Pos.z_;
	}

	// Pos�̊e�������ۑ�����Ă���l����������������B
	if (Pos.x_ < ObjectBuffer.vertexMin_.x_) {
		ObjectBuffer.vertexMin_.x_ = Pos.x_;
	}
	if (Pos.y_ < ObjectBuffer.vertexMin_.y_) {
		ObjectBuffer.vertexMin_.y_ = Pos.y_;
	}
	if (Pos.z_ < ObjectBuffer.vertexMin_.z_) {
		ObjectBuffer.vertexMin_.z_ = Pos.z_;
	}

}
