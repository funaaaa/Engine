#pragma once
#include "Vec.h"
#include <memory>

class OBB;
class BaseStage;
class PolygonMeshInstance;
class BLAS;

class ShellObject {

public:

	enum class SHELL_ID {

		FORWARD_THROW,	// �������Ă�����
		BEHIND_THROW,
		BEHIND,	// ���ɕێ�����Ă�����

	};

private:

	/*===== �����o�ϐ� =====*/

	// ���f���f�[�^�֌W
	std::weak_ptr<BLAS> blas_;							// BLAS�̃C���f�b�N�X
	std::weak_ptr<PolygonMeshInstance> instance;		// INSTANCE�̃C���f�b�N�X
	std::weak_ptr<PolygonMeshInstance> charaInstance;	// �ێ����Ă���L������INSTANCE�̃C���f�b�N�X

	// �I�u�W�F�N�g�ŗL�̃p�����[�^�[
	Vec3 pos_;				// ���W
	Vec3 prevPos_;			// �O�t���[�����W
	Vec3 forwardVec_;		// �ړ������x�N�g��
	Vec3 size_;				// �T�C�Y
	float rotY_;			// Y����]
	float speed_;			// �ړ����x
	float gravity_;			// �d��
	int exitTimer_;			// ���ł܂ł̎���
	bool isActive_;			// �L�����t���O
	bool onGround_;			// �ڒn�t���O
	bool isDestroyed_;		// �C���X�^���X���j���ς݂��ǂ���

	SHELL_ID shellID_;		// �b����ID

	std::shared_ptr<OBB> obb_;		// �����蔻��pOBB

	const float SPEED = 50.0f;
	const float SCALE = 20.0f;
	const float MAX_GRAV = 10.0f;	// �d�͂̍ő��
	const float ADD_GRAV = 1.0f;	// �d�͂̉��Z��
	const int EXIT_TIMER = 180;		// ���ł܂ł̎���


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	ShellObject();

	// �j������
	void Destroy();

	// ��������
	void Generate(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, std::weak_ptr<PolygonMeshInstance> CharaInstance);

	// �X�V����
	void Update(std::weak_ptr<BaseStage> StageData);

	// OBB�Ƃ̓����蔻�� �L�����N�^�[�Ƃ̓����蔻��Ŏg�p����B
	bool CheckHitOBB(std::weak_ptr<OBB> CharaOBB);

	bool GetIsActive() { return isActive_; }

private:

	// �����蔻��
	void CheckHit(std::weak_ptr<BaseStage> StageData);

};