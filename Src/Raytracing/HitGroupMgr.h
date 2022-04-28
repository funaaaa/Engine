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

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<HitGroup>> hitGroup;	// ヒットグループ


public:

	// ヒットグループ識別ID
	enum HITGROUP_ID {

		DEF_HIT_GROUP,

	};

	// ヒットグループ識別ID、文字列
	std::vector<LPCWSTR> hitGroupNames;


public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting();


};