#pragma once
#include <memory>
#include <vector>
#include "Character.h"
#include "ConstBuffers.h"
#include "FHelper.h"
#include "CharacterTireMask.h"

class Character;
class BaseStage;

// �L�����N�^�[���Ǘ�����N���X�BAddChara�ŃL������ID��p�����[�^�[��n�����ƂŃv���C���[��AI�𐶐�����B
class CharacterMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<Character>> character_;
	std::vector<std::pair<int, int>> rankingCount_;	// �Ԃ����񃉃��L���O�ǂɂ����������Q�Ɨp
	int prevPlayerRanking_;				// �O�t���[���̃v���C���[�����L���O
	int playerRaking_;					// �v���C���[�̃����L���O
	int playerIndex_;


public:

	/*===== �����o�֐� =====*/

	CharacterMgr();
	void Init();
	void Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish);
	void Draw();

	// ���X�̐ݒ菈��
	void Setting();
	void AddChara(int CharaID, bool IsPlayer, int Param = 0, int CharaPersonality = 0);
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<CharacterTireMask::TireMaskUV>& TireMaskUVData);
	inline std::weak_ptr<Character> GetPlayerIns() { return character_[playerIndex_]; }

	// �Q�b�^
	Vec3 GetPos(int Index);
	bool GetIsChangePlayerRanking() { return prevPlayerRanking_ != playerRaking_; }
	int GetPlayerRanking() { return playerRaking_; }

	// �����ꂩ�̃L�������S�[�����������`�F�b�N����B
	bool CheckGoal(bool& IsPlayer);

};