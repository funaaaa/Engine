#include "HitGroup.h"
#include "RayRootsignature.h"

HitGroup::HitGroup()
{
}

void HitGroup::Generate(const HitGroupInitData& InputData, const int& RegisterSpace, const LPCWSTR& HitGroupName)
{

	/*===== 生成処理 =====*/

	// 入力されたデータの値を保存する。
	CH = InputData.CH;
	AH = InputData.AH;
	IS = InputData.IS;
	SRVcount = InputData.SRVcount;
	CBVcount = InputData.CBVcount;
	UAVcount = InputData.UAVcount;
	registerSpace = RegisterSpace;
	hitGroupName = HitGroupName;

	// ローカルルートシグネチャを生成する。
	localRootSig = std::make_unique<RayRootsignature>();

	// SRVについて追加。
	for (int index = 0; index < SRVcount; ++index) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index, registerSpace);

	}
	// CBVについて追加。
	for (int index = 0; index < CBVcount; ++index) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index, registerSpace);

	}
	// UAVについて追加。
	for (int index = 0; index < UAVcount; ++index) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index, registerSpace);

	}
	// サンプラーを追加。
	localRootSig->AddStaticSampler(registerSpace);

	// ローカルルートシグネチャを生成。
	localRootSig->Create(true, hitGroupName);

}
