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
	int playerIndex_;


public:

	/*===== �����o�֐� =====*/

	CharacterMgr();
	void Init();
	void Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish);
	void Draw();

	// �����n�_��ݒ�B
	void SettingStartPos();
	void AddChara(int CharaID, bool IsPlayer, int Param = 0, int CharaPersonality = 0);
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<CharacterTireMask::TireMaskUV>& TireMaskUVData);
	inline std::weak_ptr<Character> GetPlayerIns() { return character_[playerIndex_]; }

	// �����ꂩ�̃L�������S�[�����������`�F�b�N����B
	bool CheckGoal();

};