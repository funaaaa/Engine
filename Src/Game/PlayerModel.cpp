#include "PlayerModel.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "Vec.h"
#include "HitGroupMgr.h"
#include "TextureManager.h"

void PlayerModel::Load(COLOR ColorID, CHARA Chara)
{

	/*===== ���f�������[�h =====*/

	// �g�p����f�B���N�g��
	std::wstring folder;
	std::string folderStr;

	switch (ColorID)
	{
	case PlayerModel::COLOR::RED:

		folder = L"Red/";
		folderStr = "Red/";

		break;
	case PlayerModel::COLOR::BLUE:

		folder = L"Blue/";
		folderStr = "Blue/";

		break;
	default:
		break;
	}

	PolygonInstanceRegister::SHADER_ID shaderID;
	switch (Chara)
	{
	case PlayerModel::CHARA::PLAYER:
		shaderID = PolygonInstanceRegister::SHADER_ID::DEF;
		break;
	case PlayerModel::CHARA::GHOST:
		shaderID = PolygonInstanceRegister::SHADER_ID::REFRACTION;
		break;
	case PlayerModel::CHARA::AI:
		shaderID = PolygonInstanceRegister::SHADER_ID::DEF;
		break;
	default:
		break;
	}

	// �Ԃ̃{�f�B�����[�h
	carBodyBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"body.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBodyInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlas_, shaderID);
	carBodyInstance.lock()->AddScale(Vec3(25, 25, 25));

	// �Ԃ̃o���p�[�����[�h
	carBumperBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"bumper.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBumperInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlas_, shaderID);
	carBumperInstance.lock()->SetParentInstance(carBodyInstance);

	// �Ԃ̃��C�g�����[�h
	carLeftLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "leftLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carLeftLightInstance.lock()->SetParentInstance(carBodyInstance);

	// �Ԃ̃��C�g�����[�h
	carRightLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "rightLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carRightLightInstance.lock()->SetParentInstance(carBodyInstance);

	// �Ԃ̃��C�g�����[�h
	carFrontLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "frontLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carFrontLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carFrontLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carFrontLightInstance.lock()->SetParentInstance(carBodyInstance);

	// �Ԃ̌��^�C�������[�h
	carBehindTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBehindTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlas_, shaderID);
	carBehindTireInstance.lock()->SetParentInstance(carBodyInstance);
	carBehindTireInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

	// �Ԃ̌��^�C���̃t���[�������[�h
	carBehindTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBehindTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlas_, shaderID);
	carBehindTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
	carBehindTireFrameInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

	// �E�O�^�C�������[�h
	carRightTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlas_, shaderID);
	carRightTireInstance.lock()->SetParentInstance(carBodyInstance);
	carRightTireInstance.lock()->AddTrans(Vec3(0.84f, -0.87f, -1.55f));

	// �E�O�^�C���̃t���[�������[�h
	carRightTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlas_, shaderID);
	carRightTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
	carRightTireFrameInstance.lock()->AddTrans(Vec3(0.85f, -0.86f, -1.55f));

	// ���O�^�C�������[�h
	carLeftTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlas_, shaderID);
	carLeftTireInstance.lock()->SetParentInstance(carBodyInstance);
	carLeftTireInstance.lock()->AddTrans(Vec3(-0.94f, -0.86f, -1.57f));

	// ���O�^�C���̃t���[�������[�h
	carLeftTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlas_, shaderID);
	carLeftTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
	carLeftTireFrameInstance.lock()->AddTrans(Vec3(-0.95f, -0.85f, -1.55f));

	// �Ԃ̋������[�h
	carMirrorBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirror.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carMirrorInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, shaderID);
	carMirrorInstance.lock()->SetParentInstance(carBodyInstance);

	// �Ԃ̋��̃J�o�[�����[�h
	carMirrorCoverBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirrorCover.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carMirrorCoverInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, shaderID);
	carMirrorCoverInstance.lock()->SetParentInstance(carBodyInstance);

	// �Ԃ̃E�B���h�E�����[�h
	carWindowBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"window.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carWindowInsInstance = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlas_, shaderID);
	carWindowInsInstance.lock()->SetParentInstance(carBodyInstance);


	bodyInstance.clear();
	tireInstance.clear();
	carBlasIndex_.clear();


	// �ԑ̂̃C���f�b�N�X��ۑ��B
	bodyInstance.emplace_back(carBumperInstance);
	bodyInstance.emplace_back(carLeftLightInstance);
	bodyInstance.emplace_back(carRightLightInstance);
	bodyInstance.emplace_back(carFrontLightInstance);
	bodyInstance.emplace_back(carMirrorInstance);
	bodyInstance.emplace_back(carMirrorCoverInstance);
	bodyInstance.emplace_back(carWindowInsInstance);
	bodyInstance.emplace_back(carBodyInstance);

	// �^�C���̃C���f�b�N�X��ۑ��B
	tireInstance.emplace_back(carBehindTireInstance);
	tireInstance.emplace_back(carBehindTireFrameInstance);
	tireInstance.emplace_back(carRightTireInstance);
	tireInstance.emplace_back(carRightTireFrameInstance);
	tireInstance.emplace_back(carLeftTireInstance);
	tireInstance.emplace_back(carLeftTireFrameInstance);

	// �ԑ̂̃C���f�b�N�X��ۑ��B
	carBlasIndex_.emplace_back(carBumperBlas_);
	carBlasIndex_.emplace_back(carLeftLightBlas_);
	carBlasIndex_.emplace_back(carRightLightBlas_);
	carBlasIndex_.emplace_back(carFrontLightBlas_);
	carBlasIndex_.emplace_back(carMirrorBlas_);
	carBlasIndex_.emplace_back(carMirrorCoverBlas_);
	carBlasIndex_.emplace_back(carWindowBlas_);
	carBlasIndex_.emplace_back(carBodyBlas_);
	carBlasIndex_.emplace_back(carBehindTireBlas_);
	carBlasIndex_.emplace_back(carBehindTireFrameBlas_);
	carBlasIndex_.emplace_back(carRightTireBlas_);
	carBlasIndex_.emplace_back(carRightTireFrameBlas_);
	carBlasIndex_.emplace_back(carLeftTireBlas_);
	carBlasIndex_.emplace_back(carLeftTireFrameBlas_);
}

void PlayerModel::Delete()
{

	/*===== �Ԃ̃C���X�^���X���폜 =====*/

	for (auto& index : tireInstance) {

		PolygonInstanceRegister::Ins()->DestroyInstance(index);

	}
	for (auto& index : bodyInstance) {

		PolygonInstanceRegister::Ins()->DestroyInstance(index);

	}

	for (auto& index : carBlasIndex_) {

		index.lock()->Init();

	}

}
