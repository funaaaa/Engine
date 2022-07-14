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
	bool isActive_;
	EntryPoint() {};
	EntryPoint(LPCWSTR Entry, bool Flag) :entryPoint_(Entry), isActive_(Flag) {};
};
struct HitGroupInitData {

	EntryPoint CH_;		// ClosestHitShader
	EntryPoint AH_;		// AnyHitShader
	EntryPoint IS_;		// IntersectShader
	int SRVcount_;		// SRVの数
	int CBVcount_;		// CBVの数
	int UAVcount_;		// UAVの数
	HitGroupInitData() {};
};

// ヒットグループクラス
class HitGroup {

private:

	/*===== メンバ変数 =====*/

	EntryPoint CH_;		// ClosestHitShader
	EntryPoint AH_;		// AnyHitShader
	EntryPoint IS_;		// IntersectShader
	int SRVcount_;		// SRVの数
	int CBVcount_;		// CBVの数
	int UAVcount_;		// UAVの数

	int registerSpace_;	// シェーダーレジスターのレジスタースペース番号

	LPCWSTR hitGroupName_;	// このヒットグループの名前

	std::shared_ptr<RayRootsignature> localRootSig_;	// このヒットグループで使用するローカルルートシグネチャ


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	HitGroup();

	// 生成処理
	void Generate(const HitGroupInitData& InputData, const int& RegisterSpace, const LPCWSTR& HitGroupName);

	// 各種ゲッタ
	const LPCWSTR& GetCH() { return CH_.entryPoint_; }
	const LPCWSTR& GetAH() { return AH_.entryPoint_; }
	const LPCWSTR& GetIS() { return IS_.entryPoint_; }
	const bool& GetCHFlag() { return CH_.isActive_; }
	const bool& GetAHFlag() { return AH_.isActive_; }
	const bool& GetISFlag() { return IS_.isActive_; }
	const int& GetSRVCount() { return SRVcount_; }
	const int& GetCBVCount() { return CBVcount_; }
	const int& GetUAVCount() { return UAVcount_; }
	const std::shared_ptr<RayRootsignature> GetLoacalRootSig() {return localRootSig_; };

};