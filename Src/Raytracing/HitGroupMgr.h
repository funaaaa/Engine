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

	std::vector<std::shared_ptr<HitGroup>> hitGroup_;	// ヒットグループ


public:

	// ヒットグループ識別ID
	enum HITGROUP_ID {

		DEF,
		MAX_HIT_GROUP,

	};

	// ヒットグループ識別ID、文字列
	std::vector<LPCWSTR> hitGroupNames;


public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting();

	// 配列数のゲッタ
	int GetHitGroupCount() { return static_cast<int>(hitGroup_.size()); }
	// CHのゲッタ
	const LPCWSTR& GetCH(const int& Index);
	const bool& GetCHFlag(const int& Index);
	// AHのゲッタ
	const LPCWSTR& GetAH(const int& Index);
	const bool& GetAHFlag(const int& Index);
	// ISのゲッタ
	const LPCWSTR& GetIS(const int& Index);
	const bool& GetISFlag(const int& Index);
	// 各種要素数のゲッタ
	const int& GetSRVCount(const int& Index);
	const int& GetCBVCount(const int& Index);
	const int& GetUAVCount(const int& Index);
	// ローカルルートシグネチャのゲッタ
	const std::shared_ptr<RayRootsignature> GetLocalRootSig(const int& Index);
	// ヒットグループ名からヒットグループIDを取得する。
	int GetHitGroupID(const LPCWSTR& HitGroupName);

	// ヒットグループのSRVの数を取得する。
	int GetHitGroupSRVCount(const int& HitGroupID);
	// ヒットグループのUAVの数を取得する。
	int GetHitGroupUAVCount(const int& HitGroupID);

};