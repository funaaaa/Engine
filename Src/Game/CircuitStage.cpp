//#include "CircuitStage.h"
//#include "StageObjectMgr.h"
//#include "HitGroupMgr.h"
//#include "PolygonInstanceRegister.h"
//#include "FHelper.h"
//#include "BLASRegister.h"
//#include "BLAS.h"
//
//void CircuitStage::Setting(int TireMaskIndex)
//{
//
//	/*===== �Z�b�e�B���O���� =====*/
//
//	// �X�e�[�W�I�u�W�F�N�g�Ǘ��N���X���Z�b�g�B
//	stageObjectMgr_ = std::make_shared<StageObjectMgr>();
//
//	// �X�e�[�W���Z�b�g�B
//	int indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/", "stage3.obj", { L"Resource/Game/wayGray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI_TIREMASK);
//	// �X�e�[�W�̃p�����[�^�[��ݒ�B
//	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//
//	// �X�e�[�W�Ƀ^�C�����p�}�X�N���Z�b�g�B
//	BLASRegister::Ins()->GetBLAS()[stageObjectMgr_->GetBlasIndex(indexBuff)]->AddUAVTex(TireMaskIndex);
//
//	// �X�e�[�W�̑����Z�b�g�B
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/", "grass.obj", { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
//	// �X�e�[�W�̃p�����[�^�[��ݒ�B
//	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//
//	// �S�[�����Z�b�g�B
//	goalInsIndex_ = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
//	// �S�[���̃p�����[�^�[��ݒ�B
//	stageObjectMgr_->AddScale(goalInsIndex_, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->AddTrans(goalInsIndex_, Vec3(10, -30, 0));
//
//	// ���Ԓn�_���Z�b�g�B
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/", "middlePointCollision.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
//	// ���Ԓn�_��ݒ�B
//	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->AddTrans(indexBuff, Vec3(-8000, 1000, 2800));
//
//	// �����I�u�W�F�N�g���Z�b�g�B
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/stageOrnament/", "blockA.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFLECTION);
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(32.5f, 66.0f, -2359.5f));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/stageOrnament/", "blockB.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFLECTION);
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1870.0f, 133.5f, -2410.0f));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/stageOrnament/", "blockC.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-3550.5, 71.5f, -955.5f));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/stageOrnament/", "blockD.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-2038.0f, 21.0f, 2865.0f));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/stageOrnament/", "blockE.obj", { L"Resource/Game/green.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1702.0f, 145.5f, 4905.0f));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/stageOrnament/", "blockF.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1587.5f, 80.0f, 6646.0f));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
//		"Resource/Game/stageOrnament/", "goalSideObj.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
//
//	// �A�C�e���{�b�N�X�e�X�g�p
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1871.5f, 48.0f, -2793.0f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1871.5f, 48.0f, -2893.0f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1871.5f, 48.0f, -2693.0f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-7607.0f, 589.0f, 2801.0f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, -0.184f));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-7607.0f, 589.0f, 2951.0f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, -0.184f));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-7607.0f, 589.0f, 2651.0f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, -0.184f));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1305.0f, 48.0f, 6326.5f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1305.0f, 48.0f, 6476.5f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ITEM_BOX, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/stageOrnament/", "itemBox.obj", { L"Resource/Game/item.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-1305.0f, 48.0f, 6186.5f));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(60, 60, 60));
//
//
//
//	// �����M�~�b�N���Z�b�g�B
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(100, -15, 1400));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
//
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-80, -15, 3000));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(100, -15, 4000));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 0, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(100, 200, 200));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-5842, -29, -167));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 1.48f, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(200, 200, 200));
//	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::BOOST_GIMMICK, BaseStageObject::COLLISION_ID::OBB,
//		"Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//	stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-2352, -18, 6336));
//	stageObjectMgr_->ChangeRotate(indexBuff, Vec3(0, 1.58f, 0));
//	stageObjectMgr_->ChangeScale(indexBuff, Vec3(190, 200, 200));
//
//	// �S�[���O�̂ӂ�ӂ�I�u�W�F�N�g���Z�b�g�B
//	for (int index = 0; index < 4; ++index) {
//		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
//			"Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//		stageObjectMgr_->ChangeTrans(indexBuff, Vec3(250, 200, 1000 * index + 1500.0f));
//		stageObjectMgr_->ChangeScale(indexBuff, Vec3(40.0f, 40.0f, 40.0f));
//		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
//		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
//			"Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
//		stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-250, 200, 1000 * index + 1500.0f));
//		stageObjectMgr_->ChangeScale(indexBuff, Vec3(40.0f, 40.0f, 40.0f));
//		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
//	}
//
//	// �e�ϐ����������B
//	timer_ = 0;
//
//}
//
//void CircuitStage::Destroy()
//{
//
//	/*===== �j������ =====*/
//
//}
//
//void CircuitStage::Update(RayConstBufferData& ConstBufferData)
//{
//
//	/*===== �X�V���� =====*/
//
//	++timer_;
//
//	stageObjectMgr_->Update(timer_);
//
//	ConstBufferData;
//
//}
//
//BaseStage::ColliderOutput CircuitStage::Collider(BaseStage::ColliderInput Input)
//{
//
//	/*===== �����蔻�� =====*/
//
//	BaseStage::ColliderOutput output;
//
//	output = stageObjectMgr_->Collider(Input);
//
//	return output;
//
//}
//
//void CircuitStage::ChangeStageStatus(int Status)
//{
//
//	Status;
//
//}
//
//void CircuitStage::DisplayGoal()
//{
//
//	/*===== �S�[���̕\�� =====*/
//
//	PolygonInstanceRegister::Ins()->Display(goalInsIndex_);
//
//}
//
//void CircuitStage::NonDisplayGoal()
//{
//
//	/*===== �S�[���̔�\�� =====*/
//
//	PolygonInstanceRegister::Ins()->NonDisplay(goalInsIndex_);
//
//}
