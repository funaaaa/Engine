#pragma once
#include "Vec.h"

// �A�C�e�����N���X
class BaseItem {

public:

	/*====== �����o�ϐ� =====*/

	enum class ItemID {

		BOOST,		// �������̂�
		SHELL,		// �΍b��

	};

protected:

	ItemID itemID_;		// �A�C�e�����ʗpID
	int charaInsIndex_;	// �L�����̃C���X�^���X�̃C���f�b�N�X


public:

	/*===== �����o�ϐ� =====*/

	virtual void Generate(const int& CharaInsIndex) = 0;
	virtual void Update() = 0;
	virtual void Use(const float& CharaRotY = 0) = 0;

	ItemID GetItemID() { return itemID_; }
	int GetCharaInsIndex() { return charaInsIndex_; }

};