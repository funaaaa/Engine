#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>
#include "BLAS.h"

void PolygonInstanceRegister::Setting()
{

	/*===== インスタンスをセッティング =====*/

	for (auto& index : instance_) {

		index = std::make_shared<PolygonMeshInstance>();
		instanceDesc_[&index - &instance_[0]] = {};

	}

}

std::weak_ptr<PolygonMeshInstance> PolygonInstanceRegister::CreateInstance(std::weak_ptr<BLAS> Blas_, UINT ShaderID, bool HaveMeshCollisionData)
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

			index.AccelerationStructure = Blas_.lock()->GetBLASBuffer()->GetGPUVirtualAddress();

		}

	}

	// InstanceIndexが-1だったら最大数を超えているということなので、assertを出す。
	if (instanceIndex == -1) {

		assert(0);

	}

	// 最後尾のやつを生成する。
	D3D12_RAYTRACING_INSTANCE_DESC buff = instance_[instanceIndex]->CreateInstance(Blas_, ShaderID, HaveMeshCollisionData, instanceIndex);

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

void PolygonInstanceRegister::CalWorldMat(int Index)
{
	instance_[Index]->CalWorldMat(instanceDesc_[Index]);
}

void PolygonInstanceRegister::DestroyInstance(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== 指定のインスタンスを破棄 =====*/

	if (Instance.expired()) return;
	// インデックスが範囲外だったらassert	。
	int index = Instance.lock()->GetInstanceIndex();
	if (index < 0 || MAX_INSTANCE < index) assert(0);

	instance_[index]->Disable();
	instanceDesc_[index] = {};

}

void PolygonInstanceRegister::DestroyInstance(int Index)
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
	DirectX::XMMATRIX matRot = savedInstanceData_[index].matRot_;
	DirectX::XMMATRIX matScale = savedInstanceData_[index].matScale_;
	DirectX::XMMATRIX matTrans = savedInstanceData_[index].matTrans_;
	bool haveMeshCollisionData = Instance.lock()->GetHaveMeshCollisionData();

	// 保存していた行列をセット。
	Instance.lock()->ChangeRotate(matRot);
	Instance.lock()->ChangeScale(matScale);
	Instance.lock()->ChangeTrans(matTrans);

	instance_[index]->CalWorldMat(instanceDesc_[index]);

	instanceDesc_[index] = instance_[index]->ReCreateInstance(BLASRegister::Ins()->GetBlasSpecificationIndex(instance_[index]->GetBLASIndex()), instance_[index]->GetShaderID(), haveMeshCollisionData, index);


}

void PolygonInstanceRegister::Display(int Index)
{

	/*====== 非表示 ======*/

	// 行列を保存しておく。
	DirectX::XMMATRIX matRot = savedInstanceData_[Index].matRot_;
	DirectX::XMMATRIX matScale = savedInstanceData_[Index].matScale_;
	DirectX::XMMATRIX matTrans = savedInstanceData_[Index].matTrans_;
	bool haveMeshCollisionData = instance_[Index]->GetHaveMeshCollisionData();

	// 保存していた行列をセット。
	instance_[Index]->ChangeRotate(matRot);
	instance_[Index]->ChangeScale(matScale);
	instance_[Index]->ChangeTrans(matTrans);

	instance_[Index]->CalWorldMat(instanceDesc_[Index]);

	instanceDesc_[Index] = instance_[Index]->ReCreateInstance(BLASRegister::Ins()->GetBlasSpecificationIndex(instance_[Index]->GetBLASIndex()), instance_[Index]->GetShaderID(), haveMeshCollisionData, Index);


}

void PolygonInstanceRegister::NonDisplay(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*====== 非表示 ======*/

	CalWorldMat(Instance.lock()->GetInstanceIndex());

	savedInstanceData_[Instance.lock()->GetInstanceIndex()].matRot_ = Instance.lock()->GetRotate();
	savedInstanceData_[Instance.lock()->GetInstanceIndex()].matScale_ = Instance.lock()->GetScale();
	savedInstanceData_[Instance.lock()->GetInstanceIndex()].matTrans_ = Instance.lock()->GetTrans();

	instanceDesc_[Instance.lock()->GetInstanceIndex()] = {};

}

void PolygonInstanceRegister::NonDisplay(int Index)
{

	/*====== 非表示 ======*/

	CalWorldMat(Index);

	savedInstanceData_[Index].matRot_ = instance_[Index]->GetRotate();
	savedInstanceData_[Index].matScale_ = instance_[Index]->GetScale();
	savedInstanceData_[Index].matTrans_ = instance_[Index]->GetTrans();

	instanceDesc_[Index] = {};

}

const std::vector<FHelper::CheckHitPorygon>& PolygonInstanceRegister::GetMeshCollisionData(int Index)
{

	/*===== メッシュの当たり判定データを取得 =====*/

	// メッシュの当たり判定データを持っているかをチェック。
	if (!instance_[Index]->GetHaveMeshCollisionData()) assert(0);

	return instance_[Index]->GetMeshCollisionData();

}
