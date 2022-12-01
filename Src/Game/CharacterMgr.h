#pragma once
#include <memory>
#include <vector>
#include "Character.h"
#include "ConstBuffers.h"
#include "FHelper.h"

class Character;
class BaseStage;

class CharacterMgr {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<Character>> character_;
	int playerIndex_;


public:

	/*===== メンバ関数 =====*/

	CharacterMgr();
	void Init();
	void Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish);
	void Draw();

	// 初期地点を設定。
	void SettingStartPos();
	void AddChara(int CharaID, bool IsPlayer, int Param = 0, int CharaPersonality = 0);
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<Character::TireMaskUV>& TireMaskUVData);
	inline std::weak_ptr<Character> GetPlayerIns() { return character_[playerIndex_]; }

	// いずれかのキャラがゴールしたかをチェックする。
	bool CheckGoal();

};