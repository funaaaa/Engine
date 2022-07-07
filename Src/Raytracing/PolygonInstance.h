#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "Vec.h"

// TLAS�ɓo�^����C���X�^���X�N���X
class PolygonMeshInstance {

private:

	/*===== �����o�ϐ� =====*/

	UINT instanceID;								// ���̃C���X�^���X��ID

	DirectX::XMMATRIX worldMat;						// ���[���h�s��

	DirectX::XMMATRIX matTrans;
	DirectX::XMMATRIX matRot;
	DirectX::XMMATRIX scaleMat;

	Vec3 pos;

	UINT blasIndex;


public:

	/*===== �����o�֐� =====*/

	// Ins�����֐�
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& InstanceID);

	// �ړ�(���������Z)�֐�
	void AddTrans(const Vec3& Pos);
	DirectX::XMMATRIX GetTrans() { return matTrans; }

	// �ړ�(�����Ɉړ�)�֐�
	void ChangeTrans(const Vec3& Pos);

	// ��](���W�A���A���������Z)�֐�
	void AddRotate(const Vec3& Rot);
	void AddRotate(const DirectX::XMMATRIX& Rot);
	DirectX::XMMATRIX GetRotate() { return matRot; }

	// ��](���W�A���A��������)�֐�
	void ChangeRotate(const Vec3& Rot);
	void ChangeRotate(const DirectX::XMMATRIX& Rot);

	// �g��(���������Z)�֐�
	void AddScale(const Vec3& Scale);
	DirectX::XMMATRIX GetScale() { return scaleMat; }

	// �g��(��������)�֐�
	void ChangeScale(const Vec3& Scale);

	// ���[���h�s������Z�B
	void CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input);

	// ���W���擾�B
	inline const Vec3& GetPos() { return pos; }
	// BLAS�C���f�b�N�X���擾�B
	inline const UINT& GetBLASIndex() { return blasIndex; }


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};