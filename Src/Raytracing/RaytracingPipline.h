#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "WindowsAPI.h"

class RayRootsignature;

struct RayPiplineShaderData {

	std::string shaderPath_;					// �V�F�[�_�[�p�X
	std::vector<LPCWSTR> rayGenEnteryPoint;	// �G���g���|�C���g
	std::vector<LPCWSTR> missEntryPoint;	// �G���g���|�C���g
	std::vector<LPCWSTR> hitgroupEntryPoint;// �G���g���|�C���g
	RayPiplineShaderData() {};
	RayPiplineShaderData(std::string ShaderPath, std::vector<LPCWSTR> RGEntry, std::vector<LPCWSTR> MSEntry, std::vector<LPCWSTR> HGEntry)
		:shaderPath_(ShaderPath), rayGenEnteryPoint(RGEntry), missEntryPoint(MSEntry), hitgroupEntryPoint(HGEntry) {};

};

// ���C�g���[�V���O�Ŏg�p����p�C�v���C��
class RaytracingPipline {

protected:

	/*===== �����o�ϐ� =====*/

	std::vector<RayPiplineShaderData> shaderData_;			// �g�p����V�F�[�_�[��Z�߂��\����
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
	void Setting(const std::vector<RayPiplineShaderData>& InputData, const int& UseHitGroup, const int& SRVCount, const int& CBVCount, const int& UAVCount, const int& PayloadSize, const int& AttribSize, const int& ReflectionCount = 4);

	// �V�F�[�_�[�e�[�u�����\�z
	void ConstructionShaderTable(const int& DispatchX = window_width, const int& DispatchY = window_height);

	// �Q�b�^
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject; }
	D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return dispatchRayDesc; }
	std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return globalRootSig; }

protected:

	// �q�b�g�O���[�v�̒��ōł��g�p�f�[�^�T�C�Y���傫���̂��擾����B
	UINT GetLargestDataSizeInHitGroup();

	// �A���C�����g
	UINT RoundUp(size_t Size, UINT Align) {
		return UINT(Size + Align - 1) & ~(Align - 1);
	}

	// �o�b�t�@�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType, const wchar_t* Name = nullptr);

	// �V�F�[�_�[���ʎq���������ށB
	UINT WriteShaderIdentifier(void* Dst, const void* ShaderId);

	// RayGeneration�V�F�[�_�[�̐����擾�B
	int GetRayGenerationCount();

	// MissShader�̐����擾�B
	int GetMissCount();

};