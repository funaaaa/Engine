#include "PorygonInstance.h"
#include "PorygonInstanceRegister.h"

void PorygonMeshInstance::CreateInstance(const ComPtr<ID3D12Resource>& blassBuffer, const UINT& hitGroupIndex, const UINT& instanceID)
{

	/*===== �C���X�^���X�𐶐����鏈�� =====*/

	// �ړ��s����������B
	worldMat = XMMatrixIdentity();

	// �C���X�^���XID�����߂�B
	//instanceID = InstanceIDMgr::Instance()->GetInstanceID();

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};

	// �s���ݒ�B
	XMStoreFloat3x4(
		reinterpret_cast<XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat);

	// �C���X�^���X�̏ڍׂ�ݒ�B
	instanceDesc.InstanceID = instanceID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = hitGroupIndex;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = blassBuffer->GetGPUVirtualAddress();

	// ���W�X�^�[�Ɋi�[����B
	registerID = PorygonInstanceRegister::Instance()->SetRegister(instanceDesc);

}

void PorygonMeshInstance::AddTrans(const float& x, const float& y, const float z)
{

	/*===== �ړ��֐� =====*/

	// �ړ��s����X�V�B
	worldMat *= XMMatrixTranslation(x, y, z);

	// ���W�X�^�[�̐ݒ���X�V����B
	PorygonInstanceRegister::Instance()->AddTrans(worldMat, registerID);

}

void PorygonMeshInstance::AddTrans(const XMFLOAT3& pos)
{

	/*===== �ړ��֐� =====*/

	AddTrans(pos.x, pos.y, pos.z);

}

void PorygonMeshInstance::AddRotate(const float& x, const float& y, const float z)
{

	/*===== �ړ��֐� =====*/

	// �ړ��s����X�V�B
	worldMat *= XMMatrixRotationZ(z);
	worldMat *= XMMatrixRotationX(x);
	worldMat *= XMMatrixRotationY(y);

	// ���W�X�^�[�̐ݒ���X�V����B
	PorygonInstanceRegister::Instance()->AddTrans(worldMat, registerID);

}

void PorygonMeshInstance::AddRotate(const XMFLOAT3& pos)
{

	/*===== �ړ��֐� =====*/

	AddRotate(pos.x, pos.y, pos.z);

}

void PorygonMeshInstance::WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize)
{

	/*===== �������ɒl���������ޏ��� =====*/

	// null�`�F�b�N�B
	if (resource == nullptr) return;

	// �}�b�v�������s���B
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, dataSize };
	HRESULT hr = resource->Map(0, nullptr, (void**)&mapped);

	// �}�b�v������������l���������ށB
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, dataSize);
		resource->Unmap(0, nullptr);

	}

}

ComPtr<ID3D12Resource> PorygonMeshInstance::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
{

	/*===== �o�b�t�@�S�ʂ𐶐����鏈�� =====*/

	// ��������ݒ�p�\���̂�ݒ肷��B
	D3D12_HEAP_PROPERTIES heapProps{};
	if (heapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}

	// ���ۂɃo�b�t�@�𐶐�����B
	HRESULT hr;
	ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = flags;

	// �o�b�t�@�������߂��o���B
	hr = DirectXBase::Instance()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// �����Ɏ��s������B
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}
