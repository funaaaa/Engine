#pragma once
#include <vector>
#include <memory>
#include "BaseStageObject.h"
#include "Singleton.h"
#include "BaseStage.h"
#include "FHelper.h"

class StageObjectMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<BaseStageObject>> objects_;



public:

	/*===== �����o�ϐ� =====*/

	// �ǉ�����B
	int AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);

	// �X�V����
	void Update(const int& Timer);

	// �����蔻��
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input);

	// �M�~�b�N�W��ǉ��B
	std::vector<std::shared_ptr<BaseStageObject>> GetGimmickData() { return objects_; }

	// �ړ��֌W
	void AddTrans(const int& Index, const Vec3& Trans);
	void ChangeTrans(const int& Index, const Vec3& Trans);

	// �T�C�Y�֌W
	void AddScale(const int& Index, const Vec3& Scale);
	void ChangeScale(const int& Index, const Vec3& Scale);

	// ��]�֌W
	void AddRotate(const int& Index, const Vec3& Rotate);
	void ChangeRotate(const int& Index, const Vec3& Rotate);

	// Instanse���\���A�\���֐�
	void NonDisplay(const int& Index);
	void Display(const int& Index);

	// �Q�b�^
	int GetBlasIndex(const int& Index) { return objects_[Index]->GetBLASIndex(); }
	int GetInstanceIndex(const int& Index) { return objects_[Index]->GetInstanceIndex(); }


private:

	// �X�e�[�W�A���Ƃ̓����蔻��
	BaseStage::ColliderOutput StageMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID);

	// �����I�u�W�F�N�g�Ƃ̓����蔻��
	BaseStage::ColliderOutput OrnamentMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output);

	// 4�����ɓ����蔻��
	BaseStage::ColliderOutput Decision4Way(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID);

	// �΂ߏ��̉�]
	void RotObliqueFloor(BaseStage::ColliderInput Input, const Vec3& HitNormal, BaseStage::ColliderOutput& Output);

};