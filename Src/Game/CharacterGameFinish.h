#pragma once
#include "Vec.h"
#include "PlayerModel.h"
#include <memory>

class CharacterInclineBody;

// �L�����N�^�[�̃Q�[���I�����ɍs���������܂Ƃ߂��N���X
class CharacterGameFinish {

private:

	/*===== �����o�ϐ� =====*/

	DirectX::XMMATRIX gameFinishTriggerMatRot_;
	Vec3 gameFinishTruggerForardVec_;// �Q�[���I�����̐��ʃx�N�g��
	float gameFinishTriggerRotY_;	// �Q�[�����I������u�Ԃ̉�]��
	float gameFinishEasingTimer_;	// �Q�[���I�������o�Ɏg�p����C�[�W���O�^�C�}�[
	float gameFinishRotStopAmount_;	// ���S�ɒ�~�����ۂ̉�]�ʁB
	float gameFinishRotStopReturnAmount_;	// ���S�ɒꂵ���ۂɌ��̈ʒu�ɖ߂����߂̉�]�ʁB
	const float GAME_FINISH_EASING_TIMER = 0.01f;
	const float GAME_FINISH_STOP_ROT = 2.0f;
	const float GAME_FINISH_STOP_ROT_LIMIT = 0.3f;
	bool isGameFinish_;				// �Q�[�����I������t���O
	bool isPrevGameFinish_;			// �O�t���[���̃Q�[�����I������t���O
	bool isGameFinishDriftLeft_;	// �Q�[���I�����o�łǂ���Ƀh���t�g���邩�B t:�� f:�E

public:

	// �Q�[���I�����̊�{�I�ȍX�V�����ɕK�v�ȃf�[�^
	struct UpdateInputData {
		float maxSpeed_;			// �L�����N�^�[�̍ő�ړ����x
		PlayerModel playerModel_;	// �L�����N�^�[�̃��f���f�[�^
		std::weak_ptr<CharacterInclineBody> inclineBody_;	// �L�����N�^�[�̉�]�Ɏg�p���鏈�����܂Ƃ߂��N���X
	};


public:

	/*===== �����o�֐� =====*/

	CharacterGameFinish();
	void Init();

	// �Q�[���I�����̊�{�I�ȍX�V���� ��Ƀ^�C�}�[�̍X�V
	void Update(const UpdateInputData& Input, int& BoostTimer, float& Speed, float& RotY, Vec3& ForwardVec);

	// �Q�[���I������AKIRA�̃h���t�g���C���[�W�������o�̏��� ��ɉ�]�̍X�V
	void AKIRAERotation(PlayerModel& Model);

	// �Q�[���I�����̃t���O���X�V�B
	void UpdateGameFinishFlag(bool IsGameFinish);

	// �Q�[���I�����̃g���K�[������擾�B
	inline bool GetIsGameFinishTrigger() { return isGameFinish_ && !isPrevGameFinish_; }

	// �Q�[�����I�����Ă��邩���`�F�b�N����B�Q�[���I�����t���O�Ƃ͕ʁB�������ł͑O�t���[���ł��Q�[�����I�����Ă邩���m�F����B
	inline bool CheckGameFinish() { return isGameFinish_ && isPrevGameFinish_; }

	// �e��Q�b�^
	inline bool GetIsGameFinish() { return isGameFinish_; }
	inline bool GetIsPrevGameFinish() { return isPrevGameFinish_; }
	bool GetIsGameFinishDriftLeft() { return isGameFinishDriftLeft_; }
	float GetGameFinishEasingTimer() { return gameFinishEasingTimer_; }
	DirectX::XMMATRIX GetGameFinishTriggerMatRot() { return gameFinishTriggerMatRot_; }

};