#pragma once
#include <vector>
#include <memory>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "Singleton.h"
#include "ModelDataManager.h"

class BLAS;

class BLASRegister : public Singleton<BLASRegister> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<BLAS>> blas;	// �����\����


public:

	/*===== �����o�֐� =====*/

	// ��������
	int GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	int GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	int GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLAS�̍X�V
	void Update(const int& Index);

	// �X�L�j���O���v�Z
	void ComputeSkin(const int& Index);

	// �A�j���[�V�����̗L����
	void InitAnimation(const int& Index);	// ������
	void PlayAnimation(const int& Index);	// �Đ�
	void StopAnimation(const int& Index);	// ��~

	// �V�F�[�_�[���R�[�h���������ށB
	uint8_t* WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);
	uint8_t* WriteShaderRecordSpecifyUAV(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName, const int& SpecifyIndex);

	// �o�b�t�@���擾�B
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer(const int& Index);

	// BLAS�̐����擾�B
	int GetBLASCount() { return blas.size(); }

	// BLAS���擾�B
	std::vector<std::shared_ptr<BLAS>>& GetBLAS() { return blas; }

};