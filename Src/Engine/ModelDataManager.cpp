#include "ModelDataManager.h"
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

void ModelDataManager::LoadObj(std::string DirectryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing)
{
	// obj�t�@�C�������[�h�ς݂��ǂ���
	bool isLoad = false;
	// ���[�h�ς݂������ꍇ�A���Ԗڂ̗v�f�ɕۑ�����Ă���̂����擾����ϐ�
	int dataNumber = 0;

	const int MODELDATA_SIZE = static_cast<int>(modelData.size());
	for (int index = 0; index < MODELDATA_SIZE; ++index) {
		if (modelData[index].modelName == DirectryPath + FileName && modelData[index].isSmoothing == IsSmoothing) {
			isLoad = true;
			dataNumber = index;
			break;
		}
	}

	// obj�t�@�C���������[�h�������烍�[�h����
	if (!isLoad) {
		// �������_���w�������Ă��钸�_�C���f�b�N�X��ۑ����Ă������߂̃}�b�v
		std::map<unsigned short, std::vector<unsigned short>> smoothData;

		modelData.push_back({});
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = DirectryPath + FileName;
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).isSmoothing = IsSmoothing;

		// �ϐ����������B
		ObjectBuffer.vertexMax = Vec3();
		ObjectBuffer.vertexMin = Vec3();

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
		std::vector<Vec3> position;			// ���W��ۑ����Ă���
		std::vector<Vec2> uv;				// uv��ۑ����Ă���
		std::vector<Vec3> normal;			// �@���x�N�g����ۑ����Ă���
		std::vector<unsigned short> index;
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
				position.push_back(pos_);
				// ���_�̍ő�ŏ��v�f��ۑ��B
				SaveVertexMinMaxInfo(ObjectBuffer, pos_);
				modelData[(static_cast<int>(modelData.size()) - 1)].vertexMin = ObjectBuffer.vertexMin;
				modelData[(static_cast<int>(modelData.size()) - 1)].vertexMax = ObjectBuffer.vertexMax;
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
				uv.push_back(texcoord);
			}
			// �擪������vn�Ȃ�@���x�N�g��
			if (key == "vn") {
				// X,Y,Z�����ǂݍ���
				Vec3 norm{};
				lineStream >> norm.x_;
				lineStream >> norm.y_;
				lineStream >> norm.z_;
				// �@���x�N�g���f�[�^�ɒǉ�
				normal.push_back(norm);
			}
			// �擪������f�Ȃ�|���S��(�O�p�`)
			if (key == "f") {
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
					vert.pos_ = position[indexPosition - 1];
					vert.normal = normal[indexNormal - 1];
					vert.uv = uv[indexTexcoord - 1];
					// ���f���f�[�^�ɒǉ�
					modelData[(static_cast<int>(modelData.size()) - 1)].vertex.push_back(vert);
					modelData[(static_cast<int>(modelData.size()) - 1)].index.push_back(static_cast<unsigned short>(modelData[(static_cast<int>(modelData.size())) - 1].index.size()));
					// proSprite�ɂ��ǉ�
					ObjectBuffer.vertex.push_back(vert);
					ObjectBuffer.index.push_back(static_cast<int>(ObjectBuffer.index.size()));
					// isSmoothing��true�Ȃ璸�_����ǉ�����
					if (IsSmoothing == true) {
						smoothData[indexPosition].push_back(static_cast<unsigned short>(ObjectBuffer.vertex.size()) - 1);
					}
				}
			}
			// �擪������mtllib�Ȃ�}�e���A��
			if (key == "mtllib") {
				// �}�e���A���̃t�@�C�����ǂݍ���
				std::string materialFileName;
				lineStream >> materialFileName;
				// �}�e���A���̓ǂݍ���
				LoadObjMaterial(DirectryPath + materialFileName, modelData.at(modelData.size() - 1));
				ObjectBuffer.material = modelData[modelData.size() - 1].material;

			}
		}
		// �t�@�C�������
		file.close();

		// isSmoothing��true��������@�������Ȃ߂炩�ɂ���
		if (IsSmoothing) {
			CalculateSmoothedVertexNormals(smoothData, ObjectBuffer, modelData.at(modelData.size() - 1));
		}

		return;
	}

	// obj�̃f�[�^��ObjectData�ɓ����
	for (auto& index : modelData[dataNumber].index) {
		ObjectBuffer.index.push_back(index);
	}
	for (auto& index : modelData[dataNumber].vertex) {
		ObjectBuffer.vertex.push_back(index);
	}
	ObjectBuffer.material = modelData[dataNumber].material;
	ObjectBuffer.vertexMin = modelData[dataNumber].vertexMin;
	ObjectBuffer.vertexMax = modelData[dataNumber].vertexMax;

}

void ModelDataManager::LoadObjMaterial(const std::string& MaterialFileName, ModelData& ModelData)
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
		// �擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			lineStream >> ModelData.material.ambient.x_;
			lineStream >> ModelData.material.ambient.y_;
			lineStream >> ModelData.material.ambient.z_;
		}
		// �擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			lineStream >> ModelData.material.diffuse.x_;
			lineStream >> ModelData.material.diffuse.y_;
			lineStream >> ModelData.material.diffuse.z_;
		}
		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			lineStream >> ModelData.material.specular.x_;
			lineStream >> ModelData.material.specular.y_;
			lineStream >> ModelData.material.specular.z_;
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
		Vec3 normal = {};
		for (unsigned short index : v) {
			normal += ObjectData.vertex[index].normal;
		}
		normal = normal / (float)v.size();
		normal.Normalize();

		for (unsigned short index : v) {
			ObjectData.vertex[index].normal = normal;
			ModelData.vertex[index].normal = normal;
		}
	}
}

void ModelDataManager::SaveVertexMinMaxInfo(ObjectData& ObjectBuffer, const Vec3& Pos)
{

	/*===== ���_�̍ő�ŏ��̏���ۑ� =====*/

	// �ő��ۑ��B

	// Pos�̊e�������ۑ�����Ă���l�����傫��������B
	if (ObjectBuffer.vertexMax.x_ < Pos.x_) {
		ObjectBuffer.vertexMax.x_ = Pos.x_;
	}
	if (ObjectBuffer.vertexMax.y_ < Pos.y_) {
		ObjectBuffer.vertexMax.y_ = Pos.y_;
	}
	if (ObjectBuffer.vertexMax.z_ < Pos.z_) {
		ObjectBuffer.vertexMax.z_ = Pos.z_;
	}

	// Pos�̊e�������ۑ�����Ă���l����������������B
	if (Pos.x_ < ObjectBuffer.vertexMin.x_) {
		ObjectBuffer.vertexMin.x_ = Pos.x_;
	}
	if (Pos.y_ < ObjectBuffer.vertexMin.y_) {
		ObjectBuffer.vertexMin.y_ = Pos.y_;
	}
	if (Pos.z_ < ObjectBuffer.vertexMin.z_) {
		ObjectBuffer.vertexMin.z_ = Pos.z_;
	}

}
