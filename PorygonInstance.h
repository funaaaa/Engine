#pragma once
#include "DirectXBase.h"
#include <DirectXMath.h>

using namespace DirectX;

// TLAS�ɓo�^����C���X�^���X�N���X
class PorygonMeshInstance {

private:

	/*===== �����o�ϐ� =====*/

	UINT instanceID;								// ���̃C���X�^���X��ID
	UINT registerID;								// register�̉��ԖڂɊi�[����Ă��邩��ID �f�o�b�O�p

	XMMATRIX transMat;								// �ړ��s��


public:

	/*===== �����o�֐� =====*/

	// Instance�����֐�
	void CreateInstance(const ComPtr<ID3D12Resource>& blassBuffer, const UINT& hitGroupIndex, const UINT& instanceID);

	// �ړ�(���������Z)�֐�
	void AddTrans(const float& x, const float& y, const float z);
	void AddTrans(const XMFLOAT3& pos);


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

};