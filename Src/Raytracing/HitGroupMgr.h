#pragma once
#include <memory>
#include <vector>
#include <string>
#include <array>
#include <wtypes.h>
#include "Singleton.h"

class HitGroup;

class HitGroupMgr : public Singleton<HitGroupMgr> {

public:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<HitGroup>> hitGroup;	// �q�b�g�O���[�v


public:

	// �q�b�g�O���[�v����ID
	enum HITGROUP_ID {

		DEF_HIT_GROUP,

	};

	// �q�b�g�O���[�v����ID�A������
	std::vector<LPCWSTR> hitGroupNames;


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O����
	void Setting();


};