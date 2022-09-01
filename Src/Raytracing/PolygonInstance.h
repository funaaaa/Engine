#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "Vec.h"
#include "FHelper.h"

// TLAS�ɓo�^����C���X�^���X�N���X
class PolygonMeshInstance {

private:

	/*===== �����o�ϐ� =====*/

	UINT instanceIndex_;								// ���̃C���X�^���X��ID

	UINT shaderID_;

	DirectX::XMMATRIX worldMat_;						// ���[���h�s��

	DirectX::XMMATRIX matTrans_;
	DirectX::XMMATRIX matRot_;
	DirectX::XMMATRIX scaleMat_;

	bool isActive_;	// ���̃C���X�^���X���L��������Ă��邩�ǂ����B

	int childCount_;	// ���̃C���X�^���X�Ɛe�q�֌W������ł���q���̐��B

	// �eInstance�̃|�C���^
	std::weak_ptr<PolygonMeshInstance> parentInstance_;

	Vec3 pos_;
	Vec3 rotate_;		// �����I�u�W�F�N�g�z�u�p

	UINT blasIndex_;

	bool haveMeshCollisionData_;
	std::vector<FHelper::CheckHitPorygon> meshCollisionData_;


public:

	/*===== �����o�֐� =====*/

	// Ins�����֐�
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& ShaderID, const bool& HaveMeshCollisionData, const int& InstanceIndex);

	// �ړ�(���������Z)�֐�
	void AddTrans(const Vec3& Pos);
	DirectX::XMMATRIX GetTrans() { return matTrans_; }

	// �ړ�(�����Ɉړ�)�֐�
	void ChangeTrans(const Vec3& Pos);
	void ChangeTrans(const DirectX::XMMATRIX& Trans);

	// ��](���W�A���A���������Z)�֐�
	void AddRotate(const Vec3& Rot);
	void AddRotate(const DirectX::XMMATRIX& Rot);
	DirectX::XMMATRIX GetRotate() { return matRot_; }

	// ��](���W�A���A��������)�֐�
	void ChangeRotate(const Vec3& Rot);
	void ChangeRotate(const DirectX::XMMATRIX& Rot);

	// �g��(���������Z)�֐�
	void AddScale(const Vec3& Scale);
	DirectX::XMMATRIX GetScale() { return scaleMat_; }

	// �g��(��������)�֐�
	void ChangeScale(const Vec3& Scale);
	void ChangeScale(const DirectX::XMMATRIX& Scale);

	// �e�q�֌W���l���������[���h���W�n�ł̍��W���擾�B
	Vec3 GetWorldPos();

	// ���[���h�s������Z�B
	void CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input);

	// ���[���h�s����擾�B
	DirectX::XMMATRIX GetWorldMat();

	// �e�s���ݒ�B
	void SetParentInstance(std::weak_ptr<PolygonMeshInstance> ParentInstance);

	// ���W���擾�B
	inline const Vec3& GetPos() { return pos_; }
	inline const Vec3& GetRotateVec3() { return rotate_; }
	inline const UINT GetShaderID() { return shaderID_; }
	inline const int& GetInstanceIndex() { return instanceIndex_; }
	const int& GetParentInstanceIndex();

	// BLAS�C���f�b�N�X���擾�B
	inline const UINT& GetBLASIndex() { return blasIndex_; }

	// instance�𖳌�������B
	void Disable();
	inline const bool GetIsActive() { return isActive_; }

	// ���b�V���̓����蔻��̃f�[�^��Ԃ��B
	const std::vector<FHelper::CheckHitPorygon>& GetMeshCollisionData() { return meshCollisionData_; }
	const bool& GetHaveMeshCollisionData() { return haveMeshCollisionData_; }


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

	// ���b�V���̓����蔻������v�Z����B
	void CalMeshCollisionData();

};