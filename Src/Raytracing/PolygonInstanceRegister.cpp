#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>
#include "BLAS.h"

void PolygonInstanceRegister::Setting()
{

	/*===== �C���X�^���X���Z�b�e�B���O =====*/

	for (auto& index : instance_) {

		index = std::make_shared<PolygonMeshInstance>();
		instanceDesc_[&index - &instance_[0]] = {};

	}

}

std::weak_ptr<PolygonMeshInstance> PolygonInstanceRegister::CreateInstance(std::weak_ptr<BLAS> Blas_, UINT ShaderID, bool HaveMeshCollisionData)
{

	/*===== �C���X�^���X�𐶐����� =====*/

	// ��������C���X�^���X�̃C���f�b�N�X�����߂�B
	int instanceIndex = -1;
	for (auto& index : instance_) {

		if (index->GetIsActive()) continue;

		instanceIndex = static_cast<int>(&index - &instance_[0]);

		break;

	}

	// �ŏ���Instance��������A�S�Ă̗v�f��BLAS���A�h���X�𖄂߂�B��������Ȃ��ƁATLAS�𐶐�����ۂ�BLAS�̃A�h���X�������ĂȂ��T�C�Y���J�b�g����Ă��܂��A
	// �ŏ���1F�ł���Instance�𐶐��ł��Ȃ��Ȃ��Ă��܂�����B
	// �����ꏈ����ς������B
	if (instanceIndex == 0) {

		for (auto& index : instanceDesc_) {

			index.AccelerationStructure = Blas_.lock()->GetBLASBuffer()->GetGPUVirtualAddress();

		}

	}

	// InstanceIndex��-1��������ő吔�𒴂��Ă���Ƃ������ƂȂ̂ŁAassert���o���B
	if (instanceIndex == -1) {

		assert(0);

	}

	// �Ō���̂�𐶐�����B
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

	/*===== �w��̃C���X�^���X��j�� =====*/

	if (Instance.expired()) return;
	// �C���f�b�N�X���͈͊O��������assert	�B
	int index = Instance.lock()->GetInstanceIndex();
	if (index < 0 || MAX_INSTANCE < index) assert(0);

	instance_[index]->Disable();
	instanceDesc_[index] = {};

}

void PolygonInstanceRegister::DestroyInstance(int Index)
{

	/*===== �w��̃C���X�^���X��j�� =====*/

	// �C���f�b�N�X���͈͊O��������assert	�B
	int index = Index;
	if (index < 0 || MAX_INSTANCE < index) assert(0);

	instance_[index]->Disable();
	instanceDesc_[index] = {};

}

void PolygonInstanceRegister::Display(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*====== ��\�� ======*/

	// �s���ۑ����Ă����B
	int index = Instance.lock()->GetInstanceIndex();
	DirectX::XMMATRIX matRot = savedInstanceData_[index].matRot_;
	DirectX::XMMATRIX matScale = savedInstanceData_[index].matScale_;
	DirectX::XMMATRIX matTrans = savedInstanceData_[index].matTrans_;
	bool haveMeshCollisionData = Instance.lock()->GetHaveMeshCollisionData();

	// �ۑ����Ă����s����Z�b�g�B
	Instance.lock()->ChangeRotate(matRot);
	Instance.lock()->ChangeScale(matScale);
	Instance.lock()->ChangeTrans(matTrans);

	instance_[index]->CalWorldMat(instanceDesc_[index]);

	instanceDesc_[index] = instance_[index]->ReCreateInstance(BLASRegister::Ins()->GetBlasSpecificationIndex(instance_[index]->GetBLASIndex()), instance_[index]->GetShaderID(), haveMeshCollisionData, index);


}

void PolygonInstanceRegister::Display(int Index)
{

	/*====== ��\�� ======*/

	// �s���ۑ����Ă����B
	DirectX::XMMATRIX matRot = savedInstanceData_[Index].matRot_;
	DirectX::XMMATRIX matScale = savedInstanceData_[Index].matScale_;
	DirectX::XMMATRIX matTrans = savedInstanceData_[Index].matTrans_;
	bool haveMeshCollisionData = instance_[Index]->GetHaveMeshCollisionData();

	// �ۑ����Ă����s����Z�b�g�B
	instance_[Index]->ChangeRotate(matRot);
	instance_[Index]->ChangeScale(matScale);
	instance_[Index]->ChangeTrans(matTrans);

	instance_[Index]->CalWorldMat(instanceDesc_[Index]);

	instanceDesc_[Index] = instance_[Index]->ReCreateInstance(BLASRegister::Ins()->GetBlasSpecificationIndex(instance_[Index]->GetBLASIndex()), instance_[Index]->GetShaderID(), haveMeshCollisionData, Index);


}

void PolygonInstanceRegister::NonDisplay(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*====== ��\�� ======*/

	CalWorldMat(Instance.lock()->GetInstanceIndex());

	savedInstanceData_[Instance.lock()->GetInstanceIndex()].matRot_ = Instance.lock()->GetRotate();
	savedInstanceData_[Instance.lock()->GetInstanceIndex()].matScale_ = Instance.lock()->GetScale();
	savedInstanceData_[Instance.lock()->GetInstanceIndex()].matTrans_ = Instance.lock()->GetTrans();

	instanceDesc_[Instance.lock()->GetInstanceIndex()] = {};

}

void PolygonInstanceRegister::NonDisplay(int Index)
{

	/*====== ��\�� ======*/

	CalWorldMat(Index);

	savedInstanceData_[Index].matRot_ = instance_[Index]->GetRotate();
	savedInstanceData_[Index].matScale_ = instance_[Index]->GetScale();
	savedInstanceData_[Index].matTrans_ = instance_[Index]->GetTrans();

	instanceDesc_[Index] = {};

}

const std::vector<FHelper::CheckHitPorygon>& PolygonInstanceRegister::GetMeshCollisionData(int Index)
{

	/*===== ���b�V���̓����蔻��f�[�^���擾 =====*/

	// ���b�V���̓����蔻��f�[�^�������Ă��邩���`�F�b�N�B
	if (!instance_[Index]->GetHaveMeshCollisionData()) assert(0);

	return instance_[Index]->GetMeshCollisionData();

}
