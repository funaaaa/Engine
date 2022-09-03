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
	int GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing = false, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	int GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	int GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLAS�̍X�V
	void Update(const int& Index);

	// �e�N�X�`����ύX�B
	void ChangeTex(const int& BlasIndex, const int& TexIndex, const int& TexHandle);

	// �X�L�j���O���v�Z
	void ComputeSkin(const int& Index);

	// �A�j���[�V�����̗L����
	void InitAnimation(const int& Index);	// ������
	void PlayAnimation(const int& Index);	// �Đ�
	void StopAnimation(const int& Index);	// ��~

	// �V�F�[�_�[���R�[�h���������ށB
	uint8_t* WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);

	// �o�b�t�@���擾�B
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer(const int& Index);

	// BLAS�̐����擾�B
	int GetBLASCount() { return static_cast<int>(blas_.size()); }

	// BLAS���擾�B
	std::array<std::shared_ptr<BLAS>, BLAS_COUNT>& GetBLAS() { return blas_; }

	// ���ׂĂ̒��_����Vec3����������B �d�������Ȃ̂ł�������Ă΂Ȃ��B
	void MulVec3Vertex(const int& Index, Vec3 Vec);

	// ���_�̍ő�ŏ��̒l���擾�B
	const Vec3& GetVertexMin(const int& Index);
	const Vec3& GetVertexMax(const int& Index);
	Vec3 GetVertexLengthMax(const int& Index);

	// ���b�V���̏����擾�B
	const std::vector<Vec3>& GetVertex(const int& Index);
	const std::vector<Vec3>& GetNormal(const int& Index);
	const std::vector<Vec2>& GetUV(const int& Index);
	const std::vector<UINT>& GetVertexIndex(const int& Index);

	// �w��̗v�f���폜�B
	void DeleteIndex(const int& Index);

};