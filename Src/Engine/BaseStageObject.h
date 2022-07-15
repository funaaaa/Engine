#pragma once
#include <memory>
#include <string>
#include <vector>
#include <wtypes.h>
#include "Vec.h"

class OBB;

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
		GOAL,			// �S�[��
		MIDDLE_POINT,	// ���Ԓn�_
		ORNAMENT,		// �����I�u�W�F�N�g
		FLOATING_ORNAMENT,	// �ӂ�ӂ퓮�������I�u�W�F�N�g
		BOOS_GIMMICK,	// �����M�~�b�N
	};


protected:

	int blasIndex_;		// BLAS�̃C���f�b�N�X
	int insIndex_;		// Instance�̃C���f�b�N�X
	bool isActive_;		// �L�����t���O
	COLLISION_ID collisionID_;	// �����蔻���ID
	OBJECT_ID objID_;			// �I�u�W�F�N�g��ID
	std::shared_ptr<OBB> obb_;	// �����蔻��pOBB


public:

	/*===== �����o�֐� =====*/

	virtual void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID) = 0;
	virtual void Destroy() = 0;
	virtual void Update(const int& Timer) = 0;

	// �L�����B
	inline void Activate() { isActive_ = true; }
	// �������B
	inline void Invalidate() { isActive_ = false; }

	// �Q�b�^
	inline const int& GetBLASIndex() { return blasIndex_; }
	inline const int& GetINSTANCEIndex() { return insIndex_; }
	inline const bool& GetIsActive() { return isActive_; }
	inline std::shared_ptr<OBB> GetOBB() { return obb_; }

	// �ړ��֌W
	void AddTrans(const Vec3& Trans);
	void ChangeTrans(const Vec3& Trans);

	// �T�C�Y�֌W
	void AddScale(const Vec3& Scale);
	void ChangeScale(const Vec3& Scale);

	// ��]�֌W
	void AddRotate(const Vec3& Rotate);
	void ChangeRotate(const Vec3& Rotate);

protected:

	// ���N���X�̃����o�ϐ��̏�����
	void BasicInit(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID);

};