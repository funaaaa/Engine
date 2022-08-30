#include "MugenStage.h"
#include "StageObjectMgr.h"
#include "HitGroupMgr.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include "ConvertGimmickInfoFromBlender.h"

void MugenStage::Setting(const int& TireMaskIndex)
{

	/*===== セッティング処理 =====*/

	// ステージオブジェクト管理クラスをセット。
	stageObjectMgr_ = std::make_shared<StageObjectMgr>();

	// ステージをセット。
	int indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStage.obj", { L"Resource/Game/Stage/MugenStage.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI_TIREMASK);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// ステージにタイヤ痕用マスクをセット。
	BLASRegister::Ins()->GetBLAS()[stageObjectMgr_->GetBlasIndex(indexBuff)]->AddUAVTex(TireMaskIndex);

	// ステージの坂セット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageSlope.obj", { L"Resource/Game/Stage/MugenStage.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// ステージの草をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageGrass1.obj", { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// ステージの草をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageGrass2.obj", { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// ステージの山をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/", "MugenStageOrnament.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));


	// 段差加速ギミックをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(697, 56, -1768));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(815, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(962, 56, -2551));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(815, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1399, 80, -3225));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(698, 338, 600));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1854, 104, -4358));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.376f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(698, 338, 600));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(2883, 56, -6397));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(285, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(2883, 56, -8095));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(285, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(3585, 56, -9240));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(575, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(8364, -612, -6995));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(206, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(7097, -914, -5374));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(206, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(5134, -1194, -5050));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(206, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(3258, -1228, -3264));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -1.237, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(206, 295, 431));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(380, -1228, -2275));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -1.237, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(206, 295, 431));

	// 加速ギミックをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1861, 47, -4395));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, -0.395, 0));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(606, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(3593, 31, -9243));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, -0.395, 0));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(577, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(4940, 119, -12816));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.057, -0.663, -0.404));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(281, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(6732, 194, -13264));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.114, -1.724, -0.465));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(318, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(8406, 192, -12935));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.081, -1.993, -0.536));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(430, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(10002, 225, -11907));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.304, -2.160, -0.667));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(430, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(10975, 211, -10432));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.624, -2.603, -0.731));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(348, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(350, -1269, -2276));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-1.571, -3.486, -0.908));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(188, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-6632, 21, 838));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-2.508, -3.171, -1.001));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(217, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-7000, 253, 2231));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.187, -2.794, -0.836));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(309, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-6587, 213, 3629));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.262, -2.284, -0.708));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(309, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-5500, 211, 4613));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.711, -2.655, -0.880));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(309, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-4136, 235, 5001));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.553, -2.066, -0.732));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(377, 200, 200));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-2450, 221, 4659));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.718, -1.312, -0.742));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(344, 200, 200));


	// アイテムボックス
	ConvertGimmickInfoFromBlender itemBoxData;
	itemBoxData.Convert("Resource/Game/Stage/", "MugenStageItemBox.obj", DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), DirectX::XMMatrixScaling(200, 200, 200));

	int itemBoxCount = itemBoxData.GetVertexSize();
	for (int index = 0; index < itemBoxCount; ++index) {

		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
			"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
		stageObjectMgr_->ChangeTrans(indexBuff, itemBoxData.GetVertex(index));
		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
		stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	}

	// ゴールをセット。
	goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// ゴールのパラメーターを設定。
	stageObjectMgr_->AddScale(goalInsIndex, Vec3(622, 200.0f, 200.0f));
	stageObjectMgr_->ChangeRotate(goalInsIndex, Vec3(0, -0.499, 0));
	stageObjectMgr_->AddTrans(goalInsIndex, Vec3(-81, -30, 263));

	// 中間地点をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "middlePointCollision.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// 中間地点を設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	stageObjectMgr_->AddTrans(indexBuff, Vec3(23, -1210, -2278));

	// ステージの壁をセット。
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::WALL, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/Stage/", "MugenStageWall.obj", { L"Resource/Game/wayGray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	//dstageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	//// ゴールをセット。
	//goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	//// ゴールのパラメーターを設定。
	//stageObjectMgr_->AddScale(goalInsIndex, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->AddTrans(goalInsIndex, Vec3(10, -30, 0));

	//// 中間地点をセット。
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "middlePointCollision.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	//// 中間地点を設定。
	//stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->AddTrans(indexBuff, Vec3(-8000, 1000, 2800));

	//// 装飾オブジェクトをセット。
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/stageOrnament/", "blockA.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFLECTION);
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(32.5f, 66.0f, -2359.5f));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/stageOrnament/", "blockB.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFLECTION);
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1870.0f, 133.5f, -2410.0f));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/stageOrnament/", "blockC.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-3550.5, 71.5f, -955.5f));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/stageOrnament/", "blockD.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-2038.0f, 21.0f, 2865.0f));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/stageOrnament/", "blockE.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1702.0f, 145.5f, 4905.0f));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/stageOrnament/", "blockF.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1587.5f, 80.0f, 6646.0f));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
	//	"Resource/Game/stageOrnament/", "goalSideObj.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	//// アイテムボックステスト用
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1871.5f, 48.0f, -2793.0f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1871.5f, 48.0f, -2893.0f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1871.5f, 48.0f, -2693.0f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-7607.0f, 589.0f, 2801.0f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, -0.184));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-7607.0f, 589.0f, 2951.0f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, -0.184));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-7607.0f, 589.0f, 2651.0f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, -0.184));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1305.0f, 48.0f, 6326.5f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1305.0f, 48.0f, 6476.5f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1305.0f, 48.0f, 6186.5f));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));



	//// 加速ギミックをセット。
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));

	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-80, -15, 3000));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(100, -15, 4000));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-5842, -29, -167));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 1.48f, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(200, 200, 200));
	//indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
	//	"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-2352, -18, 6336));
	//stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 1.58f, 0));
	//stageObjectMgr_->ChangeScale(indexBuff, Vec3(190, 200, 200));

	//// ゴール前のふわふわオブジェクトをセット。
	//for (int index = 0; index < 4; ++index) {
	//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
	//		"Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(250, 200, 1000 * index + 1500.0f));
	//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(40.0f, 40.0f, 40.0f));
	//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
	//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
	//		"Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-250, 200, 1000 * index + 1500.0f));
	//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(40.0f, 40.0f, 40.0f));
	//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
	//}

	// 各変数を初期化。
	timer_ = 0;

}

void MugenStage::Destroy()
{

	/*===== 破棄処理 =====*/

}

void MugenStage::Update()
{

	/*===== 更新処理 =====*/

	++timer_;

	stageObjectMgr_->Update(timer_);

}

BaseStage::ColliderOutput MugenStage::Collider(BaseStage::ColliderInput Input)
{

	/*===== 当たり判定 =====*/

	BaseStage::ColliderOutput output;

	output = stageObjectMgr_->Collider(Input);

	return output;

}

void MugenStage::DisplayGoal()
{

	/*===== ゴールの表示 =====*/

	//PolygonInstanceRegister::Ins()->Display(goalInsIndex);

}

void MugenStage::NonDisplayGoal()
{

	/*===== ゴールの非表示 =====*/

	//PolygonInstanceRegister::Ins()->NonDisplay(goalInsIndex);

}
