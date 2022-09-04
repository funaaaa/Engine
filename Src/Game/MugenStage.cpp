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
	stageObjectMgr_->Setting();

	// ステージをセット。
	int indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStage.obj", { L"Resource/Game/Stage/MugenStage.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI_TIREMASK);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージにタイヤ痕用マスクをセット。
	BLASRegister::Ins()->GetBLAS()[stageObjectMgr_->GetBlasIndex(indexBuff)]->AddUAVTex(TireMaskIndex);

	// ステージの坂セット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageSlope.obj", { L"Resource/Game/slope.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
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

	// トンネルをセット。
	tunnelIndex_ = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/", "MugenStageTunnel.obj", { L"Resource/Game/wayGray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(tunnelIndex_, Vec3(120.0f, 120.0f, 120.0f));


	// 段差加速ギミックをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(572.0f, 13.0f, -1000.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(491.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(687.0f, 9.0f, -1491.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(408.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(831.0f, 23.0f, -1963.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(357.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1129.0f, 21.0f, -2609.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.376f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(382.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1734.0f, 7.0f, -3828.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(176.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1680.0f, -3.0f, -4883.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(247.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(2076.0f, 10.0f, -5535.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(423.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(5023.0f, -359.0f, -4220.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(141.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(4270.0f, -558.0f, -3216.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(141.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(3068.0f, -725.0f, -3045.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(141.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1992.0f, -748.0f, -1875.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -1.237f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(206.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(222.0f, -748.0f, -1385.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -1.237f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(146.0f, 200.0f, 200.0f));

	// 加速ギミックをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(2144.0f, -14.0f, -5519.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.0f, -0.363f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(350.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1113.0f, 18.0f, -2620.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.0f, -0.395f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(362.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(2824.0f, 57.0f, -7561.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.057f, -0.663f, -0.404f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(168.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(4144.0f, 100.0f, -7956.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.114f, -1.724f, -0.465f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(242.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(5455.0f, 107.0f, -7594.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.059f, -1.907f, -0.580f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(240.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(6326.0f, 134.0f, -6730.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.314f, -2.162f, -0.643f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(252.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(6602.0f, 48.0f, -5278.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-0.748f, -2.632f, -0.637f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(260.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(208.0f, -752.0f, -1366.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-1.571f, -3.486f, -0.908f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(122.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-3889.0f, -18.0f, 360.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-2.508f, -3.171f, -1.001f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(120.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-4108.0f, 143.0f, 1654.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.187f, -2.794f, -0.836f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(198.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-3254.0f, 161.0f, 2876.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.257f, -2.281f, -0.718f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(241.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-2049.0f, 151.0f, 3062.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.816f, -2.020f, -0.830f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(247.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-735.0f, 70.0f, 2264.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-3.430f, -0.708f, -0.510f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(176.0f, 200.0f, 200.0f));


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
	goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/", "MugenStageGoal.obj", { L"Resource/Game/red.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// ゴールのパラメーターを設定。
	stageObjectMgr_->AddScale(goalInsIndex, Vec3(120.0f, 120.0f, 120.0f));

	// ゴールの当たり判定
	goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	stageObjectMgr_->AddScale(goalInsIndex, Vec3(450.0f, 300.0f, 300.0f));
	stageObjectMgr_->ChangeRotate(goalInsIndex, Vec3(0.0f, -0.417f, 0.0f));
	stageObjectMgr_->AddTrans(goalInsIndex, Vec3(-32.0f, -14.0f, 220.0f));

	// 中間地点をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "middlePointCollision.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// 中間地点を設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(1007.0f, 458.0f, 43.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.0f, -1.238f, 0.0f));
	stageObjectMgr_->AddTrans(indexBuff, Vec3(4470.0f, -491.0f, -3678.0f));

	// 見えない壁
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::WALL, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/", "MugenStageInvWall.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// 見えない壁を設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	stageObjectMgr_->NonDisplay(indexBuff);

	// 各変数を初期化。
	timer_ = 0;

	status_ = STATUS::DEF;

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
	for (auto& index : pointLightPos) {

		ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].isActive_ = status_ == STATUS::DEF;
		ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].isShadow_ = false;
		ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].lightPower_ = 1000;
		ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].lightPos_ = index;
		ConstBufferData.light_.pointLight_[static_cast<int>(&index - &pointLightPos[0])].lightSize_ = 1;

	}

}

BaseStage::ColliderOutput MugenStage::Collider(BaseStage::ColliderInput Input)
{

	/*===== 当たり判定 =====*/

	BaseStage::ColliderOutput output;

	output = stageObjectMgr_->Collider(Input);

	return output;

}

void MugenStage::ChangeStageStatus(const int& Status)
{

	/*===== ステージのステータスを変更 =====*/

	// 前フレームがDEFで、今フレームがREFLECTIONだったら。
	if (status_ == STATUS::DEF && static_cast<STATUS>(Status) == STATUS::REFLECTION) {
		
		stageObjectMgr_->ChangeInstanceShaderID(tunnelIndex_, PolygonInstanceRegister::COMPLETE_REFLECTION);

		// ステージのパラメーターを設定。
		stageObjectMgr_->AddScale(tunnelIndex_, Vec3(120.0f, 120.0f, 120.0f));


	}
	else if (status_ == STATUS::REFLECTION && static_cast<STATUS>(Status) == STATUS::DEF) {

		stageObjectMgr_->ChangeInstanceShaderID(tunnelIndex_, PolygonInstanceRegister::DEF);

		// ステージのパラメーターを設定。
		stageObjectMgr_->AddScale(tunnelIndex_, Vec3(120.0f, 120.0f, 120.0f));

	}

	// ステータスを保存。
	status_ = static_cast<STATUS>(Status);

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