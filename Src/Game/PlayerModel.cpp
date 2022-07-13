#include "PlayerModel.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Vec.h"
#include "HitGroupMgr.h"

void PlayerModel::Load()
{

	/*===== ���f�������[�h =====*/

	// �Ԃ̃{�f�B�����[�h
	carBodyBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }, true);
	carBodyInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlasIndex, PolygonInstanceRegister::SHADER_ID::REFLECTION);
	PolygonInstanceRegister::Ins()->AddScale(carBodyInsIndex, Vec3(25, 25, 25));

	// �Ԃ̃o���p�[�����[�h
	carBumperBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBumper.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carBumperInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBumperBlasIndex, PolygonInstanceRegister::SHADER_ID::REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carBumperInsIndex, carBodyInsIndex);

	// �Ԃ̃��C�g�����[�h
	carLightBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLight.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/white.png" });
	carLightInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carLightBlasIndex, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	PolygonInstanceRegister::Ins()->SetParentInstance(carLightInsIndex, carBodyInsIndex);

	// �Ԃ̌��^�C�������[�h
	carBehindTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carBehindTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carBehindTireInsIndex, carBodyInsIndex);

	// �Ԃ̌��^�C���̃t���[�������[�h
	carBehindTireFrameBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBehindTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carBehindTireFrameInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBehindTireFrameBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carBehindTireFrameInsIndex, carBodyInsIndex);

	// �E�O�^�C�������[�h
	carRightTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carRightTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carRightTireInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carRightTireInsIndex, Vec3(0.84f, -0.87f, -1.55f));

	// �E�O�^�C���̃t���[�������[�h
	carRightTireFrameBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carRightTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carRightTireFrameInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carRightTireFrameBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carRightTireFrameInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carRightTireFrameInsIndex, Vec3(0.87f, -0.32f, -1.55f));

	// ���O�^�C�������[�h
	carLeftTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carLeftTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carLeftTireInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carLeftTireInsIndex, Vec3(-0.94f, -0.86f, -1.57f));

	// ���O�^�C���̃t���[�������[�h
	carLeftTireFrameBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carLeftTireFrame.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	carLeftTireFrameInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carLeftTireFrameBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carLeftTireFrameInsIndex, carBodyInsIndex);
	PolygonInstanceRegister::Ins()->AddTrans(carLeftTireFrameInsIndex, Vec3(-0.91f, -0.3f, -1.53f));

	// �Ԃ̋������[�h
	carMirrorBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirror.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorInsIndex, carBodyInsIndex);

	// �Ԃ̋��̃J�o�[�����[�h
	carMirrorCoverBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirrorCover.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorCoverInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorCoverInsIndex, carBodyInsIndex);

	// �Ԃ̃E�B���h�E�����[�h
	carWindowBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/windowColor.png" });
	carWindowInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carWindowInsIndex, carBodyInsIndex);

}
