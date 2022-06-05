#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include "Vec.h"

// TLAS�ɓo�^����C���X�^���X�N���X
class PorygonMeshInstance {

private:

	/*===== �����o�ϐ� =====*/

	UINT instanceID;								// ���̃C���X�^���X��ID

	DirectX::XMMATRIX worldMat;						// ���[���h�s��

	DirectX::XMMATRIX transMat;
	DirectX::XMMATRIX rotMat;
	DirectX::XMMATRIX scaleMat;


public:

	/*===== �����o�֐� =====*/

	// Ins�����֐�
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& HitGroupIndex, const UINT& InstanceID);

	// �ړ�(���������Z)�֐�
	void AddTrans(const Vec3& Pos);

	// �ړ�(�����Ɉړ�)�֐�
	void ChangeTrans(const Vec3& Pos);

	// ��](���W�A���A���������Z)�֐�
	void AddRotate(const Vec3& Rot);

	// ��](���W�A���A��������)�֐�
	void ChangeRotate(const Vec3& Rot);

	// �g��(���������Z)�֐�
	void AddScale(const Vec3& Scale);

	// �g��(��������)�֐�
	void ChangeScale(const Vec3& Scale);

	// ���[���h�s������Z�B
	void CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input);


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};