#include "CircuitStage.h"
#include "StageObjectMgr.h"
#include "HitGroupMgr.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

void CircuitStage::Setting()
{

	/*===== �Z�b�e�B���O���� =====*/

	// �X�e�[�W�I�u�W�F�N�g�Ǘ��N���X���Z�b�g�B
	stageObjectMgr_ = std::make_shared<StageObjectMgr>();

	// �X�e�[�W���Z�b�g�B
	int indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/", "stage3.obj", { L"Resource/Game/white.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// �X�e�[�W�̃p�����[�^�[��ݒ�B
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// �X�e�[�W�̑����Z�b�g�B
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::STAGE_GRASS, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/", "grass.obj", { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF_GI);
	// �X�e�[�W�̃p�����[�^�[��ݒ�B
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// �S�[�����Z�b�g�B
	goalInsIndex = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::GOAL, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "goal.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// �S�[���̃p�����[�^�[��ݒ�B
	stageObjectMgr_->AddScale(goalInsIndex, Vec3(200.0f, 200.0f, 200.0f));
	stageObjectMgr_->AddTrans(goalInsIndex, Vec3(10, -30, 0));

	// ���Ԓn�_���Z�b�g�B
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::MIDDLE_POINT, BaseStageObject::COLLISION_ID::OBB,
		"Resource/Game/", "middlePointCollision.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::REFRACTION);
	// ���Ԓn�_��ݒ�B
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));
	stageObjectMgr_->AddTrans(indexBuff, Vec3(-8000, 1000, 2800));

	// �����I�u�W�F�N�g���Z�b�g�B
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
	indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::ORNAMENT, BaseStageObject::COLLISION_ID::MESH,
		"Resource/Game/stageOrnament/", "goalSideObj.obj", { L"Resource/Game/gray.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
	stageObjectMgr_->AddScale(indexBuff, Vec3(200.0f, 200.0f, 200.0f));

	// �����M�~�b�N���Z�b�g�B
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

	// �S�[���O�̂ӂ�ӂ�I�u�W�F�N�g���Z�b�g�B
	for (int index = 0; index < 4; ++index) {
		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
			"Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
		stageObjectMgr_->ChangeTrans(indexBuff, Vec3(250, 200, 1000 * index + 1500.0f));
		stageObjectMgr_->ChangeScale(indexBuff, Vec3(40.0f, 40.0f, 40.0f));
		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
		indexBuff = stageObjectMgr_->AddObject(BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT, BaseStageObject::COLLISION_ID::NONE,
			"Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", { L"Resource/Game/red.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::DEF);
		stageObjectMgr_->ChangeTrans(indexBuff, Vec3(-250, 200, 1000 * index + 1500.0f));
		stageObjectMgr_->ChangeScale(indexBuff, Vec3(40.0f, 40.0f, 40.0f));
		stageObjectMgr_->ChangeRotate(indexBuff, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
	}

	// �e�ϐ����������B
	timer_ = 0;

}

void CircuitStage::Destroy()
{

	/*===== �j������ =====*/

}

void CircuitStage::Update()
{

	/*===== �X�V���� =====*/

	++timer_;

	stageObjectMgr_->Update(timer_);

}

BaseStage::ColliderOutput CircuitStage::Collider(BaseStage::ColliderInput Input)
{

	/*===== �����蔻�� =====*/

	BaseStage::ColliderOutput output;

	output = stageObjectMgr_->Collider(Input);

	return output;

}

void CircuitStage::DisplayGoal()
{

	/*===== �S�[���̕\�� =====*/

	PolygonInstanceRegister::Ins()->Display(goalInsIndex);

}

void CircuitStage::NonDisplayGoal()
{

	/*===== �S�[���̔�\�� =====*/

	PolygonInstanceRegister::Ins()->NonDisplay(goalInsIndex);

}