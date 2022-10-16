#include "HitGroup.h"
#include "RayRootsignature.h"

HitGroup::HitGroup()
{
}

void HitGroup::Generate(const HitGroupInitData& InputData, int RegisterSpace, const LPCWSTR& HitGroupName)
{

	/*===== 生成処理 =====*/

	// 入力されたデータの値を保存する。
	CH_ = InputData.CH_;
	AH_ = InputData.AH_;
	IS_ = InputData.IS_;
	SRVcount_ = InputData.SRVcount_;
	CBVcount_ = InputData.CBVcount_;
	UAVcount_ = InputData.UAVcount_;
	registerSpace_ = RegisterSpace;
	hitGroupName_ = HitGroupName;

	// ローカルルートシグネチャを生成する。
	localRootSig_ = std::make_shared<RayRootsignature>();

	// SRVについて追加。
	for (int index = 0; index < SRVcount_; ++index) {

		localRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index, registerSpace_);

	}
	// UAVについて追加。
	for (int index = 0; index < UAVcount_; ++index) {

		localRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index, registerSpace_);

	}
	// CBVについて追加。
	for (int index = 0; index < CBVcount_; ++index) {

		localRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index, registerSpace_);

	}
	// サンプラーを追加。
	localRootSig_->AddStaticSampler(registerSpace_);

	// ローカルルートシグネチャを生成。
	localRootSig_->Create(true, hitGroupName_);

}
