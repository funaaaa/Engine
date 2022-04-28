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


public:

	/*===== �����o�֐� =====*/

	// Instance�����֐�
	D3D12_RAYTRACING_INSTANCE_DESC CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& HitGroupIndex, const UINT& InstanceID);

	// �ړ�(���������Z)�֐�
	void AddTrans(D3D12_RAYTRACING_INSTANCE_DESC& Input, const Vec3& Pos);

	// ��](���W�A���A���������Z)�֐�
	void AddRotate(D3D12_RAYTRACING_INSTANCE_DESC& Input, const Vec3& Pos);


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};