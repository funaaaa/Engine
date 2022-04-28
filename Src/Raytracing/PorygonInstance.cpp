#include "PorygonInstance.h"
#include "DirectXBase.h"

D3D12_RAYTRACING_INSTANCE_DESC PorygonMeshInstance::CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& blassBuffer, const UINT& hitGroupIndex, const UINT& instanceID)
{

	/*===== インスタンスを生成する処理 =====*/

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// 移動行列を初期化。
	worldMat = DirectX::XMMatrixIdentity();

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

void PorygonMeshInstance::AddTrans(D3D12_RAYTRACING_INSTANCE_DESC& Input, const Vec3& pos)
{

	/*===== 移動関数 =====*/

	worldMat *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	// 設定の移動行列を更新する。
	DirectX::XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&Input.Transform),
		worldMat);

}

void PorygonMeshInstance::AddRotate(D3D12_RAYTRACING_INSTANCE_DESC& Input, const Vec3& Pos)
{

	/*===== 移動関数 =====*/

	worldMat *= DirectX::XMMatrixRotationZ(Pos.z);
	worldMat *= DirectX::XMMatrixRotationX(Pos.x);
	worldMat *= DirectX::XMMatrixRotationY(Pos.y);

	// 設定の移動行列を更新する。
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
	hr = DirectXBase::Instance()->dev->CreateCommittedResource(
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
