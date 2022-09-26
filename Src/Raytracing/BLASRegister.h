#pragma once
#include <array>
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

	static const int BLAS_COUNT = 256;
	std::array<std::shared_ptr<BLAS>, BLAS_COUNT> blas_;	// �����\����


public:

	/*===== �����o�֐� =====*/

	// BLAS�������B
	void Setting();

	// ��������
	std::weak_ptr<BLAS> GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	std::weak_ptr<BLAS> GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	std::weak_ptr<BLAS> GenerateGLTF(const std::wstring& Path, const std::wstring& HitGroupName, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	
	// �V�F�[�_�[���R�[�h���������ށB
	uint8_t* WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);

	// BLAS�̐����擾�B
	int GetBLASCount() { return static_cast<int>(blas_.size()); }

	// BLAS���擾�B
	std::array<std::shared_ptr<BLAS>, BLAS_COUNT>& GetBLAS() { return blas_; }
	std::weak_ptr<BLAS> GetBlasSpecificationIndex(const int& Index) { return blas_[Index]; }

};