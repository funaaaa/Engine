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
	const LPCWSTR& GetCH(const int& Index);
	const bool& GetCHFlag(const int& Index);
	// AH�̃Q�b�^
	const LPCWSTR& GetAH(const int& Index);
	const bool& GetAHFlag(const int& Index);
	// IS�̃Q�b�^
	const LPCWSTR& GetIS(const int& Index);
	const bool& GetISFlag(const int& Index);
	// �e��v�f���̃Q�b�^
	const int& GetSRVCount(const int& Index);
	const int& GetCBVCount(const int& Index);
	const int& GetUAVCount(const int& Index);
	// ���[�J�����[�g�V�O�l�`���̃Q�b�^
	const std::shared_ptr<RayRootsignature> GetLocalRootSig(const int& Index);
	// �q�b�g�O���[�v������q�b�g�O���[�vID���擾����B
	int GetHitGroupID(const LPCWSTR& HitGroupName);

	// �q�b�g�O���[�v��SRV�̐����擾����B
	int GetHitGroupSRVCount(const int& HitGroupID);
	// �q�b�g�O���[�v��UAV�̐����擾����B
	int GetHitGroupUAVCount(const int& HitGroupID);

};