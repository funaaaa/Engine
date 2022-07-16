#pragma once
#include <string>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include <wtypes.h>

class RayRootsignature;

// HitGroup�𐶐�����ۂɓn���f�[�^�\����
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
	int SRVcount_;		// SRV�̐�
	int CBVcount_;		// CBV�̐�
	int UAVcount_;		// UAV�̐�
	HitGroupInitData() {};
};

// �q�b�g�O���[�v�N���X
class HitGroup {

private:

	/*===== �����o�ϐ� =====*/

	EntryPoint CH_;		// ClosestHitShader
	EntryPoint AH_;		// AnyHitShader
	EntryPoint IS_;		// IntersectShader
	int SRVcount_;		// SRV�̐�
	int CBVcount_;		// CBV�̐�
	int UAVcount_;		// UAV�̐�

	int registerSpace_;	// �V�F�[�_�[���W�X�^�[�̃��W�X�^�[�X�y�[�X�ԍ�

	LPCWSTR hitGroupName_;	// ���̃q�b�g�O���[�v�̖��O

	std::shared_ptr<RayRootsignature> localRootSig_;	// ���̃q�b�g�O���[�v�Ŏg�p���郍�[�J�����[�g�V�O�l�`��


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	HitGroup();

	// ��������
	void Generate(const HitGroupInitData& InputData, const int& RegisterSpace, const LPCWSTR& HitGroupName);

	// �e��Q�b�^
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