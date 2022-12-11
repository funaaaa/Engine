#include "StageObjectMgr.h"
#include "BasicStageObject.h"
#include "ItemBoxObject.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "OBB.h"
#include "BLAS.h"
#include "BaseStageObject.h"

void StageObjectMgr::Setting()
{

	/*===== ���� =====*/

	for (auto& index : objects_) {

		index.second = false;

	}

}

int StageObjectMgr::AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, UINT ShaderID, bool IsOpaque, bool IsNewGenerate)
{

	/*===== �M�~�b�N��ǉ� =====*/

	// �󂫃I�u�W�F�N�g�����B
	int addIndex = -1;
	for (auto& index : objects_) {

		if (index.second) continue;

		addIndex = static_cast<int>(&index - &objects_[0]);

		break;

	}

	// -1������������󂫂͂Ȃ��ł��B
	if (addIndex == -1) {

		assert(0);

	}

	// �A�C�e���{�b�N�X�I�u�W�F�N�g��������
	if (ObjectID == BaseStageObject::OBJECT_ID::ITEM_BOX) {

		objects_[addIndex].first = std::make_shared<ItemBoxObject>();

	}
	// ����ȊO�̒ʏ�̃I�u�W�F�N�g��������B
	else {

		objects_[addIndex].first = std::make_shared<BasicStageObject>();

	}

	// Blas�����[�h
	std::weak_ptr<BLAS> blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, IsOpaque, IsNewGenerate);
	// Instance�𐶐��B
	std::weak_ptr<PolygonMeshInstance> instance = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID, CollisionID == BaseStageObject::COLLISION_ID::MESH);

	// �I�u�W�F�N�g��ݒ�B
	objects_[addIndex].first->Setting(ObjectID, CollisionID, instance);
	objects_[addIndex].second = true;

	return addIndex;

}

int StageObjectMgr::AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::wstring& ModelPath, const std::wstring& HitGroupName, UINT ShaderID, bool IsOpaque)
{

	/*===== �M�~�b�N��ǉ� =====*/

	// �󂫃I�u�W�F�N�g�����B
	int addIndex = -1;
	for (auto& index : objects_) {

		if (index.second) continue;

		addIndex = static_cast<int>(&index - &objects_[0]);

		break;

	}

	// -1������������󂫂͂Ȃ��ł��B
	if (addIndex == -1) {

		assert(0);

	}

	// �A�C�e���{�b�N�X�I�u�W�F�N�g��������
	if (ObjectID == BaseStageObject::OBJECT_ID::ITEM_BOX) {

		objects_[addIndex].first = std::make_shared<ItemBoxObject>();

	}
	// ����ȊO�̒ʏ�̃I�u�W�F�N�g��������B
	else {

		objects_[addIndex].first = std::make_shared<BasicStageObject>();

	}

	// Blas�����[�h
	std::weak_ptr<BLAS> blasIndex = BLASRegister::Ins()->GenerateGLTF(ModelPath, HitGroupName, IsOpaque);
	// Instance�𐶐��B
	std::weak_ptr<PolygonMeshInstance> instance = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID, CollisionID == BaseStageObject::COLLISION_ID::MESH);

	// �I�u�W�F�N�g��ݒ�B
	objects_[addIndex].first->Setting(ObjectID, CollisionID, instance);
	objects_[addIndex].second = true;

	return addIndex;

}

#include "Camera.h"
void StageObjectMgr::Update(int Timer)
{

	/*===== �X�V���� =====*/

	for (auto& index : objects_) {

		if (!index.second) continue;

		index.first->Update(Timer);

		if (index.first->GetObjectID() != BaseStageObject::OBJECT_ID::ORNAMENT) continue;

		//// ������J�����O
		//bool inScreen = FHelper::CheckInScreen(index.first->GetInstance().lock()->GetPos(), 500.0f, 500.0f, Camera::Ins()->matView_, Camera::Ins()->matPerspective_);

		//// �J�����O����B
		//if (true) {

		//	index.first->Display();

		//}
		//else {

		//	index.first->NonDisplay();

		//}

	}

}

void StageObjectMgr::ChangeMapTexture(int Index, int TextureIndex, BLAS::MAP_PARAM MapParam)
{

	/*===== �w��̃C���f�b�N�X�̖@���x�N�g����ύX =====*/

	objects_[Index].first->ChangeMapTexture(TextureIndex, static_cast<int>(MapParam));

}

BaseStage::ColliderOutput StageObjectMgr::Collider(BaseStage::ColliderInput Input)
{

	/*==== �����蔻�� =====*/

	// �����蔻�茋�ʗp
	BaseStage::ColliderOutput output;
	output.matRot_ = DirectX::XMMatrixIdentity();
	output.isHitBoostGimmick_ = false;
	output.isHitGoal_ = false;
	output.isHitMiddlePoint_ = false;
	output.isHitOrnament_ = false;
	output.isHitStageGrass_ = false;
	output.isHitStage_ = false;
	output.isHitItemBox_ = false;
	output.isHitStepBoostGimmick_ = false;
	output.isHitBrightnessWall_ = false;
	output.isHitDarknessWall_ = false;
	output.ornamentHitNormal_ = Vec3(-100, -100, -100);
	output.resultPos_ = Input.targetPos_;

	for (auto& index : objects_) {

		if (!index.second) continue;
		if (!index.first->GetIsActive()) continue;

		// �I�u�W�F�N�g�������蔻����s��Ȃ����̂������珈�����΂��B
		BaseStageObject::COLLISION_ID indexCollisionID = index.first->GetCollisionID();
		if (indexCollisionID == BaseStageObject::COLLISION_ID::NONE) continue;

		// �����蔻�肪OBB��������B
		if (indexCollisionID == BaseStageObject::COLLISION_ID::OBB) {

			// ���ȏ㗣��Ă�����B
			float distance = Vec3(Input.targetOBB_.lock()->pos_ - index.first->GetOBB()->pos_).Length();
			float size = Vec3(Input.targetOBB_.lock()->length_ + index.first->GetOBB()->length_).Length();
			if (size < distance) continue;

			// OBB�̓����蔻����s���B
			bool isHit = Input.targetOBB_.lock()->CheckHitOBB(index.first->GetOBB());

			// �������Ă��Ȃ������玟�̗v�f�ցB
			if (!isHit) continue;

			// �I�u�W�F�N�g��ID���擾���ĕۑ��B
			BaseStageObject::OBJECT_ID indexObjID = index.first->GetObjectID();

			// �I�u�W�F�N�g���Ƃɕۑ��B
			if (indexObjID == BaseStageObject::OBJECT_ID::BOOST_GIMMICK) {
				output.isHitBoostGimmick_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::GOAL) {
				output.isHitGoal_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::MIDDLE_POINT) {
				output.isHitMiddlePoint_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK) {
				output.isHitStepBoostGimmick_ = true;
			}
			// ���������I�u�W�F�N�g���A�C�e���{�b�N�X��������B
			else if (indexObjID == BaseStageObject::OBJECT_ID::ITEM_BOX && Input.isPlayer_) {

				// �A�C�e���{�b�N�X���ꎞ�I�ɖ������B
				index.first->Disable(180);

				output.isHitItemBox_ = true;

			}

		}
		// �����蔻�肪����������B
		else if (indexCollisionID == BaseStageObject::COLLISION_ID::SPHERE) {

			// �����蔻����s���B
			bool isHit = Vec3(Input.targetPos_ - index.first->GetInstance().lock()->GetPos()).Length() <= Input.targetSize_.x_ + index.first->GetInstance().lock()->GetScaleVec3().x_;

			if (!isHit) continue;

			// �A�C�e���{�b�N�X���ꎞ�I�ɖ������B
			index.first->Disable(180);

			output.isHitItemBox_ = true;

		}
		// �����蔻�肪MESH��������B
		else if (indexCollisionID == BaseStageObject::COLLISION_ID::MESH) {

			// �����蔻��p�̃f�[�^�𐶐��B
			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(index.first->GetInstanceIndex());

			// �I�u�W�F�N�g��ID���擾���ĕۑ��B
			BaseStageObject::OBJECT_ID indexObjID = index.first->GetObjectID();

			// �I�u�W�F�N�g���Ƃɓ����蔻����s��
			if (indexObjID == BaseStageObject::OBJECT_ID::STAGE || indexObjID == BaseStageObject::OBJECT_ID::STAGE_GRASS || indexObjID == BaseStageObject::OBJECT_ID::WALL) {
				output = StageMeshCollider(Input, InputRayData, output, indexObjID);
			}
			else if (Input.isPlayer_ && (indexObjID == BaseStageObject::OBJECT_ID::BRIGHTNESS_WALL || indexObjID == BaseStageObject::OBJECT_ID::DARKNESS_WALL)) {
				output = OrnamentMeshCollider(Input, InputRayData, output, indexObjID);
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::ORNAMENT) {

				// ���ȏ㗣��Ă�����B �I�u�W�F�N�g�̔z�u��Blender��ł���Ă��邽�ߋ����𒸓_���玝���Ă��Ă��邪�A������͎蓮�Ŕz�u���č��W���狗���������Ă����悤�ɂ���
				float distance = Vec3(Input.targetPos_ - index.first->GetInstance().lock()->GetPos()).Length();
				float size = Vec3(Input.targetSize_ + index.first->GetOBB()->length_).Length();
				if (size < distance) continue;

				output = OrnamentMeshCollider(Input, InputRayData, output, indexObjID);
			}

		}

	}

	// �ŏI���ʂ̍��W��ۑ��B
	output.resultPos_ = Input.targetPos_;

	return output;

}

void StageObjectMgr::AddTrans(int Index, const Vec3& Trans)
{

	/*===== �ړ������Z =====*/

	objects_[Index].first->AddTrans(Trans);

}

void StageObjectMgr::ChangeTrans(int Index, const Vec3& Trans)
{

	/*===== �ړ����� =====*/

	objects_[Index].first->ChangeTrans(Trans);

}

void StageObjectMgr::AddScale(int Index, const Vec3& Scale)
{

	/*===== �X�P�[�������Z =====*/

	objects_[Index].first->AddScale(Scale);

}

void StageObjectMgr::ChangeScale(int Index, const Vec3& Scale)
{

	/*===== �X�P�[������ =====*/

	objects_[Index].first->ChangeScale(Scale);

}

void StageObjectMgr::AddRotate(int Index, const Vec3& Rotate)
{

	/*===== ��]�����Z =====*/

	objects_[Index].first->AddRotate(Rotate);

}

void StageObjectMgr::ChangeRotate(int Index, const Vec3& Rotate)
{

	/*===== ��]���� =====*/

	objects_[Index].first->ChangeRotate(Rotate);

}

void StageObjectMgr::NonDisplay(int Index)
{

	/*===== ��\�� =====*/

	objects_[Index].first->NonDisplay();

}

void StageObjectMgr::Display(int Index)
{

	/*===== �\�� =====*/

	objects_[Index].first->Display();

}

void StageObjectMgr::DeleteIndex(int Index)
{

	/*===== �v�f���폜 =====*/

	if (Index < 0 || static_cast<int>(objects_.size()) <= Index) assert(0);
	if (!objects_[Index].second) assert(0);

	objects_[Index].first->Delete();


}

void StageObjectMgr::ChangeInstanceShaderID(std::weak_ptr<PolygonMeshInstance> Instance, UINT ShaderID)
{

	/*===== �C���X�^���X�̃V�F�[�_�[ID��؂�ւ��� =====*/

	int index = Instance.lock()->GetInstanceIndex();
	if (index < 0 || static_cast<int>(objects_.size()) <= index) assert(0);
	if (!objects_[index].second) assert(0);

	// �e�v�f��ۑ��B
	std::weak_ptr<BLAS> blasIndex = objects_[index].first->GetInstance().lock()->GetBLAS();
	BaseStageObject::COLLISION_ID CollisionID = objects_[index].first->GetCollisionID();
	BaseStageObject::OBJECT_ID ObjectID = objects_[index].first->GetObjectID();

	// Instance��j���B
	PolygonInstanceRegister::Ins()->DestroyInstance(Instance);

	// Instance�𐶐��B
	std::weak_ptr<PolygonMeshInstance> instance = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID, CollisionID == BaseStageObject::COLLISION_ID::MESH);

	// �I�u�W�F�N�g��ݒ�B
	objects_[index].first->Setting(ObjectID, CollisionID, instance);

}

void StageObjectMgr::ChangeInstanceShaderID(int Index, UINT ShaderID)
{

	/*===== �C���X�^���X�̃V�F�[�_�[ID��؂�ւ��� =====*/

	int index = Index;
	if (index < 0 || static_cast<int>(objects_.size()) <= index) assert(0);
	if (!objects_[index].second) assert(0);

	// �e�v�f��ۑ��B
	std::weak_ptr<BLAS> blasIndex = objects_[index].first->GetInstance().lock()->GetBLAS();
	BaseStageObject::COLLISION_ID CollisionID = objects_[index].first->GetCollisionID();
	BaseStageObject::OBJECT_ID ObjectID = objects_[index].first->GetObjectID();

	// Instance��j���B
	PolygonInstanceRegister::Ins()->DestroyInstance(index);

	// Instance�𐶐��B
	std::weak_ptr<PolygonMeshInstance> instance = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID, CollisionID == BaseStageObject::COLLISION_ID::MESH);

	// �I�u�W�F�N�g��ݒ�B
	objects_[index].first->Setting(ObjectID, CollisionID, instance);

}

#include "BLASRegister.h"
void StageObjectMgr::AssignmentUVToSubUV(int AssigningBLASIndex, int AssignedBLASIndex)
{

	/*===== �w���BLAS��UV���w���BLAS��SUBUV�ɑ�����鏈�� =====*/

	BLASRegister::Ins()->GetBLAS()[objects_[AssignedBLASIndex].first->GetBLASIndex()]->AssignUV(BLASRegister::Ins()->GetBLAS()[objects_[AssigningBLASIndex].first->GetBLASIndex()]->GetVertex());

}

BaseStage::ColliderOutput StageObjectMgr::StageMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID)
{

	/*===== �X�e�[�W�A���Ƃ̃��b�V���̓����蔻�� =====*/

	// �����蔻��̌��ʕۑ��p�ϐ��B
	bool isHit = false;
	Vec3 impactPos;
	float hitDistance;
	Vec3 hitNormal;
	Vec2 hitUV;

	// �O�㍶�E�̎l�����Ƀ��C���΂��ē����蔻����s���B
	Output = Decision4Way(Input, InputRayData, Output, ObjectID);

	if (ObjectID != BaseStageObject::OBJECT_ID::WALL && ObjectID != BaseStageObject::OBJECT_ID::BRIGHTNESS_WALL && ObjectID != BaseStageObject::OBJECT_ID::BRIGHTNESS_WALL) {

		// ���C�p�̐ݒ��ǉ��B
		InputRayData.rayPos_ = Input.targetPos_;
		InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), Input.targetInstance_.lock()->GetRotate());

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
			if (ObjectID == BaseStageObject::OBJECT_ID::STAGE) {

				Output.isHitStage_ = true;

			}
			// ����������
			else if (ObjectID == BaseStageObject::OBJECT_ID::STAGE_GRASS) {

				Output.isHitStageGrass_ = true;

			}

		}



		// �ړ��ʂ̐����̓����蔻��B
		InputRayData.rayPos_ = Input.targetPrevPos_;
		InputRayData.rayDir_ = (Input.targetPos_ - Input.targetPrevPos_).GetNormal();

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

BaseStage::ColliderOutput StageObjectMgr::OrnamentMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID)
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

		if (ObjectID == BaseStageObject::OBJECT_ID::BRIGHTNESS_WALL) {
			Output.isHitDarknessWall_ = true;
		}
		else if (ObjectID == BaseStageObject::OBJECT_ID::DARKNESS_WALL) {
			Output.isHitBrightnessWall_ = true;
		}
		else {
			// �@�������ɓ��������������߂��B
			Input.targetPos_ = impactPos + hitNormal * hitDistance;
			Output.isHitOrnament_ = true;
			Output.ornamentHitNormal_ = hitNormal;
		}

	}

	return Output;

}

BaseStage::ColliderOutput StageObjectMgr::Decision4Way(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID)
{

	/*===== �l�����ɓ����蔻�� =====*/

	// �����蔻��̌��ʕۑ��p�ϐ��B
	bool isHit = false;
	Vec3 impactPos;
	float hitDistance;
	Vec3 hitNormal;
	Vec2 hitUV;
	ObjectID;

	// �ђʖh�~�Ő��ʕ����ɂ����C���΂��B
	InputRayData.rayPos_ = Input.targetPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Input.targetInstance_.lock()->GetRotate());

	// �����蔻����s���B
	isHit = false;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// ��������������Y���̃T�C�Y����������������B
	isHit &= (hitDistance - Input.targetSize_.x_) <= 0;
	isHit &= 0 < hitDistance;

	// �������Ă����牟���߂��B
	if (isHit) {

		Vec3 pushBackVec = (Input.targetPos_ - impactPos).GetNormal();

		// �@�������ɓ��������������߂��B
		Input.targetPos_ = impactPos + pushBackVec * Input.targetSize_.x_;
		Output.resultPos_ = Input.targetPos_;
		Output.upVec_ = Input.targetUpVec_;

	}

	// �ђʖh�~�ō������ɂ����C���΂��B
	InputRayData.rayPos_ = Input.targetPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), Input.targetInstance_.lock()->GetRotate());

	// �����蔻����s���B
	isHit = false;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// ��������������Y���̃T�C�Y����������������B
	isHit &= (hitDistance - Input.targetSize_.x_) <= 0;
	isHit &= 0 < hitDistance;

	// �������Ă����牟���߂��B
	if (isHit) {

		Vec3 pushBackVec = (Input.targetPos_ - impactPos).GetNormal();

		// �@�������ɓ��������������߂��B
		Input.targetPos_ = impactPos + pushBackVec * Input.targetSize_.x_;
		Output.resultPos_ = Input.targetPos_;
		Output.upVec_ = Input.targetUpVec_;

	}

	// �ђʖh�~�ŉE�����ɂ����C���΂��B
	InputRayData.rayPos_ = Input.targetPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), Input.targetInstance_.lock()->GetRotate());

	// �����蔻����s���B
	isHit = false;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// ��������������Y���̃T�C�Y����������������B
	isHit &= (hitDistance - Input.targetSize_.x_) <= 0;
	isHit &= 0 < hitDistance;

	// �������Ă����牟���߂��B
	if (isHit) {

		// �@�������ɓ��������������߂��B
		Input.targetPos_ += hitNormal * (fabs(hitDistance - Input.targetSize_.x_));
		Output.resultPos_ = Input.targetPos_;
		Output.upVec_ = Input.targetUpVec_;

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
	if (axisOfRevolution.Length() != 0 && Vec3(HitNormal).Length() != 0) {

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]������B
		Output.matRot_ = quaternionMat;
		//PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, quaternionMat);

		// �@���x�N�g��
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// �N�H�[�^�j�I�����s��Ɏ����B
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// �v���C���[����]������B
			Output.matRot_ *= upQuaternionMat;
			//PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��x�N�g�����X�V�B
		Output.upVec_ = normal_;

		//���ʃx�N�g�����X�V�B
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Output.matRot_);

	}

	// ���������@�����ゾ�����ꍇ�͍s������������ĉ�]������B
	if (HitNormal == Vec3(0, 1, 0)) {

		// �v���C���[����]������B
		Output.matRot_ = DirectX::XMMatrixIdentity();
		//PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, DirectX::XMMatrixIdentity());

		// �@���x�N�g��
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// �N�H�[�^�j�I�����s��Ɏ����B
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// ��]������B
			Output.matRot_ *= upQuaternionMat;
			//PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- ��]�s������ƂɊe�x�N�g������] --*/

		// ��x�N�g�����X�V�B
		Output.upVec_ = normal_;

		//���ʃx�N�g�����X�V�B
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Output.matRot_);

	}

}