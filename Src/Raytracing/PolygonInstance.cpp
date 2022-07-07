#include "PolygonInstance.h"
#include "DirectXBase.h"

D3D12_RAYTRACING_INSTANCE_DESC PolygonMeshInstance::CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& InstanceID)
{

	/*===== インスタンスを生成する処理 =====*/

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// 移動行列を初期化。
	worldMat = DirectX::XMMatrixIdentity();
	matTrans = DirectX::XMMatrixIdentity();
	scaleMat = DirectX::XMMatrixIdentity();
	matRot = DirectX::XMMatrixIdentity();

	// 行列を設定。
	XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat);

	// インスタンスの詳細を設定。
	instanceDesc.InstanceID = InstanceID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = BlasIndex;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = BlassBuffer->GetGPUVirtualAddress();

	// BLASのIndexを保存。
	blasIndex = BlasIndex;

	return instanceDesc;

}

void PolygonMeshInstance::AddTrans(const Vec3& Pos)
{

	/*===== 移動関数 =====*/

	matTrans *= DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	pos = Vec3(matTrans.r[3].m128_f32[0], matTrans.r[3].m128_f32[1], matTrans.r[3].m128_f32[2]);

}

void PolygonMeshInstance::ChangeTrans(const Vec3& Pos)
{

	/*===== 移動関数 =====*/

	matTrans = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	pos = Vec3(matTrans.r[3].m128_f32[0], matTrans.r[3].m128_f32[1], matTrans.r[3].m128_f32[2]);

}

void PolygonMeshInstance::AddRotate(const Vec3& Rot)
{

	/*===== 回転関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff *= DirectX::XMMatrixRotationZ(Rot.z);
	buff *= DirectX::XMMatrixRotationX(Rot.x);
	buff *= DirectX::XMMatrixRotationY(Rot.y);

	matRot = buff * matRot;

}

void PolygonMeshInstance::AddRotate(const DirectX::XMMATRIX& Rot)
{

	matRot = matRot * Rot;

}

void PolygonMeshInstance::ChangeRotate(const Vec3& Rot)
{

	/*===== 回転関数 =====*/

	matRot = DirectX::XMMatrixIdentity();

	matRot *= DirectX::XMMatrixRotationZ(Rot.z);
	matRot *= DirectX::XMMatrixRotationX(Rot.x);
	matRot *= DirectX::XMMatrixRotationY(Rot.y);

}

void PolygonMeshInstance::ChangeRotate(const DirectX::XMMATRIX& Rot)
{

	/*===== 回転関数 =====*/

	matRot = Rot;

}

void PolygonMeshInstance::AddScale(const Vec3& Scale)
{

	/*===== 拡縮関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	scaleMat *= buff;

}

void PolygonMeshInstance::ChangeScale(const Vec3& Scale)
{

	/*===== 拡縮関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	scaleMat = buff;

}

void PolygonMeshInstance::CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input)
{

	/*===== ワールド行列を計算 =====*/

	worldMat = DirectX::XMMatrixIdentity();

	worldMat *= scaleMat;
	worldMat *= matRot;
	worldMat *= matTrans;

	// 設定の行列を更新する。
	DirectX::XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&Input.Transform),
		worldMat);

}

void PolygonMeshInstance::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize)
{

	/*===== メモリに値を書き込む処理 =====*/

	// nullチェック。
	if (Resource == nullptr) return;

	// マップ処理を行う。
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, DataSize };
	HRESULT hr = Resource->Map(0, nullptr, (void**)&mapped);

	// マップが成功したら値を書き込む。
	if (SUCCEEDED(hr)) {

		memcpy(mapped, PData, DataSize);
		Resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> PolygonMeshInstance::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== バッファ全般を生成する処理 =====*/

	// 引数から設定用構造体を設定する。
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

	// 実際にバッファを生成する。
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

	// バッファ生成命令を出す。
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		InitialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// 生成に失敗したら。
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}
