#pragma once
#include <string>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include <wtypes.h>

class RayRootsignature;

// HitGroupを生成する際に渡すデータ構造体
struct EntryPoint {
	LPCWSTR entryPoint_;
	bool isActive;
	EntryPoint() {};
	EntryPoint(LPCWSTR Entry, bool Flag) :entryPoint_(Entry), isActive(Flag) {};
};
struct HitGroupInitData {

	EntryPoint CH;		// ClosestHitShader
	EntryPoint AH;		// AnyHitShader
	EntryPoint IS;		// IntersectShader
	int SRVcount;		// SRVの数
	int CBVcount;		// CBVの数
	int UAVcount;		// UAVの数
	HitGroupInitData() {};
};

// ヒットグループクラス
class HitGroup {

private:

	/*===== メンバ変数 =====*/

	EntryPoint CH;		// ClosestHitShader
	EntryPoint AH;		// AnyHitShader
	EntryPoint IS;		// IntersectShader
	int SRVcount;		// SRVの数
	int CBVcount;		// CBVの数
	int UAVcount;		// UAVの数

	int registerSpace;	// シェーダーレジスターのレジスタースペース番号

	LPCWSTR hitGroupName;	// このヒットグループの名前

	std::shared_ptr<RayRootsignature> localRootSig;	// このヒットグループで使用するローカルルートシグネチャ


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	HitGroup();

	// 生成処理
	void Generate(const HitGroupInitData& InputData, const int& RegisterSpace, const LPCWSTR& HitGroupName);

	// 各種ゲッタ
	const LPCWSTR& GetCH() { return CH.entryPoint_; }
	const LPCWSTR& GetAH() { return AH.entryPoint_; }
	const LPCWSTR& GetIS() { return IS.entryPoint_; }
	const bool& GetCHFlag() { return CH.isActive; }
	const bool& GetAHFlag() { return AH.isActive; }
	const bool& GetISFlag() { return IS.isActive; }
	const int& GetSRVCount() { return SRVcount; }
	const int& GetCBVCount() { return CBVcount; }
	const int& GetUAVCount() { return UAVcount; }
	const std::shared_ptr<RayRootsignature> GetLoacalRootSig() {return localRootSig; };

};