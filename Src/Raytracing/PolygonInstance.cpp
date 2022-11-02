#include "PolygonInstance.h"
#include "Engine.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include <assert.h>

D3D12_RAYTRACING_INSTANCE_DESC PolygonMeshInstance::CreateInstance(std::weak_ptr<BLAS> Blas, UINT ShaderID, bool HaveMeshCollisionData, int InstanceIndex)
{

	/*===== �C���X�^���X�𐶐����鏈�� =====*/

	instanceIndex_ = InstanceIndex;

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// �ړ��s����������B
	worldMat_ = DirectX::XMMatrixIdentity();
	matTrans_ = DirectX::XMMatrixIdentity();
	scaleMat_ = DirectX::XMMatrixIdentity();
	matRot_ = DirectX::XMMatrixIdentity();

	// �s���ݒ�B
	XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat_);

	shaderID_ = ShaderID;

	// �C���X�^���X�̏ڍׂ�ݒ�B
	instanceDesc.InstanceID = ShaderID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = Blas.lock()->GetBlasIndex();
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = Blas.lock()->GetBLASBuffer()->GetGPUVirtualAddress();


	// BLAS��Index��ۑ��B
	blasIndex_ = Blas.lock()->GetBlasIndex();
	blas_ = Blas;

	isActive_ = true;
	childCount_ = 0;
	haveMeshCollisionData_ = HaveMeshCollisionData;
	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

	return instanceDesc;

}

D3D12_RAYTRACING_INSTANCE_DESC PolygonMeshInstance::ReCreateInstance(std::weak_ptr<BLAS> Blas, UINT ShaderID, bool HaveMeshCollisionData, int InstanceIndex)
{

	/*===== �C���X�^���X�𐶐����鏈�� =====*/

	instanceIndex_ = InstanceIndex;

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// �s���ݒ�B
	XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat_);

	shaderID_ = ShaderID;

	// �C���X�^���X�̏ڍׂ�ݒ�B
	instanceDesc.InstanceID = ShaderID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = Blas.lock()->GetBlasIndex();
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = Blas.lock()->GetBLASBuffer()->GetGPUVirtualAddress();


	// BLAS��Index��ۑ��B
	blasIndex_ = Blas.lock()->GetBlasIndex();
	blas_ = Blas;

	isActive_ = true;
	childCount_ = 0;
	haveMeshCollisionData_ = HaveMeshCollisionData;
	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

	return instanceDesc;

}

void PolygonMeshInstance::AddTrans(const Vec3& Pos)
{

	/*===== �ړ��֐� =====*/

	matTrans_ *= DirectX::XMMatrixTranslation(Pos.x_, Pos.y_, Pos.z_);
	pos_ = Vec3(matTrans_.r[3].m128_f32[0], matTrans_.r[3].m128_f32[1], matTrans_.r[3].m128_f32[2]);

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::ChangeTrans(const Vec3& Pos)
{

	/*===== �ړ��֐� =====*/

	matTrans_ = DirectX::XMMatrixTranslation(Pos.x_, Pos.y_, Pos.z_);
	pos_ = Vec3(matTrans_.r[3].m128_f32[0], matTrans_.r[3].m128_f32[1], matTrans_.r[3].m128_f32[2]);

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::ChangeTrans(const DirectX::XMMATRIX& Trans)
{

	matTrans_ = Trans;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::AddRotate(const Vec3& Rot)
{

	/*===== ��]�֐� =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff *= DirectX::XMMatrixRotationZ(Rot.z_);
	buff *= DirectX::XMMatrixRotationX(Rot.x_);
	buff *= DirectX::XMMatrixRotationY(Rot.y_);

	matRot_ = buff * matRot_;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::AddRotate(const DirectX::XMMATRIX& Rot)
{

	matRot_ = matRot_ * Rot;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::ChangeRotate(const Vec3& Rot)
{

	/*===== ��]�֐� =====*/

	matRot_ = DirectX::XMMatrixIdentity();

	matRot_ *= DirectX::XMMatrixRotationZ(Rot.z_);
	matRot_ *= DirectX::XMMatrixRotationX(Rot.x_);
	matRot_ *= DirectX::XMMatrixRotationY(Rot.y_);

	rotate_ = Rot;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::ChangeRotate(const DirectX::XMMATRIX& Rot)
{

	/*===== ��]�֐� =====*/

	matRot_ = Rot;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::AddScale(const Vec3& Scale)
{

	/*===== �g�k�֐� =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x_, Scale.y_, Scale.z_);

	scaleMat_ *= buff;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

Vec3 PolygonMeshInstance::GetScaleVec3()
{

	Vec3 defSize = blas_.lock()->GetVertexMax();
	defSize = FHelper::MulMat(defSize, scaleMat_);

	return defSize;
}

void PolygonMeshInstance::ChangeScale(const Vec3& Scale)
{

	/*===== �g�k�֐� =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x_, Scale.y_, Scale.z_);

	scaleMat_ = buff;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::ChangeScale(const DirectX::XMMATRIX& Scale)
{

	scaleMat_ = Scale;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

Vec3 PolygonMeshInstance::GetWorldPos()
{

	/*===== �e�q�֌W���l���������[���h���W�n�ł̍��W���擾 =====*/

	// ���[���h�s����擾�B
	worldMat_ = DirectX::XMMatrixIdentity();

	worldMat_ *= scaleMat_;
	worldMat_ *= matRot_;
	worldMat_ *= matTrans_;

	// �e�s�񂪑��݂��Ă����炾������B
	if (!parentInstance_.expired()) {

		worldMat_ *= parentInstance_.lock()->GetWorldMat();

	}

	// ���W�𔲂����B
	Vec3 pos = { worldMat_.r[3].m128_f32[0], worldMat_.r[3].m128_f32[1],worldMat_.r[3].m128_f32[2] };

	return pos;
}

void PolygonMeshInstance::CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input)
{

	/*===== ���[���h�s����v�Z =====*/

	worldMat_ = DirectX::XMMatrixIdentity();

	worldMat_ *= scaleMat_;
	worldMat_ *= matRot_;
	worldMat_ *= matTrans_;

	// �e�s�񂪑��݂��Ă����炾������B
	if (!parentInstance_.expired()) {

		worldMat_ *= parentInstance_.lock()->GetWorldMat();

	}

	// �ݒ�̍s����X�V����B
	DirectX::XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&Input.Transform),
		worldMat_);

}

DirectX::XMMATRIX PolygonMeshInstance::GetWorldMat()
{

	/*===== ���[���h�s����擾 =====*/

	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	worldMat *= scaleMat_;
	worldMat *= matRot_;
	worldMat *= matTrans_;

	// �e�s�񂪑��݂��Ă����炾������B
	if (!parentInstance_.expired()) {

		worldMat *= parentInstance_.lock()->GetWorldMat();

	}

	return worldMat;

}

void PolygonMeshInstance::SetParentInstance(std::weak_ptr<PolygonMeshInstance> ParentInstance)
{

	/*===== �e�s���ݒ� =====*/

	parentInstance_ = ParentInstance;
	++parentInstance_.lock()->childCount_;

}

int PolygonMeshInstance::GetParentInstanceIndex()
{

	/*===== �e�C���X�^���X��ID���擾 =====*/

	// �e�s�񂪑��݂��Ă����炾������B
	if (!parentInstance_.expired()) {

		return parentInstance_.lock()->GetInstanceIndex();

	}

	// �e�����݂��Ă��Ȃ��B
	assert(0);
	return -1;

}

std::weak_ptr<PolygonMeshInstance> PolygonMeshInstance::GetParetntInstance()
{

	/*===== �e�C���X�^���X���擾 =====*/

	// �e�s�񂪑��݂��Ă����炾������B
	if (!parentInstance_.expired()) {

		return parentInstance_;

	}

	// �e�����݂��Ă��Ȃ��B
	assert(0);
	return std::weak_ptr<PolygonMeshInstance>();

}

void PolygonMeshInstance::Disable()
{

	/*===== �C���X�^���X�𖳌��� =====*/

	// �e�q�֌W�̎q����������B
	if (0 < childCount_) {

		// �e�q�֌W�̐e��������悤�Ƃ��Ă��܂��B�q���̋������Ӑ}���Ȃ����̂ɂȂ�\��������܂��B�q�����ɊJ�����Ă��������B
		assert(0);

	}

	isActive_ = false;
	if (!parentInstance_.expired()) {
		--parentInstance_.lock()->childCount_;
	}

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
	hr = Engine::Ins()->device_.dev_->CreateCommittedResource(
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

void PolygonMeshInstance::CalMeshCollisionData()
{

	/*===== ���b�V���̓����蔻������v�Z =====*/

	// ���b�V���̏����l���擾����B
	std::vector<Vec3> meshDataPos = blas_.lock()->GetVertexPos();
	std::vector<Vec3> meshDataNormal = blas_.lock()->GetVertexNormal();
	std::vector<Vec2> meshDataUV = blas_.lock()->GetVertexUV();
	std::vector<UINT> meshDataIndex = blas_.lock()->GetVertexIndex();

	// �|���S�����ɍ��킹�ă��T�C�Y
	meshCollisionData_.resize(static_cast<unsigned __int64>(static_cast<float>(meshDataPos.size()) / 3.0f));

	// �|���S���̒��g����
	for (auto& index : meshCollisionData_) {

		// ���_1
		index.p1_.pos_ = meshDataPos[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3)])];
		index.p1_.normal_ = meshDataNormal[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3)])];
		index.p1_.uv_ = meshDataUV[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3)])];
		// ���_2
		index.p2_.pos_ = meshDataPos[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 1)])];
		index.p2_.normal_ = meshDataNormal[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 1)])];
		index.p2_.uv_ = meshDataUV[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 1)])];
		// ���_3
		index.p3_.pos_ = meshDataPos[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 2)])];
		index.p3_.normal_ = meshDataNormal[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 2)])];
		index.p3_.uv_ = meshDataUV[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 2)])];
		// �L�����t���O
		index.isActive_ = true;
	}

	/*----- �ۑ������|���S���̒��_���W�Ƀ��[���h�ϊ��s��������� -----*/
	// ���[���h�s��
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	matWorld *= scaleMat_;
	matWorld *= matRot_;
	matWorld *= matTrans_;
	for (auto& index : meshCollisionData_) {
		// ���_��ϊ�
		index.p1_.pos_ = DirectX::XMVector3Transform(index.p1_.pos_.ConvertXMVECTOR(), matWorld);
		index.p2_.pos_ = DirectX::XMVector3Transform(index.p2_.pos_.ConvertXMVECTOR(), matWorld);
		index.p3_.pos_ = DirectX::XMVector3Transform(index.p3_.pos_.ConvertXMVECTOR(), matWorld);
		// �@������]�s�񕪂����ϊ�
		index.p1_.normal_ = DirectX::XMVector3Transform(index.p1_.normal_.ConvertXMVECTOR(), matRot_);
		index.p1_.normal_.Normalize();
		index.p2_.normal_ = DirectX::XMVector3Transform(index.p2_.normal_.ConvertXMVECTOR(), matRot_);
		index.p2_.normal_.Normalize();
		index.p3_.normal_ = DirectX::XMVector3Transform(index.p3_.normal_.ConvertXMVECTOR(), matRot_);
		index.p3_.normal_.Normalize();
	}

}
