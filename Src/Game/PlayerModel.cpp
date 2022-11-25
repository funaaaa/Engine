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

	PolygonInstanceRegister::SHADER_ID shaderID = PolygonInstanceRegister::SHADER_ID::DEF;
	switch (Chara)
	{
	case PlayerModel::CHARA::PLAYER:
		shaderID = PolygonInstanceRegister::SHADER_ID::DEF;
		break;
	case PlayerModel::CHARA::GHOST:
		shaderID = PolygonInstanceRegister::SHADER_ID::DEF;
		break;
	case PlayerModel::CHARA::AI:
		shaderID = PolygonInstanceRegister::SHADER_ID::DEF;
		break;
	default:
		shaderID = PolygonInstanceRegister::SHADER_ID::DEF;
		break;
	}

	// �Ԃ̃{�f�B�����[�h
	carBodyBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"body.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBodyBlas_.lock()->GetMaterial().roughness_ = 0.8f;
	carBodyBlas_.lock()->IsChangeMaterial();
	carBodyInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlas_, shaderID);
	carBodyInstance_.lock()->AddScale(Vec3(25, 25, 25));

	// �Ԃ̃o���p�[�����[�h
	carBumperBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"bumper.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBumperBlas_.lock()->GetMaterial().roughness_ = 1.0f;
	carBumperBlas_.lock()->IsChangeMaterial();
	carBumperInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlas_, shaderID);
	carBumperInstance_.lock()->SetParentInstance(carBodyInstance_);

	// �Ԃ̃��C�g�����[�h
	carLeftLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "leftLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftLightInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carLeftLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carLeftLightInstance_.lock()->SetParentInstance(carBodyInstance_);

	// �Ԃ̃��C�g�����[�h
	carRightLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "rightLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightLightInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carRightLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carRightLightInstance_.lock()->SetParentInstance(carBodyInstance_);

	// �Ԃ̃��C�g�����[�h
	carFrontLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "frontLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carFrontLightInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carFrontLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carFrontLightInstance_.lock()->SetParentInstance(carBodyInstance_);

	// �Ԃ̌��^�C�������[�h
	carBehindTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBehindTireInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlas_, shaderID);
	carBehindTireInstance_.lock()->SetParentInstance(carBodyInstance_);
	carBehindTireInstance_.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

	// �Ԃ̌��^�C���̃t���[�������[�h
	carBehindTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBehindTireFrameInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlas_, shaderID);
	carBehindTireFrameInstance_.lock()->SetParentInstance(carBodyInstance_);
	carBehindTireFrameInstance_.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

	// �E�O�^�C�������[�h
	carRightTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightTireInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlas_, shaderID);
	carRightTireInstance_.lock()->SetParentInstance(carBodyInstance_);
	carRightTireInstance_.lock()->AddTrans(Vec3(0.84f, -0.87f, -1.55f));

	// �E�O�^�C���̃t���[�������[�h
	carRightTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightTireFrameInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlas_, shaderID);
	carRightTireFrameInstance_.lock()->SetParentInstance(carBodyInstance_);
	carRightTireFrameInstance_.lock()->AddTrans(Vec3(0.85f, -0.86f, -1.55f));

	// ���O�^�C�������[�h
	carLeftTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftTireInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlas_, shaderID);
	carLeftTireInstance_.lock()->SetParentInstance(carBodyInstance_);
	carLeftTireInstance_.lock()->AddTrans(Vec3(-0.94f, -0.86f, -1.57f));

	// ���O�^�C���̃t���[�������[�h
	carLeftTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftTireFrameInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlas_, shaderID);
	carLeftTireFrameInstance_.lock()->SetParentInstance(carBodyInstance_);
	carLeftTireFrameInstance_.lock()->AddTrans(Vec3(-0.95f, -0.85f, -1.55f));

	// �Ԃ̋������[�h
	carMirrorBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirror.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carMirrorInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, shaderID);
	carMirrorInstance_.lock()->SetParentInstance(carBodyInstance_);

	// �Ԃ̋��̃J�o�[�����[�h
	carMirrorCoverBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirrorCover.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carMirrorCoverInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, shaderID);
	carMirrorCoverInstance_.lock()->SetParentInstance(carBodyInstance_);

	// �Ԃ̃E�B���h�E�����[�h
	carWindowBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"window.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carWindowInsInstance_ = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlas_, shaderID);
	carWindowInsInstance_.lock()->SetParentInstance(carBodyInstance_);


	bodyInstance_.clear();
	tireInstance_.clear();
	carBlasIndex_.clear();


	// �ԑ̂̃C���f�b�N�X��ۑ��B
	bodyInstance_.emplace_back(carBumperInstance_);
	bodyInstance_.emplace_back(carLeftLightInstance_);
	bodyInstance_.emplace_back(carRightLightInstance_);
	bodyInstance_.emplace_back(carFrontLightInstance_);
	bodyInstance_.emplace_back(carMirrorInstance_);
	bodyInstance_.emplace_back(carMirrorCoverInstance_);
	bodyInstance_.emplace_back(carWindowInsInstance_);
	bodyInstance_.emplace_back(carBodyInstance_);

	// �^�C���̃C���f�b�N�X��ۑ��B
	tireInstance_.emplace_back(carBehindTireInstance_);
	tireInstance_.emplace_back(carBehindTireFrameInstance_);
	tireInstance_.emplace_back(carRightTireInstance_);
	tireInstance_.emplace_back(carRightTireFrameInstance_);
	tireInstance_.emplace_back(carLeftTireInstance_);
	tireInstance_.emplace_back(carLeftTireFrameInstance_);

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

	for (auto& index : tireInstance_) {

		PolygonInstanceRegister::Ins()->DestroyInstance(index);

	}
	for (auto& index : bodyInstance_) {

		PolygonInstanceRegister::Ins()->DestroyInstance(index);

	}

	for (auto& index : carBlasIndex_) {

		index.lock()->Init();

	}

}
