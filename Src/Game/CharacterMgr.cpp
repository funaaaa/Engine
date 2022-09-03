#include "CharacterMgr.h"
#include "Character.h"

CharacterMgr::CharacterMgr()
{

	/*===== �R���X�g���N�^ =====*/

	playerIndex_ = 0;

}

void CharacterMgr::Init()
{

	/*===== ������ =====*/

	playerIndex_ = 0;

	for (auto& index : character_) {

		index->DeleteInstance();
		index.reset();

	}

	character_.resize(0);

}

void CharacterMgr::Update(std::weak_ptr<BaseStage> Stage, RayConstBufferData& ConstBufferData, const bool& IsBeforeStart, const bool& IsGameFinish)
{

	/*===== �X�V���� =====*/

	for (auto& index : character_) {

		index->Update(Stage, ConstBufferData, IsBeforeStart, IsGameFinish);

	}

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::AddChara(const int& CharaID, const bool& IsPlayer, int Param)
{

	/*===== �L�����N�^�[��ǉ� =====*/

	character_.emplace_back(std::make_shared<Character>(static_cast<Character::CHARA_ID>(CharaID), static_cast<int>(character_.size()), Param));

	if (IsPlayer) playerIndex_ = static_cast<int>(character_.size()) - 1;

}

bool CharacterMgr::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<Character::TireMaskUV>& TireMaskUVData)
{

	/*===== �^�C�����`�F�b�N =====*/

	bool isTireMask = false;

	for (auto& index : character_) {

		Character::TireMaskUV tireMaskUVBuff;

		isTireMask |= index->CheckTireMask(BaseStageData, tireMaskUVBuff);

		TireMaskUVData.emplace_back(tireMaskUVBuff);

	}

	return isTireMask;
}

bool CharacterMgr::CheckGoal()
{

	bool isGoal = false;

	for (auto& index : character_) {

		if (index->GetRapCount() < 3) continue;

		isGoal = true;

		break;

	}

	return isGoal;
}
