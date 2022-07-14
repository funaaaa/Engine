#include "PolygonInstance.h"
#include "DirectXBase.h"
#include <assert.h>

D3D12_RAYTRACING_INSTANCE_DESC PolygonMeshInstance::CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& InstanceID)
{

	/*===== �C���X�^���X�𐶐����鏈�� =====*/

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// �ړ��s����������B
	worldMat = DirectX::XMMatrixIdentity();
	matTrans = DirectX::XMMatrixIdentity();
	scaleMat = DirectX::XMMatrixIdentity();
	matRot = DirectX::XMMatrixIdentity();

	// �s���ݒ�B
	XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat);

	scale_ = Vec3(1, 1, 1);

	// �C���X�^���X�̏ڍׂ�ݒ�B
	instanceDesc.InstanceID = InstanceID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = BlasIndex;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = BlassBuffer->GetGPUVirtualAddress();

	// BLAS��Index��ۑ��B
	blasIndex_ = BlasIndex;

	isActive = true;
	childCount = 0;

	return instanceDesc;

}

void PolygonMeshInstance::AddTrans(const Vec3& Pos)
{

	/*===== �ړ��֐� =====*/

	matTrans *= DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	pos_ = Vec3(matTrans.r[3].m128_f32[0], matTrans.r[3].m128_f32[1], matTrans.r[3].m128_f32[2]);

}

void PolygonMeshInstance::ChangeTrans(const Vec3& Pos)
{

	/*===== �ړ��֐� =====*/

	matTrans = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	pos_ = Vec3(matTrans.r[3].m128_f32[0], matTrans.r[3].m128_f32[1], matTrans.r[3].m128_f32[2]);

}

void PolygonMeshInstance::ChangeTrans(const DirectX::XMMATRIX& Trans)
{

	matTrans = Trans;

}

void PolygonMeshInstance::AddRotate(const Vec3& Rot)
{

	/*===== ��]�֐� =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff *= DirectX::XMMatrixRotationZ(Rot.z);
	buff *= DirectX::XMMatrixRotationX(Rot.x);
	buff *= DirectX::XMMatrixRotationY(Rot.y);

	matRot = buff * matRot;

	// �f�o�b�O�p�̉�]��ۑ��B
	rot += Rot;

}

void PolygonMeshInstance::AddRotate(const DirectX::XMMATRIX& Rot)
{

	matRot = matRot * Rot;

}

void PolygonMeshInstance::ChangeRotate(const Vec3& Rot)
{

	/*===== ��]�֐� =====*/

	matRot = DirectX::XMMatrixIdentity();

	matRot *= DirectX::XMMatrixRotationZ(Rot.z);
	matRot *= DirectX::XMMatrixRotationX(Rot.x);
	matRot *= DirectX::XMMatrixRotationY(Rot.y);

	// �f�o�b�O�p�̉�]��ۑ��B
	rot = Rot;

}

void PolygonMeshInstance::ChangeRotate(const DirectX::XMMATRIX& Rot)
{

	/*===== ��]�֐� =====*/

	matRot = Rot;

}

void PolygonMeshInstance::AddScale(const Vec3& Scale)
{

	/*===== �g�k�֐� =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	scaleMat *= buff;

	scale_ += Scale;

}

void PolygonMeshInstance::ChangeScale(const Vec3& Scale)
{

	/*===== �g�k�֐� =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	scaleMat = buff;

	scale_ = Scale;

}

void PolygonMeshInstance::ChangeScale(const DirectX::XMMATRIX& Scale)
{

	scaleMat = Scale;

}

void PolygonMeshInstance::CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input)
{

	/*===== ���[���h�s����v�Z =====*/

	worldMat = DirectX::XMMatrixIdentity();

	worldMat *= scaleMat;
	worldMat *= matRot;
	worldMat *= matTrans;

	// �e�s�񂪑��݂��Ă����炾������B
	if (!parentInstance.expired()) {

		worldMat *= parentInstance.lock()->GetWorldMat();

	}

	// �ݒ�̍s����X�V����B
	DirectX::XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&Input.Transform),
		worldMat);

}

DirectX::XMMATRIX PolygonMeshInstance::GetWorldMat()
{

	/*===== ���[���h�s����擾 =====*/

	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	worldMat *= scaleMat;
	worldMat *= matRot;
	worldMat *= matTrans;

	// �e�s�񂪑��݂��Ă����炾������B
	if (!parentInstance.expired()) {

		worldMat *= parentInstance.lock()->GetWorldMat();

	}

	return worldMat;

}

void PolygonMeshInstance::SetParentInstance(std::weak_ptr<PolygonMeshInstance> ParentInstance)
{

	/*===== �e�s���ݒ� =====*/

	parentInstance = ParentInstance;
	++parentInstance.lock()->childCount;

}

void PolygonMeshInstance::Disable()
{

	/*===== �C���X�^���X�𖳌��� =====*/

	// �e�q�֌W�̎q����������B
	if (0 < childCount) {

		// �e�q�֌W�̐e��������悤�Ƃ��Ă��܂��B�q���̋������Ӑ}���Ȃ����̂ɂȂ�\��������܂��B�q�����ɊJ�����Ă��������B
		assert(0);

	}

	isActive = false;
	--parentInstance.lock()->childCount;

}

void PolygonMeshInstance::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize)
{

	/*===== �������ɒl���������ޏ��� =====*/

	// null�`�F�b�N�B
	if (Resource == nullptr) return;

	// �}�b�v�������s���B
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, DataSize };
	HRESULT hr = Resource->Map(0, nullptr, (void**)&mapped);

	// �}�b�v������������l���������ށB
	if (SUCCEEDED(hr)) {

		memcpy(mapped, PData, DataSize);
		Resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> PolygonMeshInstance::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== �o�b�t�@�S�ʂ𐶐����鏈�� =====*/

	// ��������ݒ�p�\���̂�ݒ肷��B
	D3D12_HEAP_PROPERTIES heapProps{};
	if (HeapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}

	// ���ۂɃo�b�t�@�𐶐�����B
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = Size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = Flags;

	// �o�b�t�@�������߂��o���B
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		InitialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// �����Ɏ��s������B
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}
