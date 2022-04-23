#pragma once
#include "DirectXBase.h"
#include "RayDescriptor.h"
#include <DirectXMath.h>

using namespace DirectX;

// ���C�g���p���_�\����
struct RayVertex {

	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;

};

// �|���S���`���ۑ����Ă���BLAS�N���X
class PorygonMeshBlas {

private:

	/*===== �����o�ϐ� =====*/

	ComPtr<ID3D12Resource> vertexBuffer;	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> indexBuffer;		// ���_�C���f�b�N�X�o�b�t�@
	RayDescriptor vertexDescriptor;			// ���_�f�B�X�N���v�^
	RayDescriptor indexDescriptor;			// ���_�C���f�b�N�X�f�B�X�N���v�^

	ComPtr<ID3D12Resource> blasBuffer;		// BLAS�p�o�b�t�@
	ComPtr<ID3D12Resource> scratchBuffer;	// �X�N���b�`�o�b�t�@
	ComPtr<ID3D12Resource> updateBuffer;	// �X�V�p�o�b�t�@

	UINT vertexCount;						// ���_�̐�
	UINT indexCount;						// ���_�C���f�b�N�X�̐�
	UINT vertexStride;						// 1���_�̃f�[�^�T�C�Y
	UINT indexStride;						// 1���_�C���f�b�N�X�̃f�[�^�T�C�Y

	wstring hitGroupName;					// �g�p����q�b�g�O���[�v�̖��O

	// �f�o�b�O�p
	vector<RayVertex> vertex;
	vector<UINT> vertIndex;


public:

	/*===== �����o�֐� =====*/

	// BLAS�̐���
	void GenerateBLASObj(const string& directryPath, const string& modelName, const wstring& hitGroupName);
	void GenerateBLASFbx(const string& directryPath, const string& modelName, const wstring& hitGroupName);

	// BLAS�̍X�V
	void Update();

	// �A�N�Z�b�^
	ComPtr<ID3D12Resource> GetBLASBuffer() { return blasBuffer; }
	ComPtr<ID3D12Resource>& GetVertexBuffer() { return vertexBuffer; }
	ComPtr<ID3D12Resource>& GetIndexBuffer() { return indexBuffer; }
	wstring& GetHitGroupName() { return hitGroupName; }
	RayDescriptor& GetVertexDescriptor() { return vertexDescriptor; }
	RayDescriptor& GetIndexDescriptor() { return indexDescriptor; }

	// �f�o�b�O�p
	vector<RayVertex>& GetVertex() { return vertex; }

private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// BLAS�������ɐݒ���擾����֐�
	D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc();

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc);

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();
};