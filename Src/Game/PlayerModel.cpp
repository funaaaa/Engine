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

	/*===== モデルをロード =====*/

	// 使用するディレクトリ
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

	// 車のボディをロード
	carBodyBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"body.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBodyInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlas_, shaderID);
	carBodyInstance.lock()->AddScale(Vec3(25, 25, 25));

	// 車のバンパーをロード
	carBumperBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"bumper.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBumperInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlas_, shaderID);
	carBumperInstance.lock()->SetParentInstance(carBodyInstance);

	// 車のライトをロード
	carLeftLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "leftLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carLeftLightInstance.lock()->SetParentInstance(carBodyInstance);

	// 車のライトをロード
	carRightLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "rightLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carRightLightInstance.lock()->SetParentInstance(carBodyInstance);

	// 車のライトをロード
	carFrontLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/GLTF/" + folderStr, "frontLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carFrontLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carFrontLightBlas_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	carFrontLightInstance.lock()->SetParentInstance(carBodyInstance);

	// 車の後ろタイヤをロード
	carBehindTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBehindTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlas_, shaderID);
	carBehindTireInstance.lock()->SetParentInstance(carBodyInstance);
	carBehindTireInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

	// 車の後ろタイヤのフレームをロード
	carBehindTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carBehindTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlas_, shaderID);
	carBehindTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
	carBehindTireFrameInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

	// 右前タイヤをロード
	carRightTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlas_, shaderID);
	carRightTireInstance.lock()->SetParentInstance(carBodyInstance);
	carRightTireInstance.lock()->AddTrans(Vec3(0.84f, -0.87f, -1.55f));

	// 右前タイヤのフレームをロード
	carRightTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carRightTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlas_, shaderID);
	carRightTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
	carRightTireFrameInstance.lock()->AddTrans(Vec3(0.85f, -0.86f, -1.55f));

	// 左前タイヤをロード
	carLeftTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlas_, shaderID);
	carLeftTireInstance.lock()->SetParentInstance(carBodyInstance);
	carLeftTireInstance.lock()->AddTrans(Vec3(-0.94f, -0.86f, -1.57f));

	// 左前タイヤのフレームをロード
	carLeftTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carLeftTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlas_, shaderID);
	carLeftTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
	carLeftTireFrameInstance.lock()->AddTrans(Vec3(-0.95f, -0.85f, -1.55f));

	// 車の鏡をロード
	carMirrorBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirror.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carMirrorInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, shaderID);
	carMirrorInstance.lock()->SetParentInstance(carBodyInstance);

	// 車の鏡のカバーをロード
	carMirrorCoverBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirrorCover.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carMirrorCoverInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, shaderID);
	carMirrorCoverInstance.lock()->SetParentInstance(carBodyInstance);

	// 車のウィンドウをロード
	carWindowBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"window.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	carWindowInsInstance = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlas_, shaderID);
	carWindowInsInstance.lock()->SetParentInstance(carBodyInstance);


	bodyInstance.clear();
	tireInstance.clear();
	carBlasIndex_.clear();


	// 車体のインデックスを保存。
	bodyInstance.emplace_back(carBumperInstance);
	bodyInstance.emplace_back(carLeftLightInstance);
	bodyInstance.emplace_back(carRightLightInstance);
	bodyInstance.emplace_back(carFrontLightInstance);
	bodyInstance.emplace_back(carMirrorInstance);
	bodyInstance.emplace_back(carMirrorCoverInstance);
	bodyInstance.emplace_back(carWindowInsInstance);
	bodyInstance.emplace_back(carBodyInstance);

	// タイヤのインデックスを保存。
	tireInstance.emplace_back(carBehindTireInstance);
	tireInstance.emplace_back(carBehindTireFrameInstance);
	tireInstance.emplace_back(carRightTireInstance);
	tireInstance.emplace_back(carRightTireFrameInstance);
	tireInstance.emplace_back(carLeftTireInstance);
	tireInstance.emplace_back(carLeftTireFrameInstance);

	// 車体のインデックスを保存。
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

	/*===== 車のインスタンスを削除 =====*/

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
