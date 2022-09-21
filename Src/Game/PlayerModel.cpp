#include "PlayerModel.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "Vec.h"
#include "HitGroupMgr.h"

void PlayerModel::Load(COLOR ColorID, bool IsGhost)
{

	/*===== モデルをロード =====*/

	LPCWSTR texPath = L"Resource/Game/red.png";

	switch (ColorID)
	{
	case PlayerModel::COLOR::RED:

		texPath = L"Resource/Game/red.png";

		break;
	case PlayerModel::COLOR::BLUE:

		texPath = L"Resource/Game/blue.png";

		break;
	default:

		break;
	}

	if (IsGhost) {

		// 車のボディをロード
		carBodyBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, true, true, true);
		carBodyInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBodyInstance.lock()->AddScale(Vec3(25, 25, 25));

		// 車のバンパーをロード
		carBumperBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBumper.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carBumperInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBumperInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carLeftLightBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/white.png" }, false, true, true);
		carLeftLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftLightBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carLeftLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carRightLightBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/white.png" }, false, true, true);
		carRightLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightLightBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carRightLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carFrontLightBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carFrontLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/white.png" }, false, true, true);
		carFrontLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carFrontLightBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carFrontLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車の後ろタイヤをロード
		carBehindTireBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carBehindTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBehindTireInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 車の後ろタイヤのフレームをロード
		carBehindTireFrameBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carBehindTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carBehindTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireFrameInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 右前タイヤをロード
		carRightTireBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carRightTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carRightTireInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireInstance.lock()->AddTrans(Vec3(0.84f, -0.87f, -1.55f));

		// 右前タイヤのフレームをロード
		carRightTireFrameBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carRightTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carRightTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireFrameInstance.lock()->AddTrans(Vec3(0.85f, -0.86f, -1.55f));

		// 左前タイヤをロード
		carLeftTireBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carLeftTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carLeftTireInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireInstance.lock()->AddTrans(Vec3(-0.94f, -0.86f, -1.57f));

		// 左前タイヤのフレームをロード
		carLeftTireFrameBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carLeftTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carLeftTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireFrameInstance.lock()->AddTrans(Vec3(-0.95f, -0.85f, -1.55f));

		// 車の鏡をロード
		carMirrorBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirror.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carMirrorInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carMirrorInstance.lock()->SetParentInstance(carBodyInstance);

		// 車の鏡のカバーをロード
		carMirrorCoverBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirrorCover.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carMirrorCoverInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carMirrorCoverInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のウィンドウをロード
		carWindowBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/windowColor.png" }, false, true, true);
		carWindowInsInstance = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFRACTION);
		carWindowInsInstance.lock()->SetParentInstance(carBodyInstance);

	}
	else {

		// 車のボディをロード
		carBodyBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, true, true, true);
		carBodyInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFLECTION);
		carBodyInstance.lock()->AddScale(Vec3(25, 25, 25));

		// 車のバンパーをロード
		carBumperBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBumper.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carBumperInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlasIndex_, PolygonInstanceRegister::SHADER_ID::REFLECTION);
		carBumperInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carLeftLightBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/white.png" }, false, true, true);
		carLeftLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftLightBlasIndex_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
		carLeftLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carRightLightBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/white.png" }, false, true, true);
		carRightLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightLightBlasIndex_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
		carRightLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のライトをロード
		carFrontLightBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carFrontLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/white.png" }, false, true, true);
		carFrontLightInstance = PolygonInstanceRegister::Ins()->CreateInstance(carFrontLightBlasIndex_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
		carFrontLightInstance.lock()->SetParentInstance(carBodyInstance);

		// 車の後ろタイヤをロード
		carBehindTireBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carBehindTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
		carBehindTireInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 車の後ろタイヤのフレームをロード
		carBehindTireFrameBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carBehindTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
		carBehindTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carBehindTireFrameInstance.lock()->AddTrans(Vec3(-0.089f, -0.882f, 1.101f));

		// 右前タイヤをロード
		carRightTireBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carRightTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
		carRightTireInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireInstance.lock()->AddTrans(Vec3(0.84f, -0.87f, -1.55f));

		// 右前タイヤのフレームをロード
		carRightTireFrameBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carRightTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
		carRightTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carRightTireFrameInstance.lock()->AddTrans(Vec3(0.85f, -0.86f, -1.55f));

		// 左前タイヤをロード
		carLeftTireBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carLeftTireInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
		carLeftTireInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireInstance.lock()->AddTrans(Vec3(-0.94f, -0.86f, -1.57f));

		// 左前タイヤのフレームをロード
		carLeftTireFrameBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { texPath }, false, true, true);
		carLeftTireFrameInstance = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
		carLeftTireFrameInstance.lock()->SetParentInstance(carBodyInstance);
		carLeftTireFrameInstance.lock()->AddTrans(Vec3(-0.95f, -0.85f, -1.55f));

		// 車の鏡をロード
		carMirrorBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirror.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carMirrorInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex_, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
		carMirrorInstance.lock()->SetParentInstance(carBodyInstance);

		// 車の鏡のカバーをロード
		carMirrorCoverBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirrorCover.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/tireColor.png" }, true, true, true);
		carMirrorCoverInstance = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex_, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
		carMirrorCoverInstance.lock()->SetParentInstance(carBodyInstance);

		// 車のウィンドウをロード
		carWindowBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/car/windowColor.png" }, false, true, true);
		carWindowInsInstance = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
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
	carBlasIndex_.emplace_back(carBumperBlasIndex_);
	carBlasIndex_.emplace_back(carLeftLightBlasIndex_);
	carBlasIndex_.emplace_back(carRightLightBlasIndex_);
	carBlasIndex_.emplace_back(carFrontLightBlasIndex_);
	carBlasIndex_.emplace_back(carMirrorBlasIndex_);
	carBlasIndex_.emplace_back(carMirrorCoverBlasIndex_);
	carBlasIndex_.emplace_back(carWindowBlasIndex_);
	carBlasIndex_.emplace_back(carBodyBlasIndex_);
	carBlasIndex_.emplace_back(carBehindTireBlasIndex_);
	carBlasIndex_.emplace_back(carBehindTireFrameBlasIndex_);
	carBlasIndex_.emplace_back(carRightTireBlasIndex_);
	carBlasIndex_.emplace_back(carRightTireFrameBlasIndex_);
	carBlasIndex_.emplace_back(carLeftTireBlasIndex_);
	carBlasIndex_.emplace_back(carLeftTireFrameBlasIndex_);
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

		BLASRegister::Ins()->DeleteIndex(index);

	}

}
