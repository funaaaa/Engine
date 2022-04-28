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
	localRootSig = std::make_unique<RayRootsignature>();

	// SRV�ɂ��Ēǉ��B
	for (int index = 0; index < SRVcount; ++index) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index, registerSpace);

	}
	// CBV�ɂ��Ēǉ��B
	for (int index = 0; index < CBVcount; ++index) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index, registerSpace);

	}
	// UAV�ɂ��Ēǉ��B
	for (int index = 0; index < UAVcount; ++index) {

		localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index, registerSpace);

	}
	// �T���v���[��ǉ��B
	localRootSig->AddStaticSampler(registerSpace);

	// ���[�J�����[�g�V�O�l�`���𐶐��B
	localRootSig->Create(true, hitGroupName);

}
