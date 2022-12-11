#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include "WindowsAPI.h"

class RayRootsignature;

struct RayPipelineShaderData {

	std::string shaderPath_;					// �V�F�[�_�[�p�X
	std::vector<LPCWSTR> rayGenEnteryPoint_;	// �G���g���|�C���g
	std::vector<LPCWSTR> missEntryPoint_;		// �G���g���|�C���g
	std::vector<LPCWSTR> hitgroupEntryPoint_;	// �G���g���|�C���g
	RayPipelineShaderData() {};
	RayPipelineShaderData(std::string ShaderPath, std::vector<LPCWSTR> RGEntry, std::vector<LPCWSTR> MSEntry, std::vector<LPCWSTR> HGEntry)
		:shaderPath_(ShaderPath), rayGenEnteryPoint_(RGEntry), missEntryPoint_(MSEntry), hitgroupEntryPoint_(HGEntry) {};

};

// ���C�g���[�V���O�Ŏg�p����p�C�v���C��
class RaytracingPipeline {

protected:

	/*===== �����o�ϐ� =====*/

	std::vector<RayPipelineShaderData> shaderData_;			// �g�p����V�F�[�_�[��Z�߂��\����
	std::vector<D3D12_SHADER_BYTECODE> shaderCode_;			// �g�p����V�F�[�_�[�̃o�C�g�R�[�h
	Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject_;	// �X�e�[�g�I�u�W�F�N�g
	std::shared_ptr<RayRootsignature> globalRootSig_;		// �O���[�o�����[�g�V�O�l�`��
	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc_;				// ���C���ˎ��̐ݒ�
	Microsoft::WRL::ComPtr<ID3D12Resource> shaderTable_;		// �V�F�[�_�[�e�[�u��
	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps_;
	LPCWSTR hitGroupName_;

	UINT raygenRegion_;
	UINT missRegion_;
	UINT hitgroupRecordSize_;


public:

	/*===== �����o�ϐ� =====*/

	// �Z�b�e�B���O����
	void Setting(const std::vector<RayPipelineShaderData>& InputData, int UseHitGroup, int SRVCount, int CBVCount, int UAVCount, int PayloadSize, int AttribSize, int ReflectionCount = 4);

	// �V�F�[�_�[�e�[�u�����\�z
	void ConstructionShaderTable(int DispatchX = WINDOW_WIDTH, int DispatchY = WINDOW_HEIGHT);

	// HitGroup�̏���]���B
	void MapHitGroupInfo();

	// �Q�b�^
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject_; }
	D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return dispatchRayDesc_; }
	std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return globalRootSig_; }

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