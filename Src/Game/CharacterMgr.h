#pragma once
#include <memory>
#include <vector>
#include "Character.h"
#include "ConstBuffers.h"
#include "FHelper.h"
#include "CharacterTireMask.h"

class Character;
class BaseStage;

// キャラクターを管理するクラス。AddCharaでキャラのIDやパラメーターを渡すことでプレイヤーやAIを生成する。
class CharacterMgr {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<Character>> character_;
	std::vector<std::pair<int, int>> rankingCount_;	// 車が何回ランキング壁にあたったか参照用
	int prevPlayerRanking_;				// 前フレームのプレイヤーランキング
	int playerRaking_;					// プレイヤーのランキング
	int playerIndex_;


public:

	/*===== メンバ関数 =====*/

	CharacterMgr();
	void Init();
	void Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish);
	void Draw();

	// 諸々の設定処理
	void Setting();
	void AddChara(int CharaID, bool IsPlayer, int Param = 0, int CharaPersonality = 0);
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<CharacterTireMask::TireMaskUV>& TireMaskUVData);
	inline std::weak_ptr<Character> GetPlayerIns() { return character_[playerIndex_]; }

	// ゲッタ
	Vec3 GetPos(int Index);
	bool GetIsChangePlayerRanking() { return prevPlayerRanking_ != playerRaking_; }
	int GetPlayerRanking() { return playerRaking_; }

	// いずれかのキャラがゴールしたかをチェックする。
	bool CheckGoal(bool& IsPlayer);

};