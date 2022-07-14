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
	bool isActive;
	EntryPoint() {};
	EntryPoint(LPCWSTR Entry, bool Flag) :entryPoint_(Entry), isActive(Flag) {};
};
struct HitGroupInitData {

	EntryPoint CH;		// ClosestHitShader
	EntryPoint AH;		// AnyHitShader
	EntryPoint IS;		// IntersectShader
	int SRVcount;		// SRV�̐�
	int CBVcount;		// CBV�̐�
	int UAVcount;		// UAV�̐�
	HitGroupInitData() {};
};

// �q�b�g�O���[�v�N���X
class HitGroup {

private:

	/*===== �����o�ϐ� =====*/

	EntryPoint CH;		// ClosestHitShader
	EntryPoint AH;		// AnyHitShader
	EntryPoint IS;		// IntersectShader
	int SRVcount;		// SRV�̐�
	int CBVcount;		// CBV�̐�
	int UAVcount;		// UAV�̐�

	int registerSpace;	// �V�F�[�_�[���W�X�^�[�̃��W�X�^�[�X�y�[�X�ԍ�

	LPCWSTR hitGroupName;	// ���̃q�b�g�O���[�v�̖��O

	std::shared_ptr<RayRootsignature> localRootSig;	// ���̃q�b�g�O���[�v�Ŏg�p���郍�[�J�����[�g�V�O�l�`��


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	HitGroup();

	// ��������
	void Generate(const HitGroupInitData& InputData, const int& RegisterSpace, const LPCWSTR& HitGroupName);

	// �e��Q�b�^
	const LPCWSTR& GetCH() { return CH.entryPoint_; }
	const LPCWSTR& GetAH() { return AH.entryPoint_; }
	const LPCWSTR& GetIS() { return IS.entryPoint_; }
	const bool& GetCHFlag() { return CH.isActive; }
	const bool& GetAHFlag() { return AH.isActive; }
	const bool& GetISFlag() { return IS.isActive; }
	const int& GetSRVCount() { return SRVcount; }
	const int& GetCBVCount() { return CBVcount; }
	const int& GetUAVCount() { return UAVcount; }
	const std::shared_ptr<RayRootsignature> GetLoacalRootSig() {return localRootSig; };

};