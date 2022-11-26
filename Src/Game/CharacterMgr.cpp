#include "CharacterMgr.h"
#include "Character.h"

#pragma warning(push)
#pragma warning(disable:4324)

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

	}

	character_.resize(0);

}

#include "OBB.h"
#include "imgui.h"
#include "FHelper.h"
#include "PolygonInstance.h"
void CharacterMgr::Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish)
{

	/*===== �X�V���� =====*/

	for (auto& index : character_) {

		index->Update(Stage, character_, IsBeforeStart, IsGameFinish);

	}

	static int charaIndex = 0;
	ImGui::SliderInt("CharaIndex", &charaIndex, 0, static_cast<int>(character_.size()) - 1);
	ImGui::DragFloat("PosX", &character_[charaIndex]->pos_.x_, 1.0f);
	ImGui::DragFloat("PosZ", &character_[charaIndex]->pos_.z_, 1.0f);

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::SettingStartPos()
{

	/*===== �����n�_��ݒ肷�� =====*/

	int counter = 0;
	for (auto& index : character_) {

		// �v���C���[��������B
		if (index->charaID_ == Character::CHARA_ID::P1) {
			index->defPos_ = index->PLAYER_DEF_POS;
			continue;
		}

		// ����ȊO�̃L�����͏��Ԃɏꏊ��ݒ肷��B
		index->defPos_ = index->GHOST_DEF_POS[counter];
		++counter;

	}

}

void CharacterMgr::AddChara(int CharaID, bool IsPlayer, int Param)
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