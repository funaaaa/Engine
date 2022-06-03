#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

class RayRootsignature;

struct RayPiplineShaderData {

	std::string shaderPath;					// �V�F�[�_�[�p�X
	std::vector<LPCWSTR> rayGenEnteryPoint;	// �G���g���|�C���g
	std::vector<LPCWSTR> missEntryPoint;	// �G���g���|�C���g
	std::vector<LPCWSTR> hitgroupEntryPoint;// �G���g���|�C���g
	RayPiplineShaderData() {};
	RayPiplineShaderData(std::string ShaderPath, std::vector<LPCWSTR> RGEntry, std::vector<LPCWSTR> MSEntry, std::vector<LPCWSTR> HGEntry)
		:shaderPath(ShaderPath), rayGenEnteryPoint(RGEntry), missEntryPoint(MSEntry), hitgroupEntryPoint(HGEntry) {};

};

// ���C�g���[�V���O�Ŏg�p����p�C�v���C��
class RaytracingPipline {

protected:

	/*===== �����o�ϐ� =====*/

	std::vector<RayPiplineShaderData> shaderData;			// �g�p����V�F�[�_�[��Z�߂��\����
	std::vector<D3D12_SHADER_BYTECODE> shaderCode;			// �g�p����V�F�[�_�[�̃o�C�g�R�[�h
	Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject;	// �X�e�[�g�I�u�W�F�N�g
	std::shared_ptr<RayRootsignature> globalRootSig;		// �O���[�o�����[�g�V�O�l�`��
	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc;				// ���C���ˎ��̐ݒ�
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderTable;		// �V�F�[�_�[�e�[�u��
	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps;
	LPCWSTR hitGroupName;


public:

	/*===== �����o�ϐ� =====*/

	// �Z�b�e�B���O����
	void Setting(const std::vector<RayPiplineShaderData>& InputData, const int& UseHitGroup, const int& SRVCount, const int& CBVCount, const int& UAVCount, const int& PayloadSize, const int& AttribSize);

	// �V�F�[�_�[�e�[�u�����\�z
	void ConstructionShaderTable();

	// �Q�b�^
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject; }
	D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return dispatchRayDesc; }
	std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return globalRootSig; }

protected:

	// �q�b�g�O���[�v�̒��ōł��g�p�f�[�^�T�C�Y���傫���̂��擾����B
	UINT GetLargestDataSizeInHitGroup();

	// �A���C�����g
	UINT RoundUp(size_t size, UINT align) {
		return UINT(size + align - 1) & ~(align - 1);
	}

	// �o�b�t�@�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, const wchar_t* name = nullptr);

	// �V�F�[�_�[���ʎq���������ށB
	UINT WriteShaderIdentifier(void* dst, const void* shaderId);

	// RayGeneration�V�F�[�_�[�̐����擾�B
	const int& GetRayGenerationCount();

	// MissShader�̐����擾�B
	const int& GetMissCount();

};