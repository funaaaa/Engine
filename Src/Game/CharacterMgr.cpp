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
#include "FHelper.h"
#include "PolygonInstance.h"
#include "PolygonInstanceRegister.h"
#include "Camera.h"
void CharacterMgr::Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish)
{

	/*===== 更新処理 =====*/

	for (auto& index : character_) {

		index->Update(Stage, character_, IsBeforeStart, IsGameFinish);

		// 視錐台カリング
		bool inScreen = FHelper::CheckInScreen(index->GetPos(), 100.0f, 100.0f, Camera::Ins()->matView_, Camera::Ins()->matPerspective_);

		// カリングする。
		if (inScreen) {

			index->GetPlayerModel().Display();

		}
		else {

			index->GetPlayerModel().NonDisplay();

		}

	}

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::SettingStartPos()
{

	/*===== 初期地点を設定する =====*/

	int counter = 0;
	for (auto& index : character_) {

		index->SettingStartPos(counter);

	}

}

void CharacterMgr::AddChara(int CharaID, bool IsPlayer, int Level, int CharaPersonality)
{

	/*===== キャラクターを追加 =====*/

	character_.emplace_back(std::make_shared<Character>(static_cast<Character::CHARA_ID>(CharaID), static_cast<int>(character_.size()), Level, CharaPersonality));

	if (IsPlayer) playerIndex_ = static_cast<int>(character_.size()) - 1;

}

bool CharacterMgr::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<CharacterTireMask::TireMaskUV>& TireMaskUVData)
{

	/*===== タイヤ痕チェック =====*/

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

	/*===== 座標を取得 =====*/

	return character_.at(Index)->GetPos();;

}

bool CharacterMgr::CheckGoal(bool& IsPlayer)
{

	/*===== ゴールしたかどうかをチェック =====*/

	bool isGoal = false;

	for (auto& index : character_) {

		if (index->GetRapCount() < 3) continue;

		isGoal = true;
		IsPlayer = index->GetIsPlayer();

		break;

	}

	return isGoal;
}