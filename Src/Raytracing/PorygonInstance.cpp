#include "PorygonInstance.h"
#include "DirectXBase.h"

D3D12_RAYTRACING_INSTANCE_DESC PorygonMeshInstance::CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& blassBuffer, const UINT& hitGroupIndex, const UINT& instanceID)
{

	/*===== インスタンスを生成する処理 =====*/

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// 移動行列を初期化。
	worldMat = DirectX::XMMatrixIdentity();
	transMat = DirectX::XMMatrixIdentity();
	scaleMat = DirectX::XMMatrixIdentity();
	rotMat = DirectX::XMMatrixIdentity();

	// 行列を設定。
	XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat);

	// インスタンスの詳細を設定。
	instanceDesc.InstanceID = instanceID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = hitGroupIndex;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = blassBuffer->GetGPUVirtualAddress();

	return instanceDesc;

}

void PorygonMeshInstance::AddTrans(const Vec3& pos)
{

	/*===== 移動関数 =====*/

	transMat *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

}

void PorygonMeshInstance::ChangeTrans(const Vec3& Pos)
{

	/*===== 移動関数 =====*/

	transMat = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);

}

void PorygonMeshInstance::AddRotate(const Vec3& Rot)
{

	/*===== 回転関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff *= DirectX::XMMatrixRotationZ(Rot.z);
	buff *= DirectX::XMMatrixRotationX(Rot.x);
	buff *= DirectX::XMMatrixRotationY(Rot.y);

	rotMat = buff * rotMat;

}

void PorygonMeshInstance::ChangeRotate(const Vec3& Rot)
{

	/*===== 回転関数 =====*/

	rotMat = DirectX::XMMatrixIdentity();

	rotMat *= DirectX::XMMatrixRotationZ(Rot.z);
	rotMat *= DirectX::XMMatrixRotationX(Rot.x);
	rotMat *= DirectX::XMMatrixRotationY(Rot.y);

}

void PorygonMeshInstance::AddScale(const Vec3& Scale)
{

	/*===== 拡縮関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	scaleMat *= buff;

}

void PorygonMeshInstance::ChangeScale(const Vec3& Scale)
{

	/*===== 拡縮関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	scaleMat = buff;

}

void PorygonMeshInstance::CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input)
{

	/*===== ワールド行列を計算 =====*/

	worldMat = DirectX::XMMatrixIdentity();

	worldMat *= scaleMat;
	worldMat *= rotMat;
	worldMat *= transMat;

	// 設定の行列を更新する。
	DirectX::XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&Input.Transform),
		worldMat);

}

void PorygonMeshInstance::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize)
{

	/*===== メモリに値を書き込む処理 =====*/

	// nullチェック。
	if (resource == nullptr) return;

	// マップ処理を行う。
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, dataSize };
	HRESULT hr = resource->Map(0, nullptr, (void**)&mapped);

	// マップが成功したら値を書き込む。
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, dataSize);
		resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> PorygonMeshInstance::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
{

	/*===== バッファ全般を生成する処理 =====*/

	// 引数から設定用構造体を設定する。
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

	// 実際にバッファを生成する。
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
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

	// バッファ生成命令を出す。
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// 生成に失敗したら。
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}
