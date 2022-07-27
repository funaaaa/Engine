#include "StageObjectMgr.h"
#include "BasicStageObject.h"
#include "FloatingStageObject.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"
#include "BLAS.h"

int StageObjectMgr::AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== �M�~�b�N��ǉ� =====*/

	// �ӂ�ӂ퓮�������I�u�W�F�N�g��������B
	if (ObjectID == BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT) {

		objects_.emplace_back(std::make_shared<FloatingStageObject>());

	}
	// ����ȊO�̒ʏ�̃I�u�W�F�N�g��������B
	else {

		objects_.emplace_back(std::make_shared<BasicStageObject>());

	}

	// Blas�����[�h
	int blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	// Instance�𐶐��B
	int instanceIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID);

	// �I�u�W�F�N�g��ݒ�B
	objects_.back()->Setting(ObjectID, CollisionID, instanceIndex);

	return static_cast<int>(objects_.size()) - 1;

}

void StageObjectMgr::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

	for (auto& index : objects_) {

		index->Update(Timer);

	}

}

BaseStage::ColliderOutput StageObjectMgr::Collider(BaseStage::ColliderInput Input)
{

	/*==== �����蔻�� =====*/

	// �����蔻�茋�ʗp
	BaseStage::ColliderOutput output;
	output.isHitBoostGimmick_ = false;
	output.isHitGoal_ = false;
	output.isHitMiddlePoint_ = false;
	output.isHitOrnament_ = false;
	output.isHitStageGrass_ = false;
	output.isHitStage_ = false;
	output.ornamentHitNormal_ = Vec3(-100, -100, -100);

	for (auto& index : objects_) {

		// �I�u�W�F�N�g���L��������Ă��Ȃ������珈�����΂��B
		if (!index->GetIsActive())continue;

		// �I�u�W�F�N�g�������蔻����s��Ȃ����̂������珈�����΂��B
		BaseStageObject::COLLISION_ID indexCollisionID = index->GetCollisionID();
		if (indexCollisionID == BaseStageObject::COLLISION_ID::NONE) continue;

		// �����蔻�肪OBB��������B
		if (indexCollisionID == BaseStageObject::COLLISION_ID::OBB) {

			// OBB�̓����蔻����s���B
			bool isHit = Input.targetOBB_.lock()->CheckHitOBB(index->GetOBB());

			// �������Ă��Ȃ������玟�̗v�f�ցB
			if (!isHit) continue;

			// �I�u�W�F�N�g��ID���擾���ĕۑ��B
			BaseStageObject::OBJECT_ID indexObjID = index->GetObjectID();

			// �I�u�W�F�N�g���Ƃɕۑ��B
			if (indexObjID == BaseStageObject::OBJECT_ID::BOOS_GIMMICK) {
				output.isHitBoostGimmick_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::GOAL) {
				output.isHitGoal_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::MIDDLE_POINT) {
				output.isHitMiddlePoint_ = true;
			}

		}
		// �����蔻�肪MESH��������B
		else if (indexCollisionID == BaseStageObject::COLLISION_ID::MESH) {

			// �����蔻��p�̃f�[�^�𐶐��B
			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetVertex_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexPos();
			InputRayData.targetNormal_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexNormal();
			InputRayData.targetIndex_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexIndex();
			InputRayData.targetUV_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexUV();
			InputRayData.matTrans_ = PolygonInstanceRegister::Ins()->GetTrans(index->GetINSTANCEIndex());
			InputRayData.matScale_ = PolygonInstanceRegister::Ins()->GetScale(index->GetINSTANCEIndex());
			InputRayData.matRot_ = PolygonInstanceRegister::Ins()->GetRotate(index->GetINSTANCEIndex());

			// �I�u�W�F�N�g��ID���擾���ĕۑ��B
			BaseStageObject::OBJECT_ID indexObjID = index->GetObjectID();

			// �I�u�W�F�N�g���Ƃɓ����蔻����s��
			if (indexObjID == BaseStageObject::OBJECT_ID::STAGE || indexObjID == BaseStageObject::OBJECT_ID::STAGE_GRASS) {
				output = StageMeshCollider(Input, InputRayData, output, indexObjID == BaseStageObject::OBJECT_ID::STAGE);
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::ORNAMENT) {
				output = OrnamentMeshCollider(Input, InputRayData, output);
			}

		}

	}

	// �ŏI���ʂ̍��W��ۑ��B
	output.resultPos_ = Input.targetPos_;

	return output;

}

void StageObjectMgr::AddTrans(const int& Index, const Vec3& Trans)
{

	/*===== �ړ������Z =====*/

	objects_[Index]->AddTrans(Trans);

}

void StageObjectMgr::ChangeTrans(const int& Index, const Vec3& Trans)
{

	/*===== �ړ����� =====*/

	objects_[Index]->ChangeTrans(Trans);

}

void StageObjectMgr::AddScale(const int& Index, const Vec3& Scale)
{

	/*===== �X�P�[�������Z =====*/

	objects_[Index]->AddScale(Scale);

}

void StageObjectMgr::ChangeScale(const int& Index, const Vec3& Scale)
{

	/*===== �X�P�[������ =====*/

	objects_[Index]->ChangeScale(Scale);

}

void StageObjectMgr::AddRotate(const int& Index, const Vec3& Rotate)
{

	/*===== ��]�����Z =====*/

	objects_[Index]->AddRotate(Rotate);

}

void StageObjectMgr::ChangeRotate(const int& Index, const Vec3& Rotate)
{

	/*===== ��]���� =====*/

	objects_[Index]->ChangeRotate(Rotate);

}

void StageObjectMgr::NonDisplay(const int& Index)
{

	/*===== ��\�� =====*/

	objects_[Index]->NonDisplay();

}

void StageObjectMgr::Display(const int& Index)
{

	/*===== �\�� =====*/

	objects_[Index]->Display();

}

BaseStage::ColliderOutput StageObjectMgr::StageMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, const bool& IsStage)
{

	/*===== �X�e�[�W�A���Ƃ̃��b�V���̓����蔻�� =====*/

	// ���C�p�̐ݒ��ǉ��B
	InputRayData.rayPos_ = Input.targetPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_));

	// �����蔻��̌��ʕۑ��p�ϐ��B
	bool isHit = false;
	Vec3 impactPos;
	float hitDistance;
	Vec3 hitNormal;
	Vec2 hitUV;

	// �����蔻����s���B
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// ��������������Y���̃T�C�Y����������������B
	isHit &= (hitDistance - Input.targetSize_.y_) <= 0;
	isHit &= 0 < hitDistance;

	// �������Ă����牟���߂��B
	if (isHit) {

		// �҂����艟���߂��Ǝ��̃t���[���ŋ󒆔���ɂȂ��Ă��܂��̂ŁA�኱�I�t�Z�b�g��݂���B
		const float PUSH_BACK_OFFSET = 1.0f;

		// �@�������ɓ��������������߂��B
		Input.targetPos_ += hitNormal * (Input.targetSize_.y_ - (hitDistance + PUSH_BACK_OFFSET));

		// �΂ߏ��̉�]�����B
		RotObliqueFloor(Input, hitNormal, Output);

		// �X�e�[�W��������
		if (IsStage) {

			Output.isHitStage_ = true;

		}
		// ����������
		else {

			Output.isHitStageGrass_ = true;

		}

	}

	// �ђʖh�~�ŁA�ʏ�̃X�e�[�W�̂Ƃ��̂ݐ��ʕ����ɂ����C���΂��B
	if (IsStage) {

		InputRayData.rayPos_ = Input.targetPrevPos_;
		InputRayData.rayDir_ = (Input.targetPos_ - Input.targetPrevPos_).GetNormal();

		// �����蔻����s���B
		isHit = false;
		isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= fabs(hitDistance) < (Input.targetPos_ - Input.targetPrevPos_).Length();

		// �������Ă����牟���߂��B
		if (isHit) {

			// �@�������ɓ��������������߂��B
			Input.targetPos_ = impactPos + hitNormal * hitDistance;

		}


	}

	return Output;

}

BaseStage::ColliderOutput StageObjectMgr::OrnamentMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output)
{

	/*===== �X�e�[�W�����I�u�W�F�N�g�Ƃ̓����蔻�� =====*/

	InputRayData.rayPos_ = Input.targetPrevPos_;
	InputRayData.rayDir_ = (Input.targetPos_ - Input.targetPrevPos_).GetNormal();

	// �����蔻����s���B
	bool isHit = false;
	Vec3 impactPos;
	float hitDistance;
	Vec3 hitNormal;
	Vec2 hitUV;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// ��������������Y���̃T�C�Y����������������B
	isHit &= fabs(hitDistance) < (Input.targetPos_ - Input.targetPrevPos_).Length();

	// �������Ă����牟���߂��B
	if (isHit) {

		// �@�������ɓ��������������߂��B
		Input.targetPos_ = impactPos + hitNormal * hitDistance;
		Output.isHitOrnament_ = true;
		Output.ornamentHitNormal_ = hitNormal;

	}

	return Output;

}

void StageObjectMgr::RotObliqueFloor(BaseStage::ColliderInput Input, const Vec3& HitNormal, BaseStage::ColliderOutput& Output)
{

	/*===== �΂ߏ��̉�]���� =====*/

	/*-- �O�ς����]�����擾 --*/

	// �f�t�H���g�̏�x�N�g���B
	Vec3 defUpVec = Vec3(0, 1, 0);

	// ��]�������߂�B
	Vec3 axisOfRevolution = defUpVec.Cross(HitNormal);

	// ��]���𐳋K������B
	if (axisOfRevolution.Length() != 0) {
		axisOfRevolution.Normalize();
	}

	/*-- ���ς����]�ʂ��擾 --*/

	// ��]�ʂ����߂�B
	float amountOfRotation = defUpVec.Dot(HitNormal);

	// �t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ��B
	amountOfRotation = acosf(amountOfRotation);


	/*-- �N�H�[�^�j�I�����g���ĉ�] --*/

	// ��]����{0,0,0}�������珈�����΂��B
	if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, quaternionMat);

		// �@���x�N�g��
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// �N�H�[�^�j�I�����s��Ɏ����B
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// �v���C���[����]������B
			PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_);

		// ��x�N�g�����X�V�B
		Output.upVec_ = normal_;

		//���ʃx�N�g�����X�V�B
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// ���������@�����ゾ�����ꍇ�͍s������������ĉ�]������B
	if (HitNormal == Vec3(0, 1, 0)) {

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, DirectX::XMMatrixIdentity());

		// �@���x�N�g��
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// �N�H�[�^�j�I�����s��Ɏ����B
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// ��]������B
			PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- ��]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_);

		// ��x�N�g�����X�V�B
		Output.upVec_ = normal_;

		//���ʃx�N�g�����X�V�B
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}
