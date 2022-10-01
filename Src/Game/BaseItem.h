#pragma once
#include "Vec.h"
#include <memory>

class PolygonMeshInstance;

// アイテム基底クラス
class BaseItem {

public:

	/*====== メンバ変数 =====*/

	enum class ItemID {

		NONE,		// 何も持ってない。
		BOOST,		// 加速きのこ
		SHELL,		// 緑甲羅

	};

protected:

	ItemID itemID_;		// アイテム識別用ID
	std::weak_ptr<PolygonMeshInstance> charaInstance;	// キャラのインスタンスのインデックス


public:

	/*===== メンバ変数 =====*/

	virtual void Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance) = 0;
	virtual void Update() = 0;
	virtual void Use(const float& CharaRotY = 0, const int ParamID = 0) = 0;

	ItemID GetItemID() { return itemID_; }
	std::weak_ptr<PolygonMeshInstance> GetCharaInstance() { return charaInstance; }

};