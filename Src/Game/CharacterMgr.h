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

	// レイとメッシュの当たり判定の際に使用する、レイの開始地点と射出方向をまとめた関数。
	struct RayPosDir {
		Vec3 pos_;
		Vec3 dir_;
	};


public:

	/*===== メンバ関数 =====*/

	CharacterMgr();
	void Init();
	void Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish);
	void Draw();

	void AddChara(int CharaID, bool IsPlayer, int Param = 0);
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<Character::TireMaskUV>& TireMaskUVData);
	inline std::weak_ptr<Character> GetPlayerIns() { return character_[playerIndex_]; }

	// いずれかのキャラがゴールしたかをチェックする。
	bool CheckGoal();

private:

	// 指定の車のレイの射出位置を求める。
	void CalRayPosDir(int Index, RayPosDir& FrontRight, RayPosDir& FrontLeft, RayPosDir& BehindRight, RayPosDir& BehindLeft);

	// 車との当たり判定
	void CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter);

};