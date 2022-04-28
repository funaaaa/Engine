#pragma once
#include <string>
#include <memory>
#include <wtypes.h>

class RayRootsignature;

// HitGroup�𐶐�����ۂɓn���f�[�^�\����
struct HitGroupInitData {

	LPCWSTR CH;			// ClosestHitShader
	LPCWSTR AH;			// AnyHitShader
	LPCWSTR IS;			// IntersectShader
	int SRVcount;		// SRV�̐�
	int CBVcount;		// CBV�̐�
	int UAVcount;		// UAV�̐�

};

// �q�b�g�O���[�v�N���X
class HitGroup {

public:

	/*===== �����o�ϐ� =====*/

	LPCWSTR CH;			// ClosestHitShader
	LPCWSTR AH;			// AnyHitShader
	LPCWSTR IS;			// IntersectShader
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

};