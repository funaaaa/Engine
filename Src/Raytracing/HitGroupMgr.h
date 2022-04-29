#pragma once
#include <memory>
#include <vector>
#include <string>
#include <array>
#include <wtypes.h>
#include "Singleton.h"

class HitGroup;
class RayRootsignature;

class HitGroupMgr : public Singleton<HitGroupMgr> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<HitGroup>> hitGroup;	// ヒットグループ


public:

	// ヒットグループ識別ID
	enum HITGROUP_ID {

		DEF_HIT_GROUP,
		MAX_HIT_GROUP,

	};

	// ヒットグループ識別ID、文字列
	std::vector<LPCWSTR> hitGroupNames;


public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting();

	// 配列数のゲッタ
	const int& GetHitGroupCount() { return hitGroup.size(); }
	// CHのゲッタ
	const LPCWSTR& GetCH(const int& Index);
	const bool& GetCHFlag(const int& Index);
	// AHのゲッタ
	const LPCWSTR& GetAH(const int& Index);
	const bool& GetAHFlag(const int& Index);
	// ISのゲッタ
	const LPCWSTR& GetIS(const int& Index);
	const bool& GetISFlag(const int& Index);
	// ローカルルートシグネチャのゲッタ
	const std::shared_ptr<RayRootsignature> GetLocalRootSig(const int& Index);

};