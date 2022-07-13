#include "PlayerModel.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Vec.h"
#include "HitGroupMgr.h"

void PlayerModel::Load()
{

	/*===== モデルをロード =====*/

	// 車のボディをロード
	carBodyBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }, true);
	carBodyInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlasIndex, PolygonInstanceRegister::SHADER_ID::REFLECTION);
	PolygonInstanceRegister::Ins()->AddScale(carBodyInsIndex, Vec3(25, 25, 25));

	// 車のバンパーをロード
	carBumperBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBumper.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carBumperInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlasIndex, PolygonInstanceRegister::SHADER_ID::REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carBumperInsIndex, carBodyInsIndex);

	// 車のライトをロード
	carLightBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/white.png" });
	carLightInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carLightBlasIndex, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	PolygonInstanceRegister::Ins()->SetParentInstance(carLightInsIndex, carBodyInsIndex);

	// 車の後ろタイヤをロード
	carBehindTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carBehindTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carBehindTireInsIndex, carBodyInsIndex);

	// 車の後ろタイヤのフレームをロード
	carBehindTireFrameBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carBehindTireFrameInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carBehindTireFrameInsIndex, carBodyInsIndex);

	// 右前タイヤをロード
	carRightTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carRightTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carRightTireInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carRightTireInsIndex, Vec3(0.84f, -0.87f, -1.55f));

	// 右前タイヤのフレームをロード
	carRightTireFrameBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carRightTireFrameInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carRightTireFrameInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carRightTireFrameInsIndex, Vec3(0.87f, -0.32f, -1.55f));

	// 左前タイヤをロード
	carLeftTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carLeftTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carLeftTireInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carLeftTireInsIndex, Vec3(-0.94f, -0.86f, -1.57f));

	// 左前タイヤのフレームをロード
	carLeftTireFrameBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carLeftTireFrameInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carLeftTireFrameInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carLeftTireFrameInsIndex, Vec3(-0.91f, -0.3f, -1.53f));

	// 車の鏡をロード
	carMirrorBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirror.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorInsIndex, carBodyInsIndex);

	// 車の鏡のカバーをロード
	carMirrorCoverBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirrorCover.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorCoverInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorCoverInsIndex, carBodyInsIndex);

	// 車のウィンドウをロード
	carWindowBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/windowColor.png" });
	carWindowInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carWindowInsIndex, carBodyInsIndex);

}
