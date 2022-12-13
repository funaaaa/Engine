#pragma once
#include "Vec.h"
#include "BaseOperationObject.h"
#include "PlayerModel.h"
#include "PlayerTire.h"
#include <array>
#include <vector>
#include <memory>

class CharacterInclineBody;
class CharacterGameFinish;

// �L�����N�^�[�̃h���t�g�Ɋւ��鏈�����܂Ƃ߂��N���X
class CharacterDrift {

private:

	/*===== �����o�ϐ� =====*/

	Vec3 driftJumpVec_;					// �h���t�g�W�����v��������� ��{�I�ɏ��̖@���ɂȂ�B
	float driftJumpSpeed_;				// �h���t�g�W�����v������ۂ̈ړ����x�B�ŏ������傫�����Ă��񂾂񏬂������邱�ƂŃW�����v����B
	const float DRIFT_JUMP_SPEED = 6.0f;
	const float SUB_DRIFT_JUMP_SPEED = 0.6f;
	int driftTimer_;					// �h���t�g�̌o�ߎ���
	int turningIndicatorTimer_;			// �E�C���J�[���`�J�`�J����^�C�}�[
	int beforeStartSmokeTimer_;			// �Q�[���J�n�O�ŃA�N�Z���̓��͂��������ۂɏo�鉌�̐����X�p���B
	const int BEFORE_START_SMOKE_TIMER = 5;
	const int TURNING_INDICATOR_TIMER = 30;
	bool isDriftRight_;					// �h���t�g���E�����ǂ����B
	bool isInputLTPrev_;				// �O�t���[����LT�������ꂽ���ǂ���
	bool isInputLT_;					// LT�������ꂽ���ǂ����B
	bool isDriftJump_;					// �h���t�g�O�̃W�����v�����ǂ����B
	bool isDriftJumpPrev_;				// �h���t�g�O�̃W�����v�����ǂ����̑OF�t���O�B
	bool isDrift_;						// �h���t�g��Ԃ��ǂ����B
	bool isTireMask_;					// �^�C�������������ނ��ǂ����̃t���O�B
	bool IsTurningIndicatorRed_;		// �E�C���J�[�̐F���Ԃ��ǂ����B
	const std::array<int, 3> DRIFT_TIMER = { 30,90,160 };			// ���x�����Ƃ̃h���t�g���x�����オ��܂ł̃^�C�}�[
	const std::array<float, 3> DRIFT_BOOST = { 10.0f,20.0f,30.0f };	// ���x�����Ƃ̃h���t�g�u�[�X�g�̗�

public:

	// ���͂̍X�V�����ɕK�v�ȃf�[�^ ���͗p
	struct InputData {
		Vec3 upVec_;
		int canNotMoveTimer_;
		float handleNormal_;
		bool isBeforeStart_;
		bool onGround_;
	};

	struct DriftParticleInData {
		Vec3 forwardVec_;
		int charaIndex_;
		float maxBoostSpeed_;
		bool isGameFinish_;
		bool isBeforeStart_;
		bool onGround_;
		bool onGroundPrev_;
		bool isAccel_;
		bool IsJumpAction_;
	};
	struct DriftParticleOutData {
		int& boostTimer_;
		float& boostSpeed_;
		PlayerModel& playerData_;
		std::weak_ptr<CharacterGameFinish> gameFinish_;
		DriftParticleOutData(int& BoostTimer, float& BoostSpeed, PlayerModel& PlayerData, std::weak_ptr<CharacterGameFinish> GameFinish)
			: boostTimer_(BoostTimer), boostSpeed_(BoostSpeed), playerData_(PlayerData), gameFinish_(GameFinish) {};
	};

public:

	/*===== �����o�֐� =====*/

	CharacterDrift();
	void Init();
	void Input(const InputData& Input, BaseOperationObject::Operation& Operation, std::vector<std::shared_ptr<PlayerTire>>& Tires, PlayerModel& PlayerModel, float& RotY, float& BoostSpeed, Vec3& ForwardVec);
	void Update(std::weak_ptr<CharacterInclineBody> InclineBody, Vec3& Pos, Vec3& PrevPos, bool OnGround);


	// �h���t�g���ɏo��p�[�e�B�N���̍X�V���� Character�̍X�V�����̍Ō�Ɏ����Ă��邽�߂�Update�ƕ����B
	void UpdateDriftParticle(const DriftParticleInData& InData, DriftParticleOutData& OutData);

	// �Q�b�^
	inline bool GetIsDrift() { return isDrift_; }
	inline bool GetIsTireMask() { return isTireMask_; }
	bool GetIsDriftRight() { return isDriftRight_; }
	bool GetIsDriftJump() { return isDriftJump_; }


};