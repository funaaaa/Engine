#include "PolygonInstance.h"
#include "DirectXBase.h"
#include <assert.h>

D3D12_RAYTRACING_INSTANCE_DESC PolygonMeshInstance::CreateInstance(const Microsoft::WRL::ComPtr<ID3D12Resource>& BlassBuffer, const UINT& BlasIndex, const UINT& ShaderID)
{

	/*===== インスタンスを生成する処理 =====*/

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// 移動行列を初期化。
	worldMat_ = DirectX::XMMatrixIdentity();
	matTrans_ = DirectX::XMMatrixIdentity();
	scaleMat_ = DirectX::XMMatrixIdentity();
	matRot_ = DirectX::XMMatrixIdentity();

	// 行列を設定。
	XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat_);

	shaderID_ = ShaderID;

	// インスタンスの詳細を設定。
	instanceDesc.InstanceID = ShaderID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = BlasIndex;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = BlassBuffer->GetGPUVirtualAddress();


	// BLASのIndexを保存。
	blasIndex_ = BlasIndex;

	isActive_ = true;
	childCount_ = 0;

	return instanceDesc;

}

void PolygonMeshInstance::AddTrans(const Vec3& Pos)
{

	/*===== 移動関数 =====*/

	matTrans_ *= DirectX::XMMatrixTranslation(Pos.x_, Pos.y_, Pos.z_);
	pos_ = Vec3(matTrans_.r[3].m128_f32[0], matTrans_.r[3].m128_f32[1], matTrans_.r[3].m128_f32[2]);

}

void PolygonMeshInstance::ChangeTrans(const Vec3& Pos)
{

	/*===== 移動関数 =====*/

	matTrans_ = DirectX::XMMatrixTranslation(Pos.x_, Pos.y_, Pos.z_);
	pos_ = Vec3(matTrans_.r[3].m128_f32[0], matTrans_.r[3].m128_f32[1], matTrans_.r[3].m128_f32[2]);

}

void PolygonMeshInstance::ChangeTrans(const DirectX::XMMATRIX& Trans)
{

	matTrans_ = Trans;

}

void PolygonMeshInstance::AddRotate(const Vec3& Rot)
{

	/*===== 回転関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff *= DirectX::XMMatrixRotationZ(Rot.z_);
	buff *= DirectX::XMMatrixRotationX(Rot.x_);
	buff *= DirectX::XMMatrixRotationY(Rot.y_);

	matRot_ = buff * matRot_;

}

void PolygonMeshInstance::AddRotate(const DirectX::XMMATRIX& Rot)
{

	matRot_ = matRot_ * Rot;

}

void PolygonMeshInstance::ChangeRotate(const Vec3& Rot)
{

	/*===== 回転関数 =====*/

	matRot_ = DirectX::XMMatrixIdentity();

	matRot_ *= DirectX::XMMatrixRotationZ(Rot.z_);
	matRot_ *= DirectX::XMMatrixRotationX(Rot.x_);
	matRot_ *= DirectX::XMMatrixRotationY(Rot.y_);

	rotate_ = Rot;

}

void PolygonMeshInstance::ChangeRotate(const DirectX::XMMATRIX& Rot)
{

	/*===== 回転関数 =====*/

	matRot_ = Rot;

}

void PolygonMeshInstance::AddScale(const Vec3& Scale)
{

	/*===== 拡縮関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x_, Scale.y_, Scale.z_);

	scaleMat_ *= buff;

}

void PolygonMeshInstance::ChangeScale(const Vec3& Scale)
{

	/*===== 拡縮関数 =====*/

	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();

	buff = DirectX::XMMatrixScaling(Scale.x_, Scale.y_, Scale.z_);

	scaleMat_ = buff;

}

void PolygonMeshInstance::ChangeScale(const DirectX::XMMATRIX& Scale)
{

	scaleMat_ = Scale;

}

Vec3 PolygonMeshInstance::GetWorldPos()
{

	/*===== 親子関係も考慮したワールド座標系での座標を取得 =====*/

	// ワールド行列を取得。
	worldMat_ = DirectX::XMMatrixIdentity();

	worldMat_ *= scaleMat_;
	worldMat_ *= matRot_;
	worldMat_ *= matTrans_;

	// 親行列が存在していたらだったら。
	if (!parentInstance_.expired()) {

		worldMat_ *= parentInstance_.lock()->GetWorldMat();

	}

	// 座標を抜き取る。
	Vec3 pos = { worldMat_.r[3].m128_f32[0], worldMat_.r[3].m128_f32[1],worldMat_.r[3].m128_f32[2] };

	return pos;
}

void PolygonMeshInstance::CalWorldMat(D3D12_RAYTRACING_INSTANCE_DESC& Input)
{

	/*===== ワールド行列を計算 =====*/

	worldMat_ = DirectX::XMMatrixIdentity();

	worldMat_ *= scaleMat_;
	worldMat_ *= matRot_;
	worldMat_ *= matTrans_;

	// 親行列が存在していたらだったら。
	if (!parentInstance_.expired()) {

		worldMat_ *= parentInstance_.lock()->GetWorldMat();

	}

	// 設定の行列を更新する。
	DirectX::XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&Input.Transform),
		worldMat_);

}

DirectX::XMMATRIX PolygonMeshInstance::GetWorldMat()
{

	/*===== ワールド行列を取得 =====*/

	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	worldMat *= scaleMat_;
	worldMat *= matRot_;
	worldMat *= matTrans_;

	// 親行列が存在していたらだったら。
	if (!parentInstance_.expired()) {

		worldMat *= parentInstance_.lock()->GetWorldMat();

	}

	return worldMat;

}

void PolygonMeshInstance::SetParentInstance(std::weak_ptr<PolygonMeshInstance> ParentInstance)
{

	/*===== 親行列を設定 =====*/

	parentInstance_ = ParentInstance;
	++parentInstance_.lock()->childCount_;

}

void PolygonMeshInstance::Disable()
{

	/*===== インスタンスを無効化 =====*/

	// 親子関係の子供がいたら。
	if (0 < childCount_) {

		// 親子関係の親を解放しようとしています。子供の挙動が意図しないものになる可能性があります。子供を先に開放してください。
		assert(0);

	}

	isActive_ = false;
	if (!parentInstance_.expired()) {
		--parentInstance_.lock()->childCount_;
	}

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
	hr = DirectXBase::Ins()->dev_->CreateCommittedResource(
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
