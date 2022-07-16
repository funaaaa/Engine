#pragma once
#include <memory>
#include "Vec.h"

class StageObjectMgr;
class OBB;

class BaseStage {

protected:

	/*====== �����o�ϐ� =====*/

	std::shared_ptr<StageObjectMgr> stageObjectMgr_;	// �X�e�[�W�I�u�W�F�N�g

public:

	// �����蔻����͍\����
	struct ColliderInput {
		Vec3 targetPrevPos_;			// �����蔻����s���I�u�W�F�N�g�̑O�t���[���̍��W
		Vec3 targetPos_;				// �����蔻����s���I�u�W�F�N�g�̍��W
		Vec3 targetSize_;				// �����蔻����s���I�u�W�F�N�g�̃T�C�Y
		std::weak_ptr<OBB> targetOBB_;	// �����蔻����s���I�u�W�F�N�g��OBB
		float targetRotY_;				// �����蔻����s���I�u�W�F�N�g��Y���̉�]��
		int targetInsIndex_;			// �����蔻����s���I�u�W�F�N�g�̃C���X�^���X�̃C���f�b�N�X
	};
	// �����蔻��o�͍\����
	struct ColliderOutput {
		Vec3 resultPos_;			// �����蔻�茋�ʂ̍��W
		Vec3 forwardVec_;			// ���ʃx�N�g��
		Vec3 upVec_;				// ��x�N�g��
		bool isHitStage_;			// �X�e�[�W�Ɠ�����������
		bool isHitStageGrass_;		// �X�e�[�W�̑��Ɠ�����������
		bool isHitGoal_;			// �S�[���Ɠ�����������
		bool isHitMiddlePoint_;		// ���Ԓn�_�Ɠ�����������
		bool isHitBoostGimmick_;	// �u�[�X�g�M�~�b�N�Ɠ�����������
		bool isHitOrnament_;		// �����I�u�W�F�N�g�Ɠ�����������
	};


public:

	/*===== �����o�֐� =====*/

	virtual void Setting() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual ColliderOutput Collider(ColliderInput Input) = 0;

	// �S�[���̕\���A��\��
	virtual void DisplayGoal() = 0;
	virtual void NonDisplayGoal() = 0;

};