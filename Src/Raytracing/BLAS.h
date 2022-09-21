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

	Vec3 position_;
	Vec3 normal_;
	Vec2 uv_;

};

// �|���S���`���ۑ����Ă���BLAS�N���X
class BLAS {

private:

	/*===== �����o�ϐ� =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;	// ���_�C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer_;	// ���_�C���f�b�N�X�o�b�t�@
	RayDescriptor vertexDescriptor_;		// ���_�f�B�X�N���v�^
	RayDescriptor indexDescriptor_;			// ���_�C���f�b�N�X�f�B�X�N���v�^
	RayDescriptor materialDescriptor_;		// �}�e���A�����p�f�B�X�N���v�^

	// �}�e���A�����p�萔�o�b�t�@
	ModelDataManager::Material material_;

	// BLAS�̃C���f�b�N�X�B
	int blasIndex_;

	Microsoft::WRL::ComPtr<ID3D12Resource> blasBuffer_;		// BLAS�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;	// �X�N���b�`�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> updateBuffer_;	// �X�V�p�o�b�t�@

	UINT vertexCount_;						// ���_�̐�
	UINT indexCount_;						// ���_�C���f�b�N�X�̐�
	UINT vertexStride_;						// 1���_�̃f�[�^�T�C�Y
	UINT indexStride_;						// 1���_�C���f�b�N�X�̃f�[�^�T�C�Y

	Vec3 vertexMax_;							// ���_�̊e�����̍ő�l
	Vec3 vertexMin_;							// ���_�̊e�����̍ŏ��l

	std::wstring hitGroupName_;				// �g�p����q�b�g�O���[�v�̖��O
	std::string modelPath_;					// �g�p���郂�f���̃p�X
	std::vector<LPCWSTR> texturePath_;		// �g�p����e�N�X�`���̃p�X

	int modelIndex_;							// ���f���̃C���f�b�N�X

	bool isOpaque_;							// �s�����t���O

	bool isChangeTexture;
	bool isChangeVertex;

	bool isGenerate_;

	std::vector<int> textureHandle_;			// �g�p����e�N�X�`���̃n���h��
	std::vector<int> uavHandle_;				// �g�p����UAV�̃n���h��

	ComputeShader skinComput_;				// �X�L�j���O�s������ɒ��_������������R���s���[�g�V�F�[�_�[

	// �f�o�b�O�p
	std::vector<RayVertex> defVertex_;	// �����������_�̒��_
	std::vector<RayVertex> vertex_;		// ���݂̒��_ ���_������������ꍇ������̂ł��̎��p
	std::vector<Vec3> vertexPos_;
	std::vector<Vec3> vertexNormal_;
	std::vector<Vec2> vertexUV_;
	std::vector<UINT> vertIndex_;


private:

	std::vector<FbxLoader::SkinComputeInput> skinComputeInput_;


public:

	/*===== �����o�֐� =====*/

	BLAS();

	// ����������
	void Init();

	// BLAS�̐���
	void GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const int& BlasIndex, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing = false, const bool& IsOpaque = true);
	void GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const int& BlasIndex, std::vector<LPCWSTR> TexturePath);
	void GenerateBLASData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, const int& BlasIndex, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLAS�̍X�V
	void Update();

	// �X�L�j���O���v�Z
	void ComputeSkin();

	// �A�j���[�V�����̗L����
	void InitAnimation();	// ������
	void PlayAnimation();	// �Đ�
	void StopAnimation();	// ��~

	// �e�N�X�`����ǉ��B
	inline void AddTex(const int& Index) { textureHandle_.emplace_back(Index); }
	inline void AddUAVTex(const int& Index) { uavHandle_.emplace_back(Index); }

	// �e�N�X�`����ύX�B
	void ChangeTex(const int& Index, const int& TextureHandle);

	// �V�F�[�_�[���R�[�h���������ށB
	uint8_t* WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);

	// �e�����̒����̍ő��Ԃ��B
	Vec3 GetVertexLengthMax();

	// �S�Ă̒��_��Vec3����������B �d�������Ȃ̂œ��I�ɂ͌Ă΂Ȃ��B
	void MulVec3Vertex(Vec3 Vec);

	// �A�N�Z�b�T
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer() { return blasBuffer_; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetVertexBuffer() { return vertexBuffer_; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetIndexBuffer() { return indexBuffer_; }
	std::wstring& GetHitGroupName() { return hitGroupName_; }
	RayDescriptor& GetVertexDescriptor() { return vertexDescriptor_; }
	RayDescriptor& GetIndexDescriptor() { return indexDescriptor_; }
	const std::string& GetModelPath() { return modelPath_; }
	const std::vector<LPCWSTR>& GetTexturePath() { return texturePath_; }
	const Vec3& GetVertexMin() { return vertexMin_; }
	const Vec3& GetVertexMax() { return vertexMax_; }
	bool GetIsGenerate() { return isGenerate_; }
	int GetBlasIndex() { return blasIndex_; }

	// �f�o�b�O�p
	std::vector<RayVertex> GetVertex() { return vertex_; }
	const std::vector<Vec3>& GetVertexPos() { return vertexPos_; }
	const std::vector<Vec3>& GetVertexNormal() { return vertexNormal_; }
	const std::vector<UINT>& GetVertexIndex() { return vertIndex_; }
	const std::vector<Vec2>& GetVertexUV() { return vertexUV_; }

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