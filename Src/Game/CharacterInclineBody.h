#pragma once
#include "Vec.h"
#include "PlayerModel.h"

// �L�����N�^�[�̎ԑ̂��X�������邽�߂̏������܂Ƃ߂��N���X
class CharacterInclineBody {

private:

	/*===== �����o�ϐ� =====*/

	DirectX::XMVECTOR handleRotQ_;		// �n���h���̉�]��
	DirectX::XMVECTOR nowHandleRotQ_;	// �n���h���̉�]��
	DirectX::XMVECTOR boostRotQ_;		// �������̐��ʕ����ւ̉�]�̍s��
	DirectX::XMVECTOR nowBoostRotQ_;	// �������̐��ʕ����ւ̉�]�̍s��
	float handleAmount_;				// �n���h����
	const float  MAX_DRIFT_ROT = 1.6f;	// �h���t�g���̎ԑ̂̉�]�̍ő��
	float baseDriftRot_;				// �h���t�g���Ɏԑ̂���]������ۂ̕�Ԑ�
	float nowDriftRot_;					// �h���t�g���Ɏԑ̂���]������ۂ̌��ݗ�
	float baseBoostRot_;				// �u�[�X�g���̎ԑ̂̕����オ����Ԃ���ۂ̕⊮��
	float nowBoostRot_;					// �u�[�X�g���Ɏԑ̂̕����オ����Ԃ���ۂ̌��ݗ�
	float tireLollingAmount_;			// �^�C���������グ���
	int forwardTireLollingTimer_;		// �^�C���������グ��ۂ̃^�C�}�[
	const int FORWARD_TIMER_LOLLING_TIMER = 20;
	int driftRotTimer_;					// �h���t�g�̍ۂɎԑ̂���]�����邽�߂Ɏg�p����^�C�}�[�B���̒l���傫���Ȃ�Ύԑ̂̌X�����傫���Ȃ�B			
	const int MAX_DRIFT_ROT_TIMER = 10;
	bool isRotRightSide_ = false;		// �ǂ��瑤�Ƀh���t�g���Ă��邩�B�ԑ̂���]��������������߂�ۂɎg�p����B

public:

	// �ԑ̂̉�]�����ɕK�v�ȍX�V�p�f�[�^
	struct InputData {
		bool onGround_;				// �ݒu�t���O
		bool isDrift_;				// �h���t�g����
		bool isDriftJump_;			// �h���t�g�W�����v�����t���O
		bool isPlayer_;				// �v���C���[���t���O
		Vec3 pos_;					// �L�����̍��W
		Vec3 prevPos_;				// �L�����̑O�t���[�����W
		float boostSpeed_;			// �L�����̃u�[�X�g��
		float maxBoostSpeed_;		// �L�����̃u�[�X�g�ʂ̍ő�l
		float rotY_;				// �L������Y����]��
		PlayerModel playerModel_;	// �L�����̃��f�����
	};


public:

	CharacterInclineBody();
	void Init();
	void Update(const InputData& Input);

	// ��]�Ɏg�p���Ă���N�H�[�^�j�I��������������B
	void InitQuaternion();

	// �n���h���ʂ��擾
	float GetHandleAmount() { return handleAmount_; }
	// �n���h���ʂ��X�V
	inline void SetHandleAmount(float HandleAmount) { handleAmount_ = HandleAmount; }


};