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

std::weak_ptr<PolygonMeshInstance> PolygonInstanceRegister::CreateInstance(const int& BlasIndex, const UINT& ShaderID, bool HaveMeshCollisionData)
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
	D3D12_RAYTRACING_INSTANCE_DESC buff = instance_[instanceIndex]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(BlasIndex), BlasIndex, ShaderID, HaveMeshCollisionData, instanceIndex);

	instanceDesc_[instanceIndex] = buff;

	return instance_[instanceIndex];

}

void PolygonInstanceRegister::CalWorldMat()
{

	for (auto& index_ : instance_) {

		if (!index_->GetIsActive())continue;

		index_->CalWorldMat(instanceDesc_[&index_ - &instance_[0]]);

	}

}

void PolygonInstanceRegister::DestroyInstance(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== 指定のインスタンスを破棄 =====*/

	// インデックスが範囲外だったらassert	。
	int index = Instance.lock()->GetInstanceIndex();
	if (index < 0 || MAX_INSTANCE < index) assert(0);

	instance_[index]->Disable();
	instanceDesc_[index] = {};

}

void PolygonInstanceRegister::DestroyInstance(const int& Index)
{

	/*===== 指定のインスタンスを破棄 =====*/

	// インデックスが範囲外だったらassert	。
	int index = Index;
	if (index < 0 || MAX_INSTANCE < index) assert(0);

	instance_[index]->Disable();
	instanceDesc_[index] = {};

}

void PolygonInstanceRegister::Display(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*====== 非表示 ======*/

	// 行列を保存しておく。
	int index = Instance.lock()->GetInstanceIndex();
	DirectX::XMMATRIX matRot = Instance.lock()->GetRotate();
	DirectX::XMMATRIX matScale = Instance.lock()->GetScale();
	DirectX::XMMATRIX matTrans = Instance.lock()->GetTrans();
	bool haveMeshCollisionData = Instance.lock()->GetHaveMeshCollisionData();

	instanceDesc_[index] = instance_[index]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(instance_[index]->GetBLASIndex()), instance_[index]->GetBLASIndex(), instance_[index]->GetShaderID(), haveMeshCollisionData, index);

	// 保存していた行列をセット。
	Instance.lock()->ChangeRotate(matRot);
	Instance.lock()->ChangeScale(matScale);
	Instance.lock()->ChangeTrans(matTrans);

	instance_[index]->CalWorldMat(instanceDesc_[index]);

}

void PolygonInstanceRegister::Display(const int& Index)
{

	/*====== 非表示 ======*/

	// 行列を保存しておく。
	DirectX::XMMATRIX matRot = instance_[Index]->GetRotate();
	DirectX::XMMATRIX matScale = instance_[Index]->GetScale();
	DirectX::XMMATRIX matTrans = instance_[Index]->GetTrans();
	bool haveMeshCollisionData = instance_[Index]->GetHaveMeshCollisionData();

	instanceDesc_[Index] = instance_[Index]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(instance_[Index]->GetBLASIndex()), instance_[Index]->GetBLASIndex(), instance_[Index]->GetShaderID(), haveMeshCollisionData, Index);

	// 保存していた行列をセット。
	instance_[Index]->ChangeRotate(matRot);
	instance_[Index]->ChangeScale(matScale);
	instance_[Index]->ChangeTrans(matTrans);

	instance_[Index]->CalWorldMat(instanceDesc_[Index]);

}

void PolygonInstanceRegister::NonDisplay(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*====== 非表示 ======*/

	instanceDesc_[Instance.lock()->GetInstanceIndex()] = {};

}

void PolygonInstanceRegister::NonDisplay(const int& Index)
{

	/*====== 非表示 ======*/

	instanceDesc_[Index] = {};

}

const std::vector<FHelper::CheckHitPorygon>& PolygonInstanceRegister::GetMeshCollisionData(const int& Index)
{

	/*===== メッシュの当たり判定データを取得 =====*/

	// メッシュの当たり判定データを持っているかをチェック。
	if (!instance_[Index]->GetHaveMeshCollisionData()) assert(0);

	return instance_[Index]->GetMeshCollisionData();

}
