#pragma once
#include "Vec.h"
#include <memory>

class PolygonMeshInstance;

// �A�C�e�����N���X
class BaseItem {

public:

	/*====== �����o�ϐ� =====*/

	enum class ItemID {

		NONE,		// ���������ĂȂ��B
		BOOST,		// �������̂�
		SHELL,		// �΍b��

	};

protected:

	ItemID itemID_;		// �A�C�e�����ʗpID
	std::weak_ptr<PolygonMeshInstance> charaInstance;	// �L�����̃C���X�^���X�̃C���f�b�N�X


public:

	/*===== �����o�ϐ� =====*/

	virtual void Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance) = 0;
	virtual void Update() = 0;
	virtual void Use(const float& CharaRotY = 0, const int ParamID = 0) = 0;

	ItemID GetItemID() { return itemID_; }
	std::weak_ptr<PolygonMeshInstance> GetCharaInstance() { return charaInstance; }

};