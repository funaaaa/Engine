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
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージにタイヤ痕用マスクをセット。
	BLASRegister::Ins()->GetBLAS()[stageObjectMgr_->GetBlasIndex(indexBuff)]->AddUAVTex(TireMaskIndex);

	// ステージの坂セット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageSlope.obj", { L"Resource/Game/Stage/MugenStage.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの草をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageGrass1.obj", { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの草をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageGrass2.obj", { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの山をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/", "MugenStageOrnament.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));


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
	itemBoxData.Convert("Resource/Game/Stage/", "MugenStageItemBox.obj", DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), DirectX::XMMatrixScaling(120, 120, 120));

	int itemBoxCount = itemBoxData.GetVertexSize();
	for (int index = 0; index < itemBoxCount; ++index) {

		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
			"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
		stageObjectMgr_->ChangeTrans(indexBuff, itemBoxData.GetVertex(index));
		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
		stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	}

	// 点光源
	ConvertGimmickInfoFromBlender pointLightData;
	pointLightData.Convert("Resource/Game/Stage/", "MugenStagePointLight.obj", DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), DirectX::XMMatrixScaling(120, 120, 120));

	int pointLightCount = pointLightData.GetVertexSize();
	for (int index = 0; index < pointLightCount; ++index) {

		pointLightPos.emplace_back(pointLightData.GetVertex(index));

	}

	// ゴールをセット。
	goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// ゴールのパラメーターを設定。
	stageObjectMgr_->AddScale(goalInsIndex, Vec3(622, 120.0f, 120.0f));
	stageObjectMgr_->ChangeRotate(goalInsIndex, Vec3(0, -0.499, 0));
	stageObjectMgr_->AddTrans(goalInsIndex, Vec3(-81, -30, 263));

	// 中間地点をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "middlePointCollision.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// 中間地点を設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	stageObjectMgr_->AddTrans(indexBuff, Vec3(23, -1210, -2278));

	// 見えない壁
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::WALL, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageInvWall.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// 見えない壁を設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	stageObjectMgr_->NonDisplay(indexBuff);

	// 各変数を初期化。
	timer_ = 0;

}

void MugenStage::Destroy()
{

	/*===== 破棄処理 =====*/

}

void MugenStage::Update(RayConstBufferData& ConstBufferData)
{

	/*===== 更新処理 =====*/

	++timer_;

	stageObjectMgr_->Update(timer_);

	// 点光源をセット。
	//for (auto& index : pointLightPos) {

	//	ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].isActive_ = true;
	//	ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].isShadow_ = false;
	//	ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].lightPower_ = 2000;
	//	ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].lightPos_ = index;
	//	ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].lightSize_ = 1;

	//}

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