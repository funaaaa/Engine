#pragma once
#include <array>
#include <memory>
#include "BaseStageObject.h"
#include "Singleton.h"
#include "BaseStage.h"
#include "FHelper.h"

class StageObjectMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::array<std::pair<std::shared_ptr<BaseStageObject>, bool>, 256> objects_;



public:

	/*===== �����o�ϐ� =====*/

	// �Z�b�e�B���O
	void Setting();

	// �ǉ�����B
	int AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const UINT& ShaderID);

	// �X�V����
	void Update(const int& Timer);

	// �w��̃C���f�b�N�X�̖@���x�N�g����ύX�B
	void ChangeNormalTexture(const int& Index, const int& NormalTexture);

	// �����蔻��
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input);

	// �M�~�b�N�W��ǉ��B
	std::array<std::pair<std::shared_ptr<BaseStageObject>, bool>, 256> GetGimmickData() { return objects_; }

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
	int GetBlasIndex(const int& Index) { return objects_[Index].first->GetBLASIndex(); }
	int GetInstanceIndex(const int& Index) { return objects_[Index].first->GetInstanceIndex(); }

	// �폜�B
	void DeleteIndex(const int& Index);
	void ChangeInstanceShaderID(std::weak_ptr<PolygonMeshInstance> Instance, const UINT& ShaderID);
	void ChangeInstanceShaderID(const int& Index, const UINT& ShaderID);


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