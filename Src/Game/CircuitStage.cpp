#include "CircuitStage.h"
#include "StageObjectMgr.h"
#include "HitGroupMgr.h"
#include "PolygonInstanceRegister.h"

void CircuitStage::Setting()
{

	/*===== セッティング処理 =====*/

	// ステージオブジェクト管理クラスをセット。
	stageObjectMgr_ = std::make_shared<StageObjectMgr>();

	// ステージをセット。
	int indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/", "stage3.obj", { L"Resource/Game/white.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// ステージの草をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/", "grass.obj", { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// ゴールをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// ゴールのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	stageObjectMgr_->AddTrans(indexBuff, Vec3(10, -30, 0));

	//// 中間地点をセット。
	//indexBuff = stageObjectMgr_->AddObject(StageObjectMgr::MOVE_ID::NOMOVE, BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	//// ゴールのパラメーターを設定。
	//stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->AddTrans(indexBuff, Vec3(10, -30, 0));

	// 装飾オブジェクトをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/stageOrnament/", "blockA.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFLECTION);
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/stageOrnament/", "blockB.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFLECTION);
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/stageOrnament/", "blockC.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/stageOrnament/", "blockD.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/stageOrnament/", "blockE.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/stageOrnament/", "blockF.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// 加速ギミックをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOS_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(100, -15, 1400));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOS_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-80, -15, 3000));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOS_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(100, -15, 4000));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOS_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-5842, -29, -167));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 1.48f, 0));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200, 200, 200));
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOS_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-2352, -18, 6336));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 1.58f, 0));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(190, 200, 200));

	// ゴール前のふわふわオブジェクトをセット。
	for (int index = 0; index < 4; ++index) {
		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
			"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
		stageObjectMgr_->ChangeTrans(indexBuff, Vec3(250, 200, 1000 * index + 1500.0f));
		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
		stageObjectMgr_->ChangeScale(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
			"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
		stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-250, 200, 1000 * index + 1500.0f));
		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
		stageObjectMgr_->ChangeScale(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
	}

}

void CircuitStage::Destroy()
{

	/*===== 破棄処理 =====*/

}

void CircuitStage::Update()
{

	/*===== 更新処理 =====*/

}
