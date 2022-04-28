#pragma once
#include <string>
#include <memory>
#include <wtypes.h>

class RayRootsignature;

// HitGroupを生成する際に渡すデータ構造体
struct HitGroupInitData {

	LPCWSTR CH;			// ClosestHitShader
	LPCWSTR AH;			// AnyHitShader
	LPCWSTR IS;			// IntersectShader
	int SRVcount;		// SRVの数
	int CBVcount;		// CBVの数
	int UAVcount;		// UAVの数

};

// ヒットグループクラス
class HitGroup {

public:

	/*===== メンバ変数 =====*/

	LPCWSTR CH;			// ClosestHitShader
	LPCWSTR AH;			// AnyHitShader
	LPCWSTR IS;			// IntersectShader
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

};