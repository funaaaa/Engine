#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>

void PolygonInstanceRegister::Setting()
{

	/*===== インスタンスをセッティング =====*/

	for (auto& index : instance_) {

		index = std::make_shared<PolygonMeshInstance>();
		instanceDesc_[&index - &instance_[0]] = {};

	}

}

int PolygonInstanceRegister::CreateInstance(const int& BlasIndex, const UINT& ShaderID)
{

	/*===== インスタンスを生成する =====*/

	// 生成するインスタンスのインデックスを求める。
	int instanceIndex = -1;
	for (auto& index : instance_) {

		if (index->GetIsActive()) continue;

		instanceIndex = static_cast<int>(&index - &instance_[0]);

		break;

	}

	// 最初のInstanceだったら、全ての要素のBLASをアドレスを埋める。これをしないと、TLASを生成する際にBLASのアドレスが入ってないサイズがカットされてしまい、
	// 最初の1FでしかInstanceを生成できなくなってしまうから。
	// いずれ処理を変えたい。
	if (instanceIndex == 0) {

		for (auto& index : instanceDesc_) {

			index.AccelerationStructure = BLASRegister::Ins()->GetBLASBuffer(0)->GetGPUVirtualAddress();

		}

	}

	// InstanceIndexが-1だったら最大数を超えているということなので、assertを出す。
	if (instanceIndex == -1) {

		assert(0);

	}

	// 最後尾のやつを生成する。
	D3D12_RAYTRACING_INSTANCE_DESC buff = instance_[instanceIndex]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(BlasIndex), BlasIndex, ShaderID);

	instanceDesc_[instanceIndex] = buff;

	return instanceIndex;

}

// 移動(引数を加算)関数
void PolygonInstanceRegister::AddTrans(const int& Index, const float& X, const float& Y, const float Z) {

	instance_[Index]->AddTrans(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::AddTrans(const int& Index, const Vec3& Pos) {

	instance_[Index]->AddTrans(Pos);

}

void PolygonInstanceRegister::ChangeTrans(const int& Index, const float& X, const float& Y, const float Z)
{

	instance_[Index]->ChangeTrans(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::ChangeTrans(const int& Index, const Vec3& Pos)
{

	instance_[Index]->ChangeTrans(Pos);

}

void PolygonInstanceRegister::ChangeTrans(const int& Index, DirectX::XMMATRIX Trans)
{

	instance_[Index]->ChangeTrans(Trans);

}

DirectX::XMMATRIX PolygonInstanceRegister::GetTrans(const int& Index)
{
	return instance_[Index]->GetTrans();
}

Vec3 PolygonInstanceRegister::GetPos(const int& Index)
{
	return instance_[Index]->GetPos();
}

// 回転(ラジアン、引数を加算)関数
void PolygonInstanceRegister::AddRotate(const int& Index, const float& X, const float& Y, const float Z) {

	instance_[Index]->AddRotate(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::AddRotate(const int& Index, const Vec3& Rot) {

	instance_[Index]->AddRotate(Rot);

}

void PolygonInstanceRegister::AddRotate(const int& Index, const DirectX::XMMATRIX& Rot)
{
	instance_[Index]->AddRotate(Rot);
}

DirectX::XMMATRIX PolygonInstanceRegister::GetRotate(const int& Index)
{
	return instance_[Index]->GetRotate();
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, const float& X, const float& Y, const float Z)
{
	instance_[Index]->ChangeRotate(Vec3(X, Y, Z));
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, const Vec3& Rot)
{
	instance_[Index]->ChangeRotate(Rot);
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, DirectX::XMMATRIX Rot)
{
	instance_[Index]->ChangeRotate(Rot);
}

void PolygonInstanceRegister::AddScale(const int& Index, const float& X, const float& Y, const float Z)
{

	instance_[Index]->AddScale(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::AddScale(const int& Index, const Vec3& Scale)
{

	instance_[Index]->AddScale(Scale);

}

DirectX::XMMATRIX PolygonInstanceRegister::GetScale(const int& Index)
{
	return instance_[Index]->GetScale();
}
void PolygonInstanceRegister::ChangeScale(const int& Index, const float& X, const float& Y, const float Z)
{

	instance_[Index]->ChangeScale(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::ChangeScale(const int& Index, const Vec3& Scale)
{

	instance_[Index]->ChangeScale(Scale);

}

void PolygonInstanceRegister::ChangeScale(const int& Index, DirectX::XMMATRIX Scale)
{

	instance_[Index]->ChangeScale(Scale);

}

void PolygonInstanceRegister::SetParentInstance(const int& Index, const int& ParentIndex)
{

	/*===== 親行列を設定 =====*/

	instance_[Index]->SetParentInstance(instance_[ParentIndex]);

}

UINT PolygonInstanceRegister::GetBLASIndex(const int& Index)
{
	return instance_[Index]->GetBLASIndex();
}

void PolygonInstanceRegister::CalWorldMat()
{

	for (auto& index_ : instance_) {

		if (!index_->GetIsActive())continue;

		index_->CalWorldMat(instanceDesc_[&index_ - &instance_[0]]);

	}

}

void PolygonInstanceRegister::DestroyInstance(const int& Index)
{

	/*===== 指定のインスタンスを破棄 =====*/

	// インデックスが範囲外だったらassert	。
	if (Index < 0 || MAX_INSTANCE < Index) assert(0);

	instance_[Index]->Disable();
	instanceDesc_[Index] = {};

}

void PolygonInstanceRegister::Display(const int& Index)
{

	/*====== 非表示 ======*/

	// 行列を保存しておく。
	DirectX::XMMATRIX matRot = instance_[Index]->GetRotate();
	DirectX::XMMATRIX matScale = instance_[Index]->GetScale();
	DirectX::XMMATRIX matTrans = instance_[Index]->GetTrans();

	instanceDesc_[Index] = instance_[Index]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(instance_[Index]->GetBLASIndex()), instance_[Index]->GetBLASIndex(), instance_[Index]->GetShaderID());

	// 保存していた行列をセット。
	ChangeRotate(Index, matRot);
	ChangeScale(Index, matScale);
	ChangeTrans(Index, matTrans);

	instance_[Index]->CalWorldMat(instanceDesc_[Index]);

}

void PolygonInstanceRegister::NonDisplay(const int& Index)
{

	/*====== 非表示 ======*/

	instanceDesc_[Index] = {};

}
