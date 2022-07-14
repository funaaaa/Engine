#pragma once
#include "DirectXBase.h"
#include "RayDescriptor.h"
#include "ComputeShader.h"
#include "ModelDataManager.h"
#include "FbxLoader.h"
#include "Vec.h"
#include "DynamicConstBuffer.h"
#include <DirectXMath.h>

// ���C�g���p���_�\����
struct RayVertex {

	Vec3 position;
	Vec3 normal_;
	Vec2 uv_;

};

// �|���S���`���ۑ����Ă���BLAS�N���X
class BLAS {

private:

	/*===== �����o�ϐ� =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;		// ���_�C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer;	// ���_�C���f�b�N�X�o�b�t�@
	RayDescriptor vertexDescriptor;			// ���_�f�B�X�N���v�^
	RayDescriptor indexDescriptor;			// ���_�C���f�b�N�X�f�B�X�N���v�^
	RayDescriptor materialDescriptor;		// �}�e���A�����p�f�B�X�N���v�^

	// �}�e���A�����p�萔�o�b�t�@
	ModelDataManager::Material material_;

	Microsoft::WRL::ComPtr<ID3D12Resource> blasBuffer;		// BLAS�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer;	// �X�N���b�`�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> updateBuffer;	// �X�V�p�o�b�t�@

	UINT vertexCount;						// ���_�̐�
	UINT indexCount;						// ���_�C���f�b�N�X�̐�
	UINT vertexStride;						// 1���_�̃f�[�^�T�C�Y
	UINT indexStride;						// 1���_�C���f�b�N�X�̃f�[�^�T�C�Y

	Vec3 vertexMax_;							// ���_�̊e�����̍ő�l
	Vec3 vertexMin_;							// ���_�̊e�����̍ŏ��l

	std::wstring hitGroupName;				// �g�p����q�b�g�O���[�v�̖��O
	std::string modelPath;					// �g�p���郂�f���̃p�X
	std::vector<LPCWSTR> texturePath;		// �g�p����e�N�X�`���̃p�X

	int modelIndex;							// ���f���̃C���f�b�N�X

	bool isOpaque;							// �s�����t���O

	std::vector<int> textureHandle;			// �g�p����e�N�X�`���̃n���h��

	ComputeShader skinComput;				// �X�L�j���O�s������ɒ��_������������R���s���[�g�V�F�[�_�[

	// �f�o�b�O�p
	std::vector<RayVertex> defVertex;	// �����������_�̒��_
	std::vector<RayVertex> vertex_;		// ���݂̒��_ ���_������������ꍇ������̂ł��̎��p
	std::vector<Vec3> vertexPos;
	std::vector<Vec3> vertexNormal;
	std::vector<UINT> vertIndex;


private:

	std::vector<FbxLoader::SkinComputeInput> skinComputeInput;


public:

	/*===== �����o�֐� =====*/

	// BLAS�̐���
	void GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing = false, const bool& IsOpaque = true);
	void GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	void GenerateBLASData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLAS�̍X�V
	void Update();

	// �X�L�j���O���v�Z
	void ComputeSkin();

	// �A�j���[�V�����̗L����
	void InitAnimation();	// ������
	void PlayAnimation();	// �Đ�
	void StopAnimation();	// ��~

	// �e�N�X�`����ǉ��B
	inline void AddTex(const int& Index) { textureHandle.emplace_back(Index); }

	// �V�F�[�_�[���R�[�h���������ށB
	uint8_t* WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);

	// �e�����̒����̍ő��Ԃ��B
	Vec3 GetVertexLengthMax();

	// �S�Ă̒��_��Vec3����������B �d�������Ȃ̂œ��I�ɂ͌Ă΂Ȃ��B
	void MulVec3Vertex(Vec3 Vec);

	// �A�N�Z�b�^
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer() { return blasBuffer; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetVertexBuffer() { return vertexBuffer; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetIndexBuffer() { return indexBuffer; }
	std::wstring& GetHitGroupName() { return hitGroupName; }
	RayDescriptor& GetVertexDescriptor() { return vertexDescriptor; }
	RayDescriptor& GetIndexDescriptor() { return indexDescriptor; }
	const std::string& GetModelPath() { return modelPath; }
	const std::vector<LPCWSTR>& GetTexturePath() { return texturePath; }
	const Vec3& GetVertexMin() { return vertexMin_; }
	const Vec3& GetVertexMax() { return vertexMax_; }

	// �f�o�b�O�p
	std::vector<RayVertex> GetVertex() { return vertex_; }
	std::vector<Vec3> GetVertexPos() { return vertexPos; }
	std::vector<Vec3> GetVertexNormal() { return vertexNormal; }
	std::vector<UINT> GetVertexIndex() { return vertIndex; }

private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size_, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// BLAS�������ɐݒ���擾����֐�
	D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc(const bool& IsOpaque);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc);

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();

	// GPU�f�B�X�N���v�^���������ށB
	inline UINT WriteGPUDescriptor(void* Dst, const D3D12_GPU_DESCRIPTOR_HANDLE* Descriptor)
	{
		memcpy(Dst, Descriptor, sizeof(Descriptor));
		return static_cast<UINT>((sizeof(Descriptor)));
	}
	inline UINT WriteGPUDescriptor(void* Dst, const D3D12_GPU_VIRTUAL_ADDRESS* Descriptor)
	{
		memcpy(Dst, Descriptor, sizeof(Descriptor));
		return static_cast<UINT>((sizeof(Descriptor)));
	}
};