#include "MugenStage.h"
#include "StageObjectMgr.h"
#include "HitGroupMgr.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include "ConvertGimmickInfoFromBlender.h"
#include "TextureManager.h"
#include <algorithm>

void MugenStage::Setting(int TireMaskIndex, bool IsBoostGimmick)
{

	/*===== セッティング処理 =====*/

	// ステージオブジェクト管理クラスをセット。
	stageObjectMgr_ = std::make_shared<StageObjectMgr>();
	stageObjectMgr_->Setting();

	// 当たり判定用のステージをセット。
	int indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/MugenStage/", "MugenStage.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	PolygonInstanceRegister::Ins()->NonDisplay(stageObjectMgr_->GetInstanceIndex(0));

	// タイヤ痕用のステージをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/MugenStage/", "MugenStageTireMaskUV.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF, true, true);
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	PolygonInstanceRegister::Ins()->NonDisplay(stageObjectMgr_->GetInstanceIndex(1));

	// 描画用のステージをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/MugenStage/", "MugenStageDraw.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_TIREMASK_AO, true, true);
	stageObjectMgr_->ChangeMapTexture(indexBuff, TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/StageTex/StageTex/Road/NormalMap.png"), BLAS::MAP_PARAM::NORMAL);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	// 当たり判定用のステージのUVをSubUVとして代入する。
	stageObjectMgr_->AssignmentUVToSubUV(1, 2);

	// ステージにタイヤ痕用マスクをセット。
	BLASRegister::Ins()->GetBLAS()[stageObjectMgr_->GetBlasIndex(indexBuff)]->AddUAVTex(TireMaskIndex);

	// ステージの坂セット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/MugenStage/", "MugenStageSlope.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_AO);
	stageObjectMgr_->ChangeMapTexture(indexBuff, TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/StageTex/SlopeNormalMap.dds"), BLAS::MAP_PARAM::NORMAL);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの白線をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/MugenStage/", "MugenStageWhiteLine.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_AO);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの縁石をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/MugenStage/", "MugenStageCurbStone.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_AO);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの歩道をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/MugenStage/", "MugenStageSidewalk.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_AO);
	stageObjectMgr_->ChangeMapTexture(indexBuff, TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/StageTex/StageTex/SideWalk/NormalMap.png"), BLAS::MAP_PARAM::NORMAL);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの草をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/MugenStage/", "MugenStageGrass.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_AO);
	stageObjectMgr_->ChangeMapTexture(indexBuff, TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/StageTex/GrassNormalMap.png"), BLAS::MAP_PARAM::NORMAL);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// ステージの山をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/MugenStage/", "MugenStageOrnament.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeMapTexture(indexBuff, TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/StageTex/OrnamentGroundNormalMap.png"), BLAS::MAP_PARAM::NORMAL);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));

	// トンネルをセット。
	tunnelIndex_ = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/MugenStage/", "MugenStageTunnel.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeMapTexture(tunnelIndex_, TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/StageTex/TunnelNormalMap.dds"), BLAS::MAP_PARAM::NORMAL);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(tunnelIndex_, Vec3(120.0f, 120.0f, 120.0f));

	// 明るさの壁
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BRIGHTNESS_WALL, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/MugenStage/", "MugenStageBrightness.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	stageObjectMgr_->NonDisplay(indexBuff);

	// 暗さの壁
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::DARKNESS_WALL, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/MugenStage/", "MugenStageDarkness.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	// ステージのパラメーターを設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
	stageObjectMgr_->NonDisplay(indexBuff);

	// 加速ギミックをセット
	if (IsBoostGimmick) {
		SetBoostGimmick();
	}

	// アイテムボックス
	ConvertGimmickInfoFromBlender itemBoxData;
	itemBoxData.Convert("Resource/Game/Stage/MugenStage/", "MugenStageItemBox.obj", DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), DirectX::XMMatrixScaling(120, 120, 120));

	int itemBoxCount = itemBoxData.GetVertexSize();
	for (int index = 0; index < itemBoxCount; ++index) {

		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::SPHERE,
			"Resource/Game/Gimmick/", "itemBox.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_EMISSIVE);
		stageObjectMgr_->ChangeTrans(indexBuff, itemBoxData.GetVertex(index));
		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
		stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));

	}

	// ランキングカウント用の壁をロード
	LoadRankingWall();

	// ビルをロード
	LoadBuilding();

	// ゴールをセット。
	goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
		"Resource/Game/Stage/MugenStage/", "MugenStageGoal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	stageObjectMgr_->ChangeMapTexture(goalInsIndex, TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/grassNormal.png"), BLAS::MAP_PARAM::NORMAL);
	// ゴールのパラメーターを設定。
	stageObjectMgr_->AddScale(goalInsIndex, Vec3(120.0f, 120.0f, 120.0f));

	// ゴールの当たり判定
	goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	stageObjectMgr_->AddScale(goalInsIndex, Vec3(450.0f, 300.0f, 300.0f));
	stageObjectMgr_->ChangeRotate(goalInsIndex, Vec3(0.0f, -0.417f, 0.0f));
	stageObjectMgr_->AddTrans(goalInsIndex, Vec3(-32.0f, -14.0f, 220.0f));

	// 中間地点をセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Stage/MugenStage/", "middlePointCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// 中間地点を設定。
	stageObjectMgr_->AddScale(indexBuff, Vec3(1007.0f, 458.0f, 43.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.0f, -1.238f, 0.0f));
	stageObjectMgr_->AddTrans(indexBuff, Vec3(4470.0f, -491.0f, -3678.0f));

	// 見えない壁
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::WALL, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/Stage/MugenStage/", "MugenStageInvWall.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
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

#include "RayEngine.h"
void MugenStage::Update(std::weak_ptr<CharacterMgr> Character)
{

	/*===== 更新処理 =====*/

	++timer_;

	stageObjectMgr_->Update(timer_, Character);

}

BaseStage::ColliderOutput MugenStage::Collider(BaseStage::ColliderInput Input)
{

	/*===== 当たり判定 =====*/

	BaseStage::ColliderOutput output;

	output = stageObjectMgr_->Collider(Input);

	return output;

}

int MugenStage::GetRankingWallCount()
{
	return stageObjectMgr_->GetRankingWallCount();
}

#include <fstream>
#include <iostream>
#include <sstream>
void MugenStage::LoadBuilding()
{

	GetBuildingData("Resource/Game/Stage/MugenStage/Building/MugenStageBuildingData.txt");		// 直線周り<
	GetBuildingData("Resource/Game/Stage/MugenStage/Building/MugenStageBuildingData2.txt");		// 大きい方のカーブ周り
	GetBuildingData("Resource/Game/Stage/MugenStage/Building/MugenStageBuildingData3.txt");		// 小さい方のカーブ周り
	GetBuildingData("Resource/Game/Stage/MugenStage/Building/MugenStageBuildingData4.txt");		// 小道を作ろうとしていたところ

}

void MugenStage::LoadRankingWall()
{

	/*===== ランキング用の壁をロード =====*/

	// オブジェクトの名前の配列
	std::array<std::string, 12> modelName = { "wall01.obj","wall02.obj" ,"wall03.obj" ,"wall04.obj" ,"wall05.obj" ,"wall06.obj" ,"wall07.obj" ,"wall08.obj","wall09.obj","wall10.obj","wall11.obj","wall12.obj" };

	for (int index = 0; index < 12; ++index) {

		// ステージの坂セット。
		int indexBuff = stageObjectMgr_->AddRankingWall("Resource/Game/Stage/MugenStage/ForRankings/", modelName[index], HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
		stageObjectMgr_->AddScaleRankingWall(indexBuff, Vec3(120.0f, 120.0f, 120.0f));
		stageObjectMgr_->NonDisplayRankingWall(indexBuff);

	}

}

void MugenStage::GetBuildingData(std::string FilePath)
{

	// ファイルを開く。
	std::ifstream file(FilePath);

	// 失敗を検出。
	if (!file) {
		assert(0);
	}

	// 1行を読み取る。
	std::string cutLine;
	int objType = 0;
	Vec3 pos;
	Vec3 rotate;
	Vec3 scale;
	OBJ_DATA_ID lineDataID = OBJ_DATA_ID::ID;
	while (std::getline(file, cutLine, '\n')) {

		std::istringstream line(cutLine);
		std::string cutElement;
		while (std::getline(line, cutElement, ' ')) {

			std::istringstream operationElement(cutElement);
			std::string operationString;

			// Positionだったら座標データ
			if (cutElement == "Position") {
				lineDataID = OBJ_DATA_ID::POSITION;
				continue;
			}
			// Scaleだったら座標データ
			if (cutElement == "Scale") {
				lineDataID = OBJ_DATA_ID::SCALE;
				continue;
			}
			// Rotationだったら座標データ
			if (cutElement == "Rotation") {
				lineDataID = OBJ_DATA_ID::ROTATE;
				continue;
			}

			if (cutElement == "--") {
				int indexBuff = 0;
				if (objType == 0) {
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Low/", "BuildingLow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Low/", "BuildingLowWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
				}
				else if (objType == 1) {
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Low/", "BuildingLow2.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Low/", "BuildingLow2Window.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
				}
				else if (objType == 2) {
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Middle/", "BuildingMiddle.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Middle/", "BuildingMiddleWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
				}
				else if (objType == 3) {
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Middle/", "BuildingMiddle2.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/Middle/", "BuildingMiddle2Window.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
				}
				else if (objType == 4) {
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/High/", "BuildingHigh.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
					indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
						"Resource/Game/Stage/MugenStage/Building/High/", "BuildingHighWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
				}
				BLASRegister::Ins()->GetBLAS()[stageObjectMgr_->GetBlasIndex(indexBuff - 1)]->ChangeMapTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/Building/AOMap.dds"), BLAS::MAP_PARAM::AO);
				stageObjectMgr_->ChangeScale(indexBuff, scale);
				stageObjectMgr_->ChangeRotate(indexBuff, rotate);
				stageObjectMgr_->ChangeTrans(indexBuff, pos);
				stageObjectMgr_->ChangeScale(indexBuff - 1, scale);
				stageObjectMgr_->ChangeRotate(indexBuff - 1, rotate);
				stageObjectMgr_->ChangeTrans(indexBuff - 1, pos);
				continue;
			}


			OBJ_DATA_ID id = OBJ_DATA_ID::ID;
			while (std::getline(operationElement, operationString, ':')) {

				// IDだったらビルのID。
				if (operationString == "ID") {
					id = OBJ_DATA_ID::ID;
					continue;
				}

				if (operationString == "X") {
					id = OBJ_DATA_ID::X;
					continue;
				}
				if (operationString == "Y") {
					id = OBJ_DATA_ID::Y;
					continue;
				}
				if (operationString == "Z") {
					id = OBJ_DATA_ID::Z;
					continue;
				}

				// IDがIDだったらビルのIDなので、保存する。
				if (id == OBJ_DATA_ID::ID) {
					objType = std::stoi(operationString);
					continue;
				}

				// 行でのデータIDによってどのデータを保存するかを決める。
				if (lineDataID == OBJ_DATA_ID::POSITION) {

					if (id == OBJ_DATA_ID::X) {
						pos.x_ = std::stof(operationString);
					}
					else if (id == OBJ_DATA_ID::Y) {
						pos.y_ = std::stof(operationString);
					}
					else if (id == OBJ_DATA_ID::Z) {
						pos.z_ = std::stof(operationString);
					}

				}
				else if (lineDataID == OBJ_DATA_ID::SCALE) {

					if (id == OBJ_DATA_ID::X) {
						scale.x_ = std::stof(operationString);
					}
					else if (id == OBJ_DATA_ID::Y) {
						scale.y_ = std::stof(operationString);
					}
					else if (id == OBJ_DATA_ID::Z) {
						scale.z_ = std::stof(operationString);
					}

				}
				else if (lineDataID == OBJ_DATA_ID::ROTATE) {

					if (id == OBJ_DATA_ID::X) {
						rotate.x_ = std::stof(operationString);
					}
					else if (id == OBJ_DATA_ID::Y) {
						rotate.y_ = std::stof(operationString);
					}
					else if (id == OBJ_DATA_ID::Z) {
						rotate.z_ = std::stof(operationString);
					}

				}


			}

		}

	}

	// ファイルを閉じる。
	file.close();

}

void MugenStage::SetBoostGimmick()
{

	/*===== 加速ギミックをセット =====*/

	// 段差加速ギミックをセット。
	int indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(572.0f, 13.0f, -1000.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(491.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(687.0f, 9.0f, -1491.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(408.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(831.0f, 23.0f, -1963.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(357.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1129.0f, 21.0f, -2609.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.376f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(382.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1734.0f, 7.0f, -3828.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(176.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1680.0f, -3.0f, -4883.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(247.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(2076.0f, 10.0f, -5535.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.343f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(423.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(5023.0f, -359.0f, -4220.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(141.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(4270.0f, -558.0f, -3216.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(141.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(3068.0f, -725.0f, -3045.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -0.959f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(141.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1992.0f, -748.0f, -1875.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -1.237f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(206.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(222.0f, -748.0f, -1385.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.165f, -1.237f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(146.0f, 200.0f, 200.0f));

	// 加速ギミックをセット。
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::TEXCOLOR);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(2144.0f, 0.0f, -5519.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.0f, -0.363f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(350.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::TEXCOLOR);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(1113.0f, 18.0f, -2620.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0.0f, -0.395f, 0.0f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(362.0f, 200.0f, 200.0f));

	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/Gimmick/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::TEXCOLOR);
	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(208.0f, -752.0f, -1366.0f));
	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(-1.571f, -3.486f, -0.908f));
	stageObjectMgr_->ChangeScale(indexBuff, Vec3(122.0f, 200.0f, 200.0f));

}
