#pragma once
#include "DirectXBase.h"
#include "RayDescriptor.h"
#include "ComputeShader.h"
#include "FbxLoader.h"
#include "Vec.h"
#include <DirectXMath.h>

using namespace DirectX;

// ���C�g���p���_�\����
struct RayVertex {

	Vec3 position;
	Vec3 normal;
	XMFLOAT2 uv;

};

// �|���S���`���ۑ����Ă���BLAS�N���X
class BLAS {

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

	int modelIndex;							// ���f���̃C���f�b�N�X

	bool isOpaque;							// �s�����t���O

	std::vector<int> textureHandle;			// �g�p����e�N�X�`���̃n���h��

	ComputeShader skinComput;				// �X�L�j���O�s������ɒ��_������������R���s���[�g�V�F�[�_�[

	// �f�o�b�O�p
	vector<RayVertex> vertex;
	vector<UINT> vertIndex;


private:

	std::vector<FbxLoader::SkinComputeInput> skinComputeInput;


public:

	/*===== �����o�֐� =====*/

	// BLAS�̐���
	void GenerateBLASObj(const string& DirectryPath, const string& ModelName, const wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	void GenerateBLASFbx(const string& DirectryPath, const string& ModelName, const wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	void GenerateBLASData(Object3DDeliveryData Data, const wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLAS�̍X�V
	void Update();

	// �X�L�j���O���v�Z
	void ComputeSkin();

	// �A�j���[�V�����̗L����
	void InitAnimation();	// ������
	void PlayAnimation();	// �Đ�
	void StopAnimation();	// ��~

	// �V�F�[�_�[���R�[�h���������ށB
	uint8_t* WriteShaderRecord(uint8_t* Dst, UINT recordSize, ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);

	// �A�N�Z�b�^
	ComPtr<ID3D12Resource>& GetBLASBuffer() { return blasBuffer; }
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
	D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc(const bool& IsOpaque);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc);

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();

	// GPU�f�B�X�N���v�^���������ށB
	inline const UINT& WriteGPUDescriptor(void* Dst, const D3D12_GPU_DESCRIPTOR_HANDLE* Descriptor)
	{
		memcpy(Dst, Descriptor, sizeof(Descriptor));
		return UINT(sizeof(Descriptor));
	}
};