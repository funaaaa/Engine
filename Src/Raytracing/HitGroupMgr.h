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

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<HitGroup>> hitGroup_;	// �q�b�g�O���[�v


public:

	// �q�b�g�O���[�v����ID
	enum HITGROUP_ID {

		DEF,
		MAX_HIT_GROUP,

	};

	// �q�b�g�O���[�v����ID�A������
	std::vector<LPCWSTR> hitGroupNames;


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O����
	void Setting();

	// �z�񐔂̃Q�b�^
	int GetHitGroupCount() { return static_cast<int>(hitGroup_.size()); }
	// CH�̃Q�b�^
	const LPCWSTR& GetCH(int Index);
	bool GetCHFlag(int Index);
	// AH�̃Q�b�^
	const LPCWSTR& GetAH(int Index);
	bool GetAHFlag(int Index);
	// IS�̃Q�b�^
	const LPCWSTR& GetIS(int Index);
	bool GetISFlag(int Index);
	// �e��v�f���̃Q�b�^
	int GetSRVCount(int Index);
	int GetCBVCount(int Index);
	int GetUAVCount(int Index);
	// ���[�J�����[�g�V�O�l�`���̃Q�b�^
	const std::shared_ptr<RayRootsignature> GetLocalRootSig(int Index);
	// �q�b�g�O���[�v������q�b�g�O���[�vID���擾����B
	int GetHitGroupID(const LPCWSTR& HitGroupName);

	// �q�b�g�O���[�v��SRV�̐����擾����B
	int GetHitGroupSRVCount(int HitGroupID);
	// �q�b�g�O���[�v��UAV�̐����擾����B
	int GetHitGroupUAVCount(int HitGroupID);

};