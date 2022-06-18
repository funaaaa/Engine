#include "ModelDataManager.h"
#include "Fbxsdk.h"
#include <sstream>
#include <fstream>
#include <memory>

void ModelDataManager::LoadObj(std::string DirectryPath, std::string FileName, ObjectData& ObjectData, bool IsSmoothing)
{
	//obj�t�@�C�������[�h�ς݂��ǂ���
	bool isLoad = false;
	//���[�h�ς݂������ꍇ�A���Ԗڂ̗v�f�ɕۑ�����Ă���̂����擾����ϐ�
	int dataNumber = 0;

	for (int i = 0; i < ModelDataManager::modelData.size(); ++i) {
		if (modelData.at(i).modelName == DirectryPath + FileName && modelData.at(i).isSmoothing == IsSmoothing) {
			isLoad = true;
			dataNumber = i;
			break;
		}
	}

	//obj�t�@�C���������[�h�������烍�[�h����
	if (isLoad == false) {
		//�������_���w�������Ă��钸�_�C���f�b�N�X��ۑ����Ă������߂̃}�b�v
		std::map<unsigned short, std::vector<unsigned short>> smoothData;

		modelData.push_back({});
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = DirectryPath + FileName;
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).isSmoothing = IsSmoothing;

		//�t�B���X�g���[��
		std::ifstream file;
		//.obj�t�@�C�����J��
		file.open(DirectryPath + FileName);
		//�t�@�C���I�[�v�����s���`�F�b�N
		if (file.fail()) {
			assert(0);
		}
		//��s���ǂݍ���
		std::string line;
		std::vector<Vec3> position;	//���W��ۑ����Ă���
		std::vector<DirectX::XMFLOAT2> uv;		//uv��ۑ����Ă���
		std::vector<Vec3> normal;	//�@���x�N�g����ۑ����Ă���
		std::vector<unsigned short> index;
		while (getline(file, line)) {
			//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
			std::istringstream lineStream(line);
			//���p�X�y�[�X��؂�ōs�̐擪�������擾
			std::string key;
			std::getline(lineStream, key, ' ');

			//�擪������v�Ȃ璸�_���W
			if (key == "v") {
				//X,Y,Z���W�ǂݍ���
				Vec3 pos{};
				lineStream >> pos.x;
				lineStream >> pos.y;
				lineStream >> pos.z;
				//���W����U�ۑ�
				position.push_back(pos);
			}
			//�擪������vt�Ȃ�e�N�X�`��
			if (key == "vt") {
				//U,V�����ǂݍ���
				DirectX::XMFLOAT2 texcoord{};
				lineStream >> texcoord.x;
				lineStream >> texcoord.y;
				//V�������]
				texcoord.y = 1.0f - texcoord.y;
				//�e�N�X�`�����W�f�[�^�ɒǉ�
				uv.push_back(texcoord);
			}
			//�擪������vn�Ȃ�@���x�N�g��
			if (key == "vn") {
				//X,Y,Z�����ǂݍ���
				Vec3 norm{};
				lineStream >> norm.x;
				lineStream >> norm.y;
				lineStream >> norm.z;
				//�@���x�N�g���f�[�^�ɒǉ�
				normal.push_back(norm);
			}
			//�擪������f�Ȃ�|���S��(�O�p�`)
			if (key == "f") {
				//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
				std::string indexString;
				unsigned short indexPosition;		//�󂯎M
				unsigned short indexNormal;			//�󂯎M
				unsigned short indexTexcoord;		//�󂯎M
				while (std::getline(lineStream, indexString, ' ')) {
					//���_�C���f�b�N�X����̕�������X�g���[���ɕϊ����ĉ�͂��₷������
					std::istringstream indexStream(indexString);
					indexStream >> indexPosition;
					indexStream.seekg(1, std::ios_base::cur);	//�X���b�V�����΂�
					indexStream >> indexTexcoord;
					indexStream.seekg(1, std::ios_base::cur);	//�X���b�V�����΂�
					indexStream >> indexNormal;
					//���_�f�[�^�̒ǉ�
					Vertex vert{};
					vert.pos = position[indexPosition - 1];
					vert.normal = normal[indexNormal - 1];
					vert.uv = uv[indexTexcoord - 1];
					//���f���f�[�^�ɒǉ�
					modelData.at(modelData.size() - 1).vertex.push_back(vert);
					modelData.at(modelData.size() - 1).index.push_back(modelData.at(modelData.size() - 1).index.size());
					//proSprite�ɂ��ǉ�
					ObjectData.vertex.push_back(vert);
					ObjectData.index.push_back(ObjectData.index.size());
					//isSmoothing��true�Ȃ璸�_����ǉ�����
					if (IsSmoothing == true) {
						smoothData[indexPosition].push_back(ObjectData.vertex.size() - 1);
					}
				}
			}
			//�擪������mtllib�Ȃ�}�e���A��
			if (key == "mtllib") {
				//�}�e���A���̃t�@�C�����ǂݍ���
				std::string materialFileName;
				lineStream >> materialFileName;
				//�}�e���A���̓ǂݍ���
				LoadObjMaterial(DirectryPath + materialFileName, modelData.at(modelData.size() - 1), ObjectData);
			}
		}
		//�t�@�C�������
		file.close();

		//isSmoothing��true��������@�������Ȃ߂炩�ɂ���
		if (IsSmoothing == true) {
			CalculateSmoothedVertexNormals(smoothData, ObjectData, modelData.at(modelData.size() - 1));
		}

		return;
	}

	//obj�̃f�[�^��proSprite�ɓ����
	for (int i = 0; i < modelData.at(dataNumber).index.size(); ++i) {
		ObjectData.index.push_back(modelData.at(dataNumber).index.at(i));
	}
	for (int i = 0; i < modelData.at(dataNumber).vertex.size(); ++i) {
		ObjectData.vertex.push_back(modelData.at(dataNumber).vertex.at(i));
	}
	
}

//void ModelDataManager::LoadObj(string directoryPath, string fileName, Object3DShadow& objectBuffer, bool isSmoothing)
//{
//	//obj�t�@�C�������[�h�ς݂��ǂ���
//	bool isLoad = false;
//	//���[�h�ς݂������ꍇ�A���Ԗڂ̗v�f�ɕۑ�����Ă���̂����擾����ϐ�
//	int dataNumber = 0;
//
//	for (int i = 0; i < ModelDataManager::modelData.size(); ++i) {
//		if (modelData.at(i).modelName == directoryPath + fileName && modelData.at(i).isSmoothing == isSmoothing) {
//			isLoad = true;
//			dataNumber = i;
//			break;
//		}
//	}
//
//	//obj�t�@�C���������[�h�������烍�[�h����
//	if (isLoad == false) {
//		//�������_���w�������Ă��钸�_�C���f�b�N�X��ۑ����Ă������߂̃}�b�v
//		map<unsigned short, vector<unsigned short>> smoothData;
//
//		modelData.push_back({});
//		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = directoryPath + fileName;
//		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).isSmoothing = isSmoothing;
//
//		//�t�B���X�g���[��
//		ifstream file;
//		//.obj�t�@�C�����J��
//		file.open(directoryPath + fileName);
//		//�t�@�C���I�[�v�����s���`�F�b�N
//		if (file.fail()) {
//			assert(0);
//		}
//		//��s���ǂݍ���
//		string line;
//		vector<XMFLOAT3> position;	//���W��ۑ����Ă���
//		vector<XMFLOAT2> uv;		//uv��ۑ����Ă���
//		vector<XMFLOAT3> normal;	//�@���x�N�g����ۑ����Ă���
//		vector<unsigned short> index;
//		while (getline(file, line)) {
//			//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
//			istringstream lineStream(line);
//			//���p�X�y�[�X��؂�ōs�̐擪�������擾
//			string key;
//			getline(lineStream, key, ' ');
//
//			//�擪������v�Ȃ璸�_���W
//			if (key == "v") {
//				//X,Y,Z���W�ǂݍ���
//				XMFLOAT3 pos{};
//				lineStream >> pos.x;
//				lineStream >> pos.y;
//				lineStream >> pos.z;
//				//���W����U�ۑ�
//				position.push_back(pos);
//			}
//			//�擪������vt�Ȃ�e�N�X�`��
//			if (key == "vt") {
//				//U,V�����ǂݍ���
//				XMFLOAT2 texcoord{};
//				lineStream >> texcoord.x;
//				lineStream >> texcoord.y;
//				//V�������]
//				texcoord.y = 1.0f - texcoord.y;
//				//�e�N�X�`�����W�f�[�^�ɒǉ�
//				uv.push_back(texcoord);
//			}
//			//�擪������vn�Ȃ�@���x�N�g��
//			if (key == "vn") {
//				//X,Y,Z�����ǂݍ���
//				XMFLOAT3 norm{};
//				lineStream >> norm.x;
//				lineStream >> norm.y;
//				lineStream >> norm.z;
//				//�@���x�N�g���f�[�^�ɒǉ�
//				normal.push_back(norm);
//			}
//			//�擪������f�Ȃ�|���S��(�O�p�`)
//			if (key == "f") {
//				//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
//				string indexString;
//				unsigned short indexPosition;		//�󂯎M
//				unsigned short indexNormal;			//�󂯎M
//				unsigned short indexTexcoord;		//�󂯎M
//				while (getline(lineStream, indexString, ' ')) {
//					//���_�C���f�b�N�X����̕�������X�g���[���ɕϊ����ĉ�͂��₷������
//					istringstream indexStream(indexString);
//					indexStream >> indexPosition;
//					indexStream.seekg(1, ios_base::cur);	//�X���b�V�����΂�
//					indexStream >> indexTexcoord;
//					indexStream.seekg(1, ios_base::cur);	//�X���b�V�����΂�
//					indexStream >> indexNormal;
//					//���_�f�[�^�̒ǉ�
//					Vertex vert{};
//					vert.pos = position[indexPosition - 1];
//					vert.normal = normal[indexNormal - 1];
//					vert.uv = uv[indexTexcoord - 1];
//					//���f���f�[�^�ɒǉ�
//					modelData.at(modelData.size() - 1).vertex.push_back(vert);
//					modelData.at(modelData.size() - 1).index.push_back(modelData.at(modelData.size() - 1).index.size());
//					//proSprite�ɂ��ǉ�
//					objectBuffer.vertex.push_back(vert);
//					objectBuffer.index.push_back(objectBuffer.index.size());
//					//isSmoothing��true�Ȃ璸�_����ǉ�����
//					if (isSmoothing == true) {
//						smoothData[indexPosition].push_back(objectBuffer.vertex.size() - 1);
//					}
//				}
//			}
//			//�擪������mtllib�Ȃ�}�e���A��
//			if (key == "mtllib") {
//				//�}�e���A���̃t�@�C�����ǂݍ���
//				string materialFileName;
//				lineStream >> materialFileName;
//				//�}�e���A���̓ǂݍ���
//				LoadObjMaterial(directoryPath + materialFileName, modelData.at(modelData.size() - 1), objectBuffer);
//			}
//		}
//		//�t�@�C�������
//		file.close();
//
//		//isSmoothing��true��������@�������Ȃ߂炩�ɂ���
//		if (isSmoothing == true) {
//			CalculateSmoothedVertexNormals(smoothData, objectBuffer, modelData.at(modelData.size() - 1));
//		}
//
//		return;
//	}
//
//	//obj�̃f�[�^��proSprite�ɓ����
//	for (int i = 0; i < modelData.at(dataNumber).index.size(); ++i) {
//		objectBuffer.index.push_back(modelData.at(dataNumber).index.at(i));
//	}
//	for (int i = 0; i < modelData.at(dataNumber).vertex.size(); ++i) {
//		objectBuffer.vertex.push_back(modelData.at(dataNumber).vertex.at(i));
//	}
//	objectBuffer.constBuffDataB1.alpha = 1;
//	objectBuffer.constBuffDataB1.ambient = modelData.at(dataNumber).material.ambient;
//	objectBuffer.constBuffDataB1.diffuse = modelData.at(dataNumber).material.diffuse;
//	objectBuffer.constBuffDataB1.specular = modelData.at(dataNumber).material.specular;
//}

void ModelDataManager::LoadObjMaterial(const std::string& materialFileName, ModelData& modelData, ObjectData& objectBuffer)
{

	//�t�@�C���X�g���[��
	std::ifstream file;
	//�}�e���A���t�@�C�����J��
	file.open(materialFileName);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}
	//��s���ǂݍ���
	std::string line;
	while (getline(file, line)) {
		//�ʒu�s���̕�������X�g���[���ɕϊ�
		std::istringstream lineStream(line);
		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		std::getline(lineStream, key, ' ');
		//�擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin());	//�擪�̕������폜
		}
		//�擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {
			//�}�e���A�����̓ǂݍ���
			lineStream >> modelData.material.name;
		}
		////�擪������Ka�Ȃ�A���r�G���g�F
		//if (key == "Ka") {
		//	lineStream >> modelData.material.ambient.x;
		//	lineStream >> modelData.material.ambient.y;
		//	lineStream >> modelData.material.ambient.z;
		//	objectBuffer.constBufferDataB1.ambient.x = modelData.material.ambient.x;
		//	objectBuffer.constBufferDataB1.ambient.y = modelData.material.ambient.y;
		//	objectBuffer.constBufferDataB1.ambient.z = modelData.material.ambient.z;
		//}
		////�擪������Kd�Ȃ�f�B�t���[�Y�F
		//if (key == "Kd") {
		//	lineStream >> modelData.material.diffuse.x;
		//	lineStream >> modelData.material.diffuse.y;
		//	lineStream >> modelData.material.diffuse.z;
		//	objectBuffer.constBufferDataB1.diffuse.x = modelData.material.diffuse.x;
		//	objectBuffer.constBufferDataB1.diffuse.y = modelData.material.diffuse.y;
		//	objectBuffer.constBufferDataB1.diffuse.z = modelData.material.diffuse.z;
		//}
		////�擪������Ks�Ȃ�X�y�L�����[�F
		//if (key == "Ks") {
		//	lineStream >> modelData.material.specular.x;
		//	lineStream >> modelData.material.specular.y;
		//	lineStream >> modelData.material.specular.z;
		//	objectBuffer.constBufferDataB1.specular.x = modelData.material.specular.x;
		//	objectBuffer.constBufferDataB1.specular.y = modelData.material.specular.y;
		//	objectBuffer.constBufferDataB1.specular.z = modelData.material.specular.z;
		//}
	}
}

void ModelDataManager::CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& smoothData, ObjectData& objectBuffer, ModelData& modelData)
{
	auto itr = smoothData.begin();
	for (; itr != smoothData.end(); ++itr) {
		// �e�ʗp�̋��ʒ��_�R���N�V����
		std::vector<unsigned short>& v = itr->second;
		// �S���_�̖@���𕽋ς���
		Vec3 normal = {};
		for (unsigned short index : v) {
			normal += objectBuffer.vertex[index].normal;
		}
		normal = normal / (float)v.size();
		normal.Normalize();

		for (unsigned short index : v) {
			objectBuffer.vertex[index].normal = normal;
			modelData.vertex[index].normal = normal;
		}
	}
}
