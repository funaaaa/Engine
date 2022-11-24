#include "CharacterMgr.h"
#include "Character.h"

#pragma warning(push)
#pragma warning(disable:4324)

CharacterMgr::CharacterMgr()
{

	/*===== コンストラクタ =====*/

	playerIndex_ = 0;

}

void CharacterMgr::Init()
{

	/*===== 初期化 =====*/

	playerIndex_ = 0;

	for (auto& index : character_) {

		index->DeleteInstance();

	}

	character_.resize(0);

}

#include "OBB.h"
#include "imgui.h"
void CharacterMgr::Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish)
{

	/*===== 更新処理 =====*/

	for (auto& index : character_) {

		index->Update(Stage, IsBeforeStart, IsGameFinish);

	}

	bool isHit = false;
	for (auto& src : character_) {

		for (auto& dst : character_) {

			// 同じオブジェクトだったら処理を飛ばす。
			if (src == dst) continue;

			// 当たり判定を行う。
			isHit |= src->obb_->CheckHitOBB(dst->obb_);

		}

	}
	ImGui::Checkbox("IsHit", &isHit);

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::AddChara(int CharaID, bool IsPlayer, int Param)
{

	/*===== キャラクターを追加 =====*/

	character_.emplace_back(std::make_shared<Character>(static_cast<Character::CHARA_ID>(CharaID), static_cast<int>(character_.size()), Param));

	if (IsPlayer) playerIndex_ = static_cast<int>(character_.size()) - 1;

}

bool CharacterMgr::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<Character::TireMaskUV>& TireMaskUVData)
{

	/*===== タイヤ痕チェック =====*/

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
