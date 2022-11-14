#include "PolygonInstance.h"
#include "Engine.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include <assert.h>

D3D12_RAYTRACING_INSTANCE_DESC PolygonMeshInstance::CreateInstance(std::weak_ptr<BLAS> Blas, UINT ShaderID, bool HaveMeshCollisionData, int InstanceIndex)
{

	/*===== インスタンスを生成する処理 =====*/

	instanceIndex_ = InstanceIndex;

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
	instanceDesc.InstanceContributionToHitGroupIndex = Blas.lock()->GetBlasIndex();
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = Blas.lock()->GetBLASBuffer()->GetGPUVirtualAddress();


	// BLASのIndexを保存。
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

	/*===== インスタンスを生成する処理 =====*/

	instanceIndex_ = InstanceIndex;

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

	// 行列を設定。
	XMStoreFloat3x4(
		reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
		worldMat_);

	shaderID_ = ShaderID;

	// インスタンスの詳細を設定。
	instanceDesc.InstanceID = ShaderID;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = Blas.lock()->GetBlasIndex();
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = Blas.lock()->GetBLASBuffer()->GetGPUVirtualAddress();


	// BLASのIndexを保存。
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

	/*===== 移動関数 =====*/

	matTrans_ *= DirectX::XMMatrixTranslation(Pos.x_, Pos.y_, Pos.z_);
	pos_ = Vec3(matTrans_.r[3].m128_f32[0], matTrans_.r[3].m128_f32[1], matTrans_.r[3].m128_f32[2]);

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::ChangeTrans(const Vec3& Pos)
{

	/*===== 移動関数 =====*/

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

	/*===== 回転関数 =====*/

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

	/*===== 回転関数 =====*/

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

	/*===== 回転関数 =====*/

	matRot_ = Rot;

	if (haveMeshCollisionData_) {
		CalMeshCollisionData();
	}

}

void PolygonMeshInstance::AddScale(const Vec3& Scale)
{

	/*===== 拡縮関数 =====*/

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

	/*===== 拡縮関数 =====*/

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

int PolygonMeshInstance::GetParentInstanceIndex()
{

	/*===== 親インスタンスのIDを取得 =====*/

	// 親行列が存在していたらだったら。
	if (!parentInstance_.expired()) {

		return parentInstance_.lock()->GetInstanceIndex();

	}

	// 親が存在していない。
	assert(0);
	return -1;

}

std::weak_ptr<PolygonMeshInstance> PolygonMeshInstance::GetParetntInstance()
{

	/*===== 親インスタンスを取得 =====*/

	// 親行列が存在していたらだったら。
	if (!parentInstance_.expired()) {

		return parentInstance_;

	}

	// 親が存在していない。
	assert(0);
	return std::weak_ptr<PolygonMeshInstance>();

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
	hr = Engine::Ins()->device_.dev_->CreateCommittedResource(
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

void PolygonMeshInstance::CalMeshCollisionData()
{

	/*===== メッシュの当たり判定情報を計算 =====*/

	// メッシュの初期値を取得する。
	std::vector<Vec3> meshDataPos = blas_.lock()->GetVertexPos();
	std::vector<Vec3> meshDataNormal = blas_.lock()->GetVertexNormal();
	std::vector<Vec2> meshDataUV = blas_.lock()->GetVertexUV();
	std::vector<UINT> meshDataIndex = blas_.lock()->GetVertexIndex();

	// ポリゴン数に合わせてリサイズ
	meshCollisionData_.resize(static_cast<unsigned __int64>(static_cast<float>(meshDataPos.size()) / 3.0f));

	// ポリゴンの中身を代入
	for (auto& index : meshCollisionData_) {

		// 頂点1
		index.p1_.pos_ = meshDataPos[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3)])];
		index.p1_.normal_ = meshDataNormal[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3)])];
		index.p1_.uv_ = meshDataUV[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3)])];
		// 頂点2
		index.p2_.pos_ = meshDataPos[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 1)])];
		index.p2_.normal_ = meshDataNormal[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 1)])];
		index.p2_.uv_ = meshDataUV[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 1)])];
		// 頂点3
		index.p3_.pos_ = meshDataPos[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 2)])];
		index.p3_.normal_ = meshDataNormal[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 2)])];
		index.p3_.uv_ = meshDataUV[static_cast<UINT>(meshDataIndex[static_cast<UINT>((&index - &meshCollisionData_[0]) * 3 + 2)])];
		// 有効化フラグ
		index.isActive_ = true;
	}

	/*----- 保存したポリゴンの頂点座標にワールド変換行列をかける -----*/
	// ワールド行列
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	matWorld *= scaleMat_;
	matWorld *= matRot_;
	matWorld *= matTrans_;
	for (auto& index : meshCollisionData_) {
		// 頂点を変換
		index.p1_.pos_ = DirectX::XMVector3Transform(index.p1_.pos_.ConvertXMVECTOR(), matWorld);
		index.p2_.pos_ = DirectX::XMVector3Transform(index.p2_.pos_.ConvertXMVECTOR(), matWorld);
		index.p3_.pos_ = DirectX::XMVector3Transform(index.p3_.pos_.ConvertXMVECTOR(), matWorld);
		// 法線を回転行列分だけ変換
		index.p1_.normal_ = DirectX::XMVector3Transform(index.p1_.normal_.ConvertXMVECTOR(), matRot_);
		index.p1_.normal_.Normalize();
		index.p2_.normal_ = DirectX::XMVector3Transform(index.p2_.normal_.ConvertXMVECTOR(), matRot_);
		index.p2_.normal_.Normalize();
		index.p3_.normal_ = DirectX::XMVector3Transform(index.p3_.normal_.ConvertXMVECTOR(), matRot_);
		index.p3_.normal_.Normalize();
	}

}
