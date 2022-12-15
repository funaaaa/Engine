#pragma once
#include "BaseStageObject.h"

// アイテムボックスクラス
class ItemBoxObject : public BaseStageObject{

private:

	/*===== メンバ変数 =====*/

	int timerToActivation_;	// 有効化までのタイマー
	int activationTimer_;	// 有効化までのタイマー計測用
	int charaIndex_;		// アイテムボックスを取得したキャラのCharacterMgr上でのIndex
	float scale_;			// アイテムボックスのスケール
	const float DEF_SCALE = 60.0f;



public:

	/*===== メンバ関数 =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)override;
	void Destroy()override;
	void Update(int Timer, std::weak_ptr<CharacterMgr> Character)override;
	void Disable(int TimerToActivation, int CharaIndex)override;

};