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
	const LPCWSTR& GetCH(int Index);
	bool GetCHFlag(int Index);
	// AHのゲッタ
	const LPCWSTR& GetAH(int Index);
	bool GetAHFlag(int Index);
	// ISのゲッタ
	const LPCWSTR& GetIS(int Index);
	bool GetISFlag(int Index);
	// 各種要素数のゲッタ
	int GetSRVCount(int Index);
	int GetCBVCount(int Index);
	int GetUAVCount(int Index);
	// ローカルルートシグネチャのゲッタ
	const std::shared_ptr<RayRootsignature> GetLocalRootSig(int Index);
	// ヒットグループ名からヒットグループIDを取得する。
	int GetHitGroupID(const LPCWSTR& HitGroupName);

	// ヒットグループのSRVの数を取得する。
	int GetHitGroupSRVCount(int HitGroupID);
	// ヒットグループのUAVの数を取得する。
	int GetHitGroupUAVCount(int HitGroupID);

};