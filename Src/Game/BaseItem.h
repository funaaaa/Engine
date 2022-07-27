#pragma once
#include "Vec.h"

// アイテム基底クラス
class BaseItem {

public:

	/*====== メンバ変数 =====*/

	enum class ItemID {

		BOOST,		// 加速きのこ
		SHELL,		// 緑甲羅

	};

protected:

	ItemID itemID_;		// アイテム識別用ID
	int charaInsIndex_;	// キャラのインスタンスのインデックス


public:

	/*===== メンバ変数 =====*/

	virtual void Generate(const int& CharaInsIndex) = 0;
	virtual void Update() = 0;
	virtual void Use(const float& CharaRotY = 0) = 0;

	ItemID GetItemID() { return itemID_; }
	int GetCharaInsIndex() { return charaInsIndex_; }

};