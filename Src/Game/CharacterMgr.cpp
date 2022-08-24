#include "CharacterMgr.h"
#include "Character.h"

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

		index.reset();

	}

	character_.resize(0);

}

void CharacterMgr::Update(std::weak_ptr<BaseStage> Stage, RayConstBufferData& ConstBufferData, int& RapCount, bool& IsPassedMiddlePoint)
{

	/*===== 更新処理 =====*/

	for (auto& index : character_) {

		index->Update(Stage, ConstBufferData, IsPassedMiddlePoint, RapCount);

	}

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::AddChara(const int& CharaID, const bool& IsPlayer)
{

	/*===== キャラクターを追加 =====*/

	character_.emplace_back(std::make_shared<Character>(static_cast<Character::CHARA_ID>(CharaID)));

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
