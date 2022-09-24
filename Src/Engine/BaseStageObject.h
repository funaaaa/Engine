#pragma once
#include <memory>
#include <string>
#include <vector>
#include <wtypes.h>
#include "Vec.h"

class OBB;
class PolygonMeshInstance;
class BLAS;

class BaseStageObject {

public:

	/*===== �����o�ϐ� =====*/


	// �����蔻��ID
	enum class COLLISION_ID {
		OBB,	// OBB�̔���
		MESH,	// ���b�V���̔���
		NONE,	// �����蔻����s��Ȃ�
	};

	// �I�u�W�F�N�g��ID
	enum class OBJECT_ID {
		STAGE,			// �X�e�[�W
		STAGE_GRASS,	// �X�e�[�W�̑�
		WALL,			// ��
		GOAL,			// �S�[��
		MIDDLE_POINT,	// ���Ԓn�_
		ORNAMENT,		// �����I�u�W�F�N�g
		FLOATING_ORNAMENT,	// �ӂ�ӂ퓮�������I�u�W�F�N�g
		BOOST_GIMMICK,	// �����M�~�b�N
		ITEM_BOX,		// �A�C�e���{�b�N�X
		STEP_BOOST_GIMMICK,	// �i�������M�~�b�N
		INV_WALL,		// �����Ȃ���
	};


protected:

	std::weak_ptr<BLAS> blas_;			// BLAS�̃C���f�b�N�X
	std::weak_ptr<PolygonMeshInstance> instance_;		// Instance�̃C���f�b�N�X
	bool isActive_;		// �L�����t���O
	COLLISION_ID collisionID_;	// �����蔻���ID
	OBJECT_ID objID_;			// �I�u�W�F�N�g��ID
	std::shared_ptr<OBB> obb_;	// �����蔻��pOBB


public:

	/*===== �����o�֐� =====*/

	virtual void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance) = 0;
	virtual void Destroy() = 0;
	virtual void Update(const int& Timer) = 0;
	virtual void Disable(const int& TimerToActivation) = 0;

	// �L�����B
	inline void Activate() { isActive_ = true; }
	// �������B
	inline void Invalidate() { isActive_ = false; }

	// �\���A��\��
	void Display();
	void NonDisplay();

	// �Q�b�^
	int GetBLASIndex();
	int GetInstanceIndex();
	std::weak_ptr<PolygonMeshInstance> GetInstance() { return instance_; }
	inline const bool& GetIsActive() { return isActive_; }
	inline std::shared_ptr<OBB> GetOBB() { return obb_; }
	inline const COLLISION_ID GetCollisionID() { return collisionID_; }
	inline const OBJECT_ID GetObjectID() { return objID_; }

	// �@���}�b�v��ύX�B
	void ChangeNormalTexture(const int& Index);

	// �ړ��֌W
	void AddTrans(const Vec3& Trans);
	void ChangeTrans(const Vec3& Trans);

	// �T�C�Y�֌W
	void AddScale(const Vec3& Scale);
	void ChangeScale(const Vec3& Scale);

	// ��]�֌W
	void AddRotate(const Vec3& Rotate);
	void ChangeRotate(const Vec3& Rotate);

	// �폜�B
	void Delete();

protected:

	// ���N���X�̃����o�ϐ��̏�����
	void BasicInit(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance);

};