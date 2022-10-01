#include "PlayerModel.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "Vec.h"
#include "HitGroupMgr.h"
#include "TextureManager.h"

void PlayerModel::Load(COLOR ColorID, bool IsGhost)
{

	/*===== モデルをロード =====*/

	// 使用するディレクトリ
	std::wstring folder = L"Red/";
	std::string folderStr = "Red/";
	if (IsGhost) {

		folder = L"Blue/";
		folderStr = "Blue/";

	}

	if (IsGhost) {

		// 車のボディをロード
		carBodyBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"body.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carBodyBlas_.lock()->ChangeNormalTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/GLTF/Blue/body.png"));
		carBodyInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBodyInstance.lock()->AddScale(Vec3(25, 25, 25));

		// 車のバンパーをロード
		carBumperBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"bumper.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carBumperBlas_.lock()->ChangeNormalTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/GLTF/Blue/body.png"));
		carBumperInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBumperInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carLeftLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/OBJ/" + folderStr, "leftLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carLeftLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftLightBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carLeftLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carRightLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/OBJ/" + folderStr, "rightLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carRightLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightLightBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carRightLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carFrontLightBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/OBJ/" + folderStr, "frontLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carFrontLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carFrontLightBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carFrontLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車の後ろタイヤをロード
		carBehindTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carBehindTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBehindTireInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 車の後ろタイヤのフレームをロード
		carBehindTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carBehindTireFrameBlas_.lock()->ChangeNormalTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/GLTF/Blue/body.png"));
		carBehindTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBehindTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireFrameInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 右前タイヤをロード
		carRightTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carRightTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carRightTireInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireInstance.lock()->AddTrans(Vec3(0.84f, -0.87f, -1.55f));

		// 右前タイヤのフレームをロード
		carRightTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carRightTireFrameBlas_.lock()->ChangeNormalTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/GLTF/Blue/body.png"));
		carRightTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carRightTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireFrameInstance.lock()->AddTrans(Vec3(0.85f, -0.86f, -1.55f));

		// 左前タイヤをロード
		carLeftTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carLeftTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carLeftTireInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireInstance.lock()->AddTrans(Vec3(-0.94f, -0.86f, -1.57f));

		// 左前タイヤのフレームをロード
		carLeftTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carLeftTireFrameBlas_.lock()->ChangeNormalTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/GLTF/Blue/body.png"));
		carLeftTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carLeftTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireFrameInstance.lock()->AddTrans(Vec3(-0.95f, -0.85f, -1.55f));

		// 車の鏡をロード
		carMirrorBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirror.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carMirrorInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carMirrorInstance.lock()->SetParentInstance(carBodyInstance);

		// 車の鏡のカバーをロード
		carMirrorCoverBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirrorCover.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carMirrorCoverBlas_.lock()->ChangeNormalTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/GLTF/Blue/body.png"));
		carMirrorCoverInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carMirrorCoverInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のウィンドウをロード
		carWindowBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"window.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carWindowInsInstance = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlas_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carWindowInsInstance.lock()->SetParentInstance(carBodyInstance);

	}
	else {

		// 車のボディをロード
		carBodyBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"body.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carBodyInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carBodyInstance.lock()->AddScale(Vec3(25, 25, 25));

		// 車のバンパーをロード
		carBumperBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"bumper.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carBumperInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
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
		carBehindTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carBehindTireInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 車の後ろタイヤのフレームをロード
		carBehindTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"behindTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carBehindTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carBehindTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireFrameInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 右前タイヤをロード
		carRightTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carRightTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carRightTireInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireInstance.lock()->AddTrans(Vec3(0.84f, -0.87f, -1.55f));

		// 右前タイヤのフレームをロード
		carRightTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"rightTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carRightTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carRightTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireFrameInstance.lock()->AddTrans(Vec3(0.85f, -0.86f, -1.55f));

		// 左前タイヤをロード
		carLeftTireBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTire.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carLeftTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carLeftTireInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireInstance.lock()->AddTrans(Vec3(-0.94f, -0.86f, -1.57f));

		// 左前タイヤのフレームをロード
		carLeftTireFrameBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"leftTireFrame.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carLeftTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carLeftTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireFrameInstance.lock()->AddTrans(Vec3(-0.95f, -0.85f, -1.55f));

		// 車の鏡をロード
		carMirrorBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirror.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carMirrorInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carMirrorInstance.lock()->SetParentInstance(carBodyInstance);

		// 車の鏡のカバーをロード
		carMirrorCoverBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"mirrorCover.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carMirrorCoverInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carMirrorCoverInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のウィンドウをロード
		carWindowBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Car/GLTF/" + folder + L"window.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
		carWindowInsInstance = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
		carWindowInsInstance.lock()->SetParentInstance(carBodyInstance);

	}

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
