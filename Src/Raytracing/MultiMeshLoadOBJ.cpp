#include "MultiMeshLoadOBJ.h"
#include "BLAS.h"
#include "Vec.h"
#include "BLASRegister.h"
#include "TextureManager.h"
#include "PorygonInstanceRegister.h"
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <cassert>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "ModelDataManager.h"

std::vector<int> MultiMeshLoadOBJ::RayMultiMeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName)
{

	/*===== ���C�g���pOBJ�������b�V���ǂݍ��݊֐� =====*/

	std::vector<int> InstanceID;

	// �t�B���X�g���[��
	std::ifstream file;

	// OBJ�t�@�C�����J��
	file.open(DirectryPath + FilePath);

	// �t�@�C���I�[�v�����s���`�F�b�N�B
	if (file.fail()) {
		assert(0);
	}

	// BLAS�𐶐�����ۂɒl�Ƃ��ēn���f�[�^�B
	ModelDataManager::ObjectData blasData;

	// ��s���ǂݍ��ށB
	string line;

	// �f�[�^���ꎟ�ۑ����Ă������߂̃R���e�i�B�B
	std::vector<Vec3> position;
	std::vector<DirectX::XMFLOAT2> uv;
	std::vector<Vec3> normal;
	std::vector<int> textureHandle;

	// �g�p����}�e���A�����ƃ}�e���A���t�@�C�����̗������擾������}�e���A���t�@�C����ǂݍ��ނ悤�ɂ��邽�߂ɕϐ��B
	bool isLoadMaterialFile = false;
	bool isLoadMaterialName = false;
	bool isLoadTexture = false;

	// �}�e���A���̏��B
	string materialName;
	string materialFile;

	// �ŏ���"o"�𖳌������邽�߂̃t���O�B
	bool isFirst = true;

	// �s�����t���O���������B
	isOpaque = true;

	// ���݂̂�GI��K�������邽�߂̃t���O�B
	isFloor = false;

	while (std::getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������B
		std::istringstream lineStream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪�������擾�B
		string key;
		std::getline(lineStream, key, ' ');

		// �擪������v�Ȃ璸�_���W�B
		if (key == "v") {

			// X,Y,Z���W�ǂݍ��݁B
			Vec3 pos{};
			lineStream >> pos.x;
			lineStream >> pos.y;
			lineStream >> pos.z;

			// ���W����U�ۑ��B
			position.emplace_back(pos);

		}
		// �擪������vt�Ȃ�e�N�X�`���B
		if (key == "vt") {

			// UV�����ǂݍ��݁B
			DirectX::XMFLOAT2 texcoord{};
			lineStream >> texcoord.x;
			lineStream >> texcoord.y;

			// V�������]�B
			texcoord.y = 1.0f - texcoord.y;

			// �e�N�X�`�����W�f�[�^�ɒǉ��B
			uv.emplace_back(texcoord);

		}
		// �擪������vn�Ȃ�@���x�N�g���B
		if (key == "vn") {

			// X,Y,Z�����ǂݍ��݁B
			Vec3 norm{};
			lineStream >> norm.x;
			lineStream >> norm.y;
			lineStream >> norm.z;

			// �@���x�N�g���f�[�^�ɒǉ��B
			normal.emplace_back(norm);

		}
		// �擪������f�Ȃ�|���S��(�O�p�`)�B
		if (key == "f") {

			// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ��ށB
			string indexString;

			while (getline(lineStream, indexString, ' ')) {

				// ���_�C���f�b�N�X����̕�������X�g���[���ɕϊ����ĉ�͂��₷������B
				std::istringstream indexStream(indexString);

				unsigned int indexPosition = 0;		// �󂯎M
				unsigned int indexNormal = 0;		// �󂯎M
				unsigned int indexTexcoord = 0;		// �󂯎M

				indexStream >> indexPosition;
				indexStream.seekg(1, ios_base::cur);	// �X���b�V�����΂��B
				indexStream >> indexTexcoord;
				indexStream.seekg(1, ios_base::cur);	// �X���b�V�����΂��B
				indexStream >> indexNormal;

				// ���_�f�[�^�̒ǉ��B
				ModelDataManager::Vertex vert{};
				vert.pos = position[indexPosition - 1].ConvertXMFLOAT3();
				vert.normal = normal[indexNormal - 1].ConvertXMFLOAT3();
				vert.uv = uv[indexTexcoord - 1];

				// BLAS�̃f�[�^�ɒǉ��B
				blasData.vertex.emplace_back(vert);
				blasData.index.emplace_back(blasData.index.size());

			}
		}

		// �擪������mtllib�Ȃ�}�e���A���t�@�C���B
		if (key == "mtllib") {

			// �}�e���A���̃t�@�C�����ǂݍ��݁B
			lineStream >> materialFile;

			isLoadMaterialFile = true;

		}

		// �擪������usemtl�Ȃ�}�e���A���B
		if (key == "usemtl") {

			// �}�e���A�����ǂݍ��݁B
			lineStream >> materialName;

			isLoadMaterialName = true;

		}

		// �擪������o��������BLAS�𐶐�����B
		if (key == "o") {

			// �ŏ��̈��͖�������B
			if (isFirst) {

				isFirst = false;

			}
			else {

				// BLAS�𐶐�����B
				int blasIDBuff = BLASRegister::Ins()->GenerateData(blasData, HitGroupName, textureHandle, isOpaque);
				std::pair<std::vector<int>, int> buff = { textureHandle,blasIDBuff };
				blasID.emplace_back(buff);

				// �ۑ�����Ă���BLASID�ŃC���X�^���X�𐶐�����B
				int idBuff = PorygonInstanceRegister::Ins()->CreateInstance(blasIDBuff, isFloor ? 10 : 0);
				InstanceID.emplace_back(idBuff);

				// ���̑��f�[�^������������B
				blasData.index.clear();
				blasData.vertex.clear();
				isLoadTexture = false;
				isLoadMaterialName = false;
				textureHandle.clear();
				textureHandle.shrink_to_fit();

				isFloor = false;

				isOpaque = true;

			}

		}

		// �}�e���A���t�@�C�����ƃ}�e���A�����̂ǂ���Ƃ��擾�ł�����A�e�N�X�`�������[�h����B
		if ((!isLoadTexture) && isLoadMaterialFile && isLoadMaterialName) {

			// �e�N�X�`����ǂݍ��ށB
			LoadMaterial(DirectryPath, materialFile, materialName, textureHandle);

			// �e�N�X�`�������[�h�ς݂ɂ���B
			isLoadTexture = true;

		}

	}

	// ��ԍŌ��BLAS�𐶐��B
	int blasIDBuff = BLASRegister::Ins()->GenerateData(blasData, HitGroupName, textureHandle, isOpaque);
	std::pair<std::vector<int>, int> buff = { textureHandle,blasIDBuff };
	blasID.emplace_back(buff);

	// �ۑ�����Ă���BLASID�ŃC���X�^���X�𐶐�����B
	int idBuff = PorygonInstanceRegister::Ins()->CreateInstance(blasIDBuff, 2);
	InstanceID.emplace_back(idBuff);

	// �t�@�C�������B
	file.close();

	return InstanceID;

}

void MultiMeshLoadOBJ::LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName, std::vector<int>& TextureHandle)
{

	// �t�@�C���X�g���[���B
	ifstream file;

	// �}�e���A���t�@�C�����J���B
	file.open(DirectryPath + MaterialFileName);

	// �t�@�C���I�[�v�����s���`�F�b�N�B
	if (file.fail()) {

		assert(0);

	}

	// ��s���ǂݍ��ށB
	string line;

	// �g�p����}�e���A���̂Ƃ���ɒB�������B
	bool isLoadTexture = false;

	// �ړI�̃}�e���A���ɂ��ǂ蒅�����ۂɁA������x"newmtl"�ɂ��ǂ蒅�����珈�����I���邽�߂̃t���O�B
	bool isEnd = false;

	while (getline(file, line)) {

		// �ʒu�s���̕�������X�g���[���ɕϊ��B
		istringstream lineStream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾�B
		string key;
		getline(lineStream, key, ' ');

		// �擪�̃^�u�����͖�������B
		if (key[0] == '\t') {

			key.erase(key.begin());	//�擪�̕������폜

		}

		// �擪������newmtl�Ȃ�}�e���A�����B
		if (key == "newmtl") {

			// ���Ƀ}�e���A������ǂݍ��ݍς݂������珈�����I����B
			if (isEnd) {

				break;

			}

			// �}�e���A�����̓ǂݍ��݁B
			string materialNameBuff;
			lineStream >> materialNameBuff;

			// ���݂̃}�e���A�����ƈ����̃}�e���A������������������B
			if (MaterialName == materialNameBuff) {

				isLoadTexture = true;

				// ���Ƀ}�e���A������ǂݍ��񂾂珈�����I����悤�ɂ���B
				isEnd = true;

			}

		}

		// �g�p����e�N�X�`���̂Ƃ���ɒB���Ă�����B
		if (isLoadTexture) {

			// �擪������map_Kd�Ȃ�e�N�X�`�����B
			if (key == "map_Kd") {

				// �e�N�X�`������ۑ��B
				string textureNameBuff;
				lineStream >> textureNameBuff;

				// �e�N�X�`������ϊ��B
				wstring buff = StringToWString(DirectryPath + textureNameBuff);

				// ���ɐ����ς݂����`�F�b�N����B
				const int TEXPATH_COUNT = static_cast<int>(texturePath.size());
				bool isLoad = false;
				for (int index = 0; index < TEXPATH_COUNT; ++index) {

					if (buff == texturePath[index]) {

						isLoad = true;

						// ���e�N�X�`����������s�����t���O��܂�B
						if (textureNameBuff == "sponzaTextures/vase_plant.png") {

							isOpaque = false;

						}

						// ���e�N�X�`����������GI��܂�B
						if (textureNameBuff == "sponzaTextures/sponza_floor_a_diff.png") {

							isFloor = true;

						}

						// �e�N�X�`����ǂݍ��ށB
						TextureHandle.emplace_back(TextureManager::Ins()->LoadTextureInDescriptorHeapMgr(texturePath[index].c_str()));


					}

				}

				// ���[�h���Ă��Ȃ�������B
				if (!isLoad) {

					texturePath.emplace_back();
					texturePath[texturePath.size() - 1] = buff;

					// �e�N�X�`����ǂݍ��ށB
					TextureHandle.emplace_back(TextureManager::Ins()->LoadTextureInDescriptorHeapMgr(texturePath[texturePath.size() - 1].c_str()));

					// ���e�N�X�`����������s�����t���O��܂�B
					if (textureNameBuff == "sponzaTextures/vase_plant.png") {

						isOpaque = false;

					}
					// ���e�N�X�`����������GI��܂�B
					if (textureNameBuff == "sponzaTextures/sponza_floor_a_diff.png") {

						isFloor = true;

					}

				}

			}

			// �擪������map_Disp�Ȃ�@���}�b�v�B
			//if (key == "map_Disp") {

			//	// �@���e�N�X�`������ۑ��B
			//	string textureNameBuff;
			//	lineStream >> textureNameBuff;

			//	// �@���e�N�X�`������ϊ��B
			//	wstring buff = StringToWString(DirectryPath + textureNameBuff);

			//	// ���ɐ����ς݂����`�F�b�N����B
			//	const int TEXPATH_COUNT = texturePath.size();
			//	bool isLoad = false;
			//	for (int index = 0; index < TEXPATH_COUNT; ++index) {

			//		if (buff == texturePath[index]) {

			//			isLoad = true;

			//			// �e�N�X�`����ǂݍ��ށB
			//			TextureHandle.emplace_back(TextureManager::Ins()->LoadTextureInDescriptorHeapMgr(texturePath[index].c_str()));


			//		}

			//	}

			//	// ���[�h���Ă��Ȃ�������B
			//	if (!isLoad) {

			//		texturePath.emplace_back();
			//		texturePath[texturePath.size() - 1] = buff;

			//		// �e�N�X�`����ǂݍ��ށB
			//		TextureHandle.emplace_back(TextureManager::Ins()->LoadTextureInDescriptorHeapMgr(texturePath[texturePath.size() - 1].c_str()));

			//	}

			//}

		}

	}

}

std::wstring MultiMeshLoadOBJ::StringToWString(std::string oString)
{
	auto num1 = MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		oString.c_str(), -1, nullptr, 0);

	std::wstring wstr;
	wstr.resize(num1);

	auto num2 = MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		oString.c_str(), -1, &wstr[0], num1);

	// �ϊ����ʂ�Ԃ�
	return(wstr);
}
