#include "HitGroupMgr.h"
#include "HitGroup.h"

void HitGroupMgr::Setting()
{

	/*===== �q�b�g�O���[�v�ݒ� =====*/

	// �q�b�g�O���[�v���̃R���e�i��ݒ�B
	hitGroupNames.emplace_back(L"DEF_HIT_GROUP");

	// �q�b�g�O���[�v�𐶐��B
	HitGroupInitData initData;

	// ���v�f
	initData.CH = L"mainCHS";
	initData.AH = L"mainAnyHit";
	initData.SRVcount = 3;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ������size()�����̂܂ܓn���Ă���̂́A0�Ԗڂɂ̓O���[�o�����[�g�V�O�l�`��������̂ŃC���N�������g����K�v�����邩��B
	hitGroup[DEF_HIT_GROUP]->Generate(initData, hitGroup.size(), hitGroupNames[DEF_HIT_GROUP]);


}