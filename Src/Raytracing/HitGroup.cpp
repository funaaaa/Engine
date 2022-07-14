#include "HitGroup.h"
#include "RayRootsignature.h"

HitGroup::HitGroup()
{
}

void HitGroup::Generate(const HitGroupInitData& InputData, const int& RegisterSpace, const LPCWSTR& HitGroupName)
{

	/*===== �������� =====*/

	// ���͂��ꂽ�f�[�^�̒l��ۑ�����B
	CH = InputData.CH;
	AH = InputData.AH;
	IS = InputData.IS;
	SRVcount = InputData.SRVcount;
	CBVcount = InputData.CBVcount;
	UAVcount = InputData.UAVcount;
	registerSpace = RegisterSpace;
	hitGroupName = HitGroupName;

	// ���[�J�����[�g�V�O�l�`���𐶐�����B
	localRootSig = std::make_shared<RayRootsignature>();

	// SRV�ɂ��Ēǉ��B
	for (int index_ = 0; index_ < SRVcount; ++index_) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index_, registerSpace);

	}
	// UAV�ɂ��Ēǉ��B
	for (int index_ = 0; index_ < UAVcount; ++index_) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index_, registerSpace);

	}
	// CBV�ɂ��Ēǉ��B
	for (int index_ = 0; index_ < CBVcount; ++index_) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index_, registerSpace);

	}
	// �T���v���[��ǉ��B
	localRootSig->AddStaticSampler(registerSpace);

	// ���[�J�����[�g�V�O�l�`���𐶐��B
	localRootSig->Create(true, hitGroupName);

}
