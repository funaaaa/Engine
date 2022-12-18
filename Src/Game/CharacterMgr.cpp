#include "CharacterMgr.h"
#include "Character.h"

#pragma warning(push)
#pragma warning(disable:4324)

CharacterMgr::CharacterMgr()
{

	/*===== �R���X�g���N�^ =====*/

	playerIndex_ = 0;
	playerRaking_ = 0;

}

void CharacterMgr::Init()
{

	/*===== ������ =====*/

	playerIndex_ = 0;

	for (auto& index : character_) {

		index->DeleteInstance();

	}

	character_.resize(0);
	playerRaking_ = 0;

}

#include "OBB.h"
#include "imgui.h"
#include "FHelper.h"
#include "PolygonInstance.h"
#include "PolygonInstanceRegister.h"
#include "Camera.h"
#include <algorithm>
#include <string>
void CharacterMgr::Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish)
{

	/*===== �X�V���� =====*/

	for (auto& index : character_) {

		index->Update(Stage, character_, IsBeforeStart, IsGameFinish);

		// ������J�����O
		bool inScreen = FHelper::CheckInScreen(index->GetPos(), 100.0f, 100.0f, Camera::Ins()->matView_, Camera::Ins()->matPerspective_);

		// �J�����O����B
		if (inScreen) {

			index->GetPlayerModel().Display();

		}
		else {

			index->GetPlayerModel().NonDisplay();

		}

		rankingCount_[static_cast<int>(&index - &character_[0])] = std::pair<int,int>(index->GetHitRankingCount(), index->GetHitRankingelapsedTime_());

	}

	// �����L���O���~���Ń\�[�g
	std::sort(rankingCount_.rbegin(), rankingCount_.rend());

	// �v���C���[�L�����̃����L���O�𒲂ׂ�B
	prevPlayerRanking_ = playerRaking_;
	for (auto& index : rankingCount_) {

		// �v���C���[�L�����Ɠ�����������
		if (!(character_[playerIndex_]->GetHitRankingCount() == index.first && character_[playerIndex_]->GetHitRankingelapsedTime_() == index.second)) continue;

		playerRaking_ = static_cast<int>(&index - &rankingCount_[0]);
		break;

	}

	ImGui::DragInt("Character", &playerRaking_);

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::Setting()
{

	/*===== ���X�̐ݒ菈�� =====*/

	int counter = 0;
	for (auto& index : character_) {

		index->SettingStartPos(counter);

	}

	rankingCount_.resize(static_cast<int>(character_.size()));

}

void CharacterMgr::AddChara(int CharaID, bool IsPlayer, int Level, int CharaPersonality)
{

	/*===== �L�����N�^�[��ǉ� =====*/

	character_.emplace_back(std::make_shared<Character>(static_cast<Character::CHARA_ID>(CharaID), static_cast<int>(character_.size()), Level, CharaPersonality));

	if (IsPlayer) playerIndex_ = static_cast<int>(character_.size()) - 1;

}

bool CharacterMgr::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<CharacterTireMask::TireMaskUV>& TireMaskUVData)
{

	/*===== �^�C�����`�F�b�N =====*/

	bool isTireMask = false;

	for (auto& index : character_) {

		CharacterTireMask::TireMaskUV tireMaskUVBuff;

		isTireMask |= index->CheckTireMask(BaseStageData, tireMaskUVBuff);

		TireMaskUVData.emplace_back(tireMaskUVBuff);

	}

	return isTireMask;
}

Vec3 CharacterMgr::GetPos(int Index)
{

	/*===== ���W���擾 =====*/

	return character_.at(Index)->GetPos();;

}

bool CharacterMgr::CheckGoal(bool& IsPlayer)
{

	/*===== �S�[���������ǂ������`�F�b�N =====*/

	bool isGoal = false;

	for (auto& index : character_) {

		if (index->GetRapCount() < 3) continue;

		isGoal = true;
		IsPlayer = index->GetIsPlayer();

		break;

	}

	return isGoal;
}
