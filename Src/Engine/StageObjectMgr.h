#pragma once
#include <array>
#include <memory>
#include "BaseStageObject.h"
#include "Singleton.h"
#include "BaseStage.h"
#include "FHelper.h"
#include "BLAS.h"

class CharacterMgr;

// �X�e�[�W�ɔz�u�����I�u�W�F�N�g���Ǘ�����N���X�BBaseStage�����̃N���X��1���B
class StageObjectMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::array<std::pair<std::shared_ptr<BaseStageObject>, bool>, 512> objects_;	// �I�u�W�F�N�g�z�� fist->�I�u�W�F�N�g second->�L�����t���O



public:

	/*===== �����o�ϐ� =====*/

	// �Z�b�e�B���O
	void Setting();

	// �ǉ�����B
	int AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, UINT ShaderID, bool IsOpaque = false, bool IsNewGenerate = false);
	int AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::wstring& ModelPath, const std::wstring& HitGroupName, UINT ShaderID, bool IsOpaque = false);

	// �X�V����
	void Update(int Timer, std::weak_ptr<CharacterMgr> Character);

	// �w��̃C���f�b�N�X�̃}�b�v�p�e�N�X�`����ύX�B
	void ChangeMapTexture(int Index, int TextureIndex, BLAS::MAP_PARAM MapParam);

	// �����蔻��
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input);

	// �M�~�b�N�W��ǉ��B
	std::array<std::pair<std::shared_ptr<BaseStageObject>, bool>, 512> GetGimmickData() { return objects_; }

	// �ړ��֌W
	void AddTrans(int Index, const Vec3& Trans);
	void ChangeTrans(int Index, const Vec3& Trans);

	// �T�C�Y�֌W
	void AddScale(int Index, const Vec3& Scale);
	void ChangeScale(int Index, const Vec3& Scale);

	// ��]�֌W
	void AddRotate(int Index, const Vec3& Rotate);
	void ChangeRotate(int Index, const Vec3& Rotate);

	// Instanse���\���A�\���֐�
	void NonDisplay(int Index);
	void Display(int Index);

	// �Q�b�^
	int GetBlasIndex(int Index) { return objects_[Index].first->GetBLASIndex(); }
	int GetInstanceIndex(int Index) { return objects_[Index].first->GetInstanceIndex(); }

	// �폜�B
	void DeleteIndex(int Index);
	void ChangeInstanceShaderID(std::weak_ptr<PolygonMeshInstance> Instance, UINT ShaderID);
	void ChangeInstanceShaderID(int Index, UINT ShaderID);

	// �w���BLAS��UV���w���BLAS��SUBUV�ɑ������B
	void AssignmentUVToSubUV(int AssigningBLASIndex, int AssignedBLASIndex);

private:

	// �X�e�[�W�A���Ƃ̓����蔻��
	BaseStage::ColliderOutput StageMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID);

	// �����I�u�W�F�N�g�Ƃ̓����蔻��
	BaseStage::ColliderOutput OrnamentMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID);

	// 4�����ɓ����蔻��
	BaseStage::ColliderOutput Decision4Way(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID);

	// �΂ߏ��̉�]
	void RotObliqueFloor(BaseStage::ColliderInput Input, const Vec3& HitNormal, BaseStage::ColliderOutput& Output);

};