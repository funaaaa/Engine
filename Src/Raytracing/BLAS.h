#pragma once
#include "Engine.h"
#include "RayDescriptor.h"
#include "ModelDataManager.h"
#include "FbxLoader.h"
#include "Vec.h"
#include "DynamicConstBuffer.h"
#include <DirectXMath.h>
#include <array>

// ���C�g���p���_�\����
struct RayVertex {

	Vec3 position_;
	Vec3 normal_;
	Vec2 uv_;
	Vec2 subUV_;
	Vec2 pad_;

};

// �|���S���`���ۑ����Ă���BLAS�N���X
class BLAS {

private:

	/*===== �����o�ϐ� =====*/

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> vertexBuffer_;		// ���_�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> vertexUploadBuffer_;	// ���_�o�b�t�@
	std::array<void*, 2> vertexMapAddress_;		// ���_�o�b�t�@Map�p�A�h���X
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> indexBuffer_;			// ���_�C���f�b�N�X�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> indexUploadBuffer_;	// ���_�C���f�b�N�X�o�b�t�@
	std::array<void*, 2> indexMapAddress_;		// ���_�C���f�b�N�X�o�b�t�@Map�p�A�h���X
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> materialBuffer_;			// �}�e���A���o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> materialUploadBuffer_;	// �}�e���A���o�b�t�@
	std::array<void*, 2> materialMapAddress_;	// �}�e���A���o�b�t�@Map�p�A�h���X
	std::array<RayDescriptor, 2> vertexDescriptor_;		// ���_�f�B�X�N���v�^
	std::array<RayDescriptor, 2> indexDescriptor_;			// ���_�C���f�b�N�X�f�B�X�N���v�^
	std::array<RayDescriptor, 2> materialDescriptor_;		// �}�e���A�����p�f�B�X�N���v�^

	// �}�e���A�����p�萔�o�b�t�@
	ModelDataManager::GPUMaterial material_;

	// BLAS�̃C���f�b�N�X�B
	int blasIndex_;

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> blasBuffer_;		// BLAS�p�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> scratchBuffer_;	// �X�N���b�`�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> updateBuffer_;	// �X�V�p�o�b�t�@

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

	// �������ރV�F�[�_�[�e�[�u�����\�Ɨ���2����̂ŁA�������2�p�ӂ���B
	std::array<bool, 2> isChangeTexture_;	// �e�N�X�`�����������������t���O
	std::array<bool, 2> isChangeVertex_;	// ���_���������������t���O

	bool isGenerate_;

	int baseTextureHandle_;				// �g�p����e�N�X�`���̃n���h��
	int mapTextureHandle_;
	std::vector<int> uavHandle_;				// �g�p����UAV�̃n���h��

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

	enum class MAP_PARAM {
		NONE,
		NORMAL,
		SPECULAR,
		AO
	};


public:

	/*===== �����o�֐� =====*/

	BLAS();

	// ����������
	void Init();

	// BLAS�̐���
	void GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque = false);
	void GenerateBLASGLTF(const std::wstring& Path, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque = false);
	void GenerateBLASData(const ModelDataManager::ObjectData& ModelData, int BlasIndex, bool IsOpaque = false);

	// BLAS�̍X�V
	void Update();

	// �A�j���[�V�����̗L����
	void InitAnimation();	// ������
	void PlayAnimation();	// �Đ�
	void StopAnimation();	// ��~

	// �}�e���A���̎Q�Ƃ��擾�B
	ModelDataManager::GPUMaterial& GetMaterial() { return material_; }

	// �}�e���A������������������B
	void IsChangeMaterial();

	// �e�N�X�`����ς����t���O�B
	void ChangeTextureFlag() {
		isChangeTexture_[0] = true;
		isChangeTexture_[1] = true;
	}

	// �e�N�X�`����ǉ��B
	void ChangeBaseTexture(int Index);
	void ChangeMapTexture(int Index, MAP_PARAM MapParam);
	void AddUAVTex(int Index) { uavHandle_.emplace_back(Index); }

	// �V�F�[�_�[���R�[�h���������ށB
	uint8_t* WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName, int Index);

	// �e�����̒����̍ő��Ԃ��B
	Vec3 GetVertexLengthMax();

	// �S�Ă̒��_��Vec3����������B �d�������Ȃ̂œ��I�ɂ͌Ă΂Ȃ��B
	void MulVec3Vertex(Vec3 Vec);

	// �w���UV��SUBUV�ɑ������B
	void AssignUV(const std::vector<RayVertex>& UV);

	// ���_���̍��W�����݂̂�ύX�B
	void ChangeVertexPosition(int Index, const Vec3& Pos);

	// �A�N�Z�b�T
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer(int Index) { return blasBuffer_[Index]; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetVertexBuffer(int Index) { return vertexBuffer_[Index]; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetIndexBuffer(int Index) { return indexBuffer_[Index]; }
	std::wstring& GetHitGroupName() { return hitGroupName_; }
	const std::string& GetModelPath() { return modelPath_; }
	const std::vector<LPCWSTR>& GetTexturePath() { return texturePath_; }
	const Vec3& GetVertexMin() { return vertexMin_; }
	const Vec3& GetVertexMax() { return vertexMax_; }
	bool GetIsGenerate() { return isGenerate_; }
	int GetBlasIndex() { return blasIndex_; }
	int GetBaseTexture() { return baseTextureHandle_; }

	std::vector<RayVertex> GetVertex() { return vertex_; }
	const std::vector<Vec3>& GetVertexPos() { return vertexPos_; }
	const std::vector<Vec3>& GetVertexNormal() { return vertexNormal_; }
	const std::vector<UINT>& GetVertexIndex() { return vertIndex_; }
	const std::vector<Vec2>& GetVertexUV() { return vertexUV_; }

private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(void* MapAddress, const void* PData, size_t DataSize);

	// BLAS�������ɐݒ���擾����֐�
	D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc(bool IsOpaque, int Index);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc, int Index);

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure(int Index);

	// �}�e���A����ݒ�
	void CreateMaterialBuffer();

	// ���_�o�b�t�@��ݒ�
	void CreateVertexBuffer(const ModelDataManager::ObjectData& DataBuff);

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