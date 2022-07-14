#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include "Vec.h"

// TLAS�ɓo�^����C���X�^���X�N���X
class PolygonMeshInstance {

private:

	/*===== �����o�ϐ� =====*/

	UINT instanceID;								// ���̃C���X�^���X��ID

	DirectX::XMMATRIX worldMat;						// ���[���h�s��

	DirectX::XMMATRIX matTrans;
	DirectX::XMMATRIX matRot;
	DirectX::XMMATRIX scaleMat_;

	bool isActive;	// ���̃C���X�^���X���L��������Ă��邩�ǂ����B

	int childCount;	// ���̃C���X�^���X�Ɛe�q�֌W������ł���q���̐��B

	// �eInstance�̃|�C���^
	std::weak_ptr<PolygonMeshInstance> parentInstance;

	Vec3 pos_;
	Vec3 rot;		// �f�o�b�O�p �M�~�b�N��z�u����Ƃ��̒����p�Ɏg�p�������̂ŁA���̒l�����m�ȉ�]�ʂ������Ă���O��Ńf�o�b�O���Ă͂����Ȃ��B (Matrix�𒼐ڑ������ꍇ�̉�]�ʂ͎擾�ł��Ă��Ȃ�����)
	Vec3 scale_;

	UINT blasIndex_;


public:

	/*===== �����o�֐� =====*/

	// Ins�����֐�
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& InstanceID);

	// �ړ�(���������Z)�֐�
	void AddTrans(const Vec3& Pos);
	DirectX::XMMATRIX GetTrans() { return matTrans; }

	// �ړ�(�����Ɉړ�)�֐�
	void ChangeTrans(const Vec3& Pos);
	void ChangeTrans(const DirectX::XMMATRIX& Trans);

	// ��](���W�A���A���������Z)�֐�
	void AddRotate(const Vec3& Rot);
	void AddRotate(const DirectX::XMMATRIX& Rot);
	DirectX::XMMATRIX GetRotate() { return matRot; }

	// ��](���W�A���A��������)�֐�
	void ChangeRotate(const Vec3& Rot);
	void ChangeRotate(const DirectX::XMMATRIX& Rot);

	// �g��(���������Z)�֐�
	void AddScale(const Vec3& Scale);
	DirectX::XMMATRIX GetScale() { return scaleMat_; }

	// �g��(��������)�֐�
	void ChangeScale(const Vec3& Scale);
	void ChangeScale(const DirectX::XMMATRIX& Scale);

	// ���[���h�s������Z�B
	void CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input);

	// ���[���h�s����擾�B
	DirectX::XMMATRIX GetWorldMat();

	// �e�s���ݒ�B
	void SetParentInstance(std::weak_ptr<PolygonMeshInstance> ParentInstance);

	// ���W���擾�B
	inline const Vec3& GetPos() { return pos_; }
	inline const Vec3& GetRotVec3() { return rot; }
	inline const Vec3& GetScaleVec3() { return scale_; }

	// BLAS�C���f�b�N�X���擾�B
	inline const UINT& GetBLASIndex() { return blasIndex_; }

	// instance�𖳌�������B
	void Disable();
	inline const bool GetIsActive() { return isActive; }


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};