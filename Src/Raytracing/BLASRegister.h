#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Singleton.h"

class BLAS;

class BLASRegister : public Singleton<BLASRegister> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<BLAS>> blas;


public:

	/*===== �����o�֐� =====*/

	// BLAS�̐���
	int GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName);
	int GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName);

	// BLAS�̍X�V
	void Update(const int& Index);

	// �X�L�j���O���v�Z
	void ComputeSkin(const int& Index);

	// �A�j���[�V�����̗L����
	void InitAnimation(const int& Index);	// ������
	void PlayAnimation(const int& Index);	// �Đ�
	void StopAnimation(const int& Index);	// ��~

	// �f�o�b�O�p�@��������
	std::shared_ptr<BLAS> GetBlas(const int& Index) { return blas[Index]; }

};