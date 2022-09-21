#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>

void PolygonInstanceRegister::Setting()
{

	/*===== �C���X�^���X���Z�b�e�B���O =====*/

	for (auto& index : instance_) {

		index = std::make_shared<PolygonMeshInstance>();
		instanceDesc_[&index - &instance_[0]] = {};

	}

}

std::weak_ptr<PolygonMeshInstance> PolygonInstanceRegister::CreateInstance(const int& BlasIndex, const UINT& ShaderID, bool HaveMeshCollisionData)
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

			index.AccelerationStructure = BLASRegister::Ins()->GetBLASBuffer(0)->GetGPUVirtualAddress();

		}

	}

	// InstanceIndex��-1��������ő吔�𒴂��Ă���Ƃ������ƂȂ̂ŁAassert���o���B
	if (instanceIndex == -1) {

		assert(0);

	}

	// �Ō���̂�𐶐�����B
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

	/*===== �w��̃C���X�^���X��j�� =====*/

	// �C���f�b�N�X���͈͊O��������assert	�B
	int index = Instance.lock()->GetInstanceIndex();
	if (index < 0 || MAX_INSTANCE < index) assert(0);

	instance_[index]->Disable();
	instanceDesc_[index] = {};

}

void PolygonInstanceRegister::DestroyInstance(const int& Index)
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
	DirectX::XMMATRIX matRot = Instance.lock()->GetRotate();
	DirectX::XMMATRIX matScale = Instance.lock()->GetScale();
	DirectX::XMMATRIX matTrans = Instance.lock()->GetTrans();
	bool haveMeshCollisionData = Instance.lock()->GetHaveMeshCollisionData();

	instanceDesc_[index] = instance_[index]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(instance_[index]->GetBLASIndex()), instance_[index]->GetBLASIndex(), instance_[index]->GetShaderID(), haveMeshCollisionData, index);

	// �ۑ����Ă����s����Z�b�g�B
	Instance.lock()->ChangeRotate(matRot);
	Instance.lock()->ChangeScale(matScale);
	Instance.lock()->ChangeTrans(matTrans);

	instance_[index]->CalWorldMat(instanceDesc_[index]);

}

void PolygonInstanceRegister::Display(const int& Index)
{

	/*====== ��\�� ======*/

	// �s���ۑ����Ă����B
	DirectX::XMMATRIX matRot = instance_[Index]->GetRotate();
	DirectX::XMMATRIX matScale = instance_[Index]->GetScale();
	DirectX::XMMATRIX matTrans = instance_[Index]->GetTrans();
	bool haveMeshCollisionData = instance_[Index]->GetHaveMeshCollisionData();

	instanceDesc_[Index] = instance_[Index]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(instance_[Index]->GetBLASIndex()), instance_[Index]->GetBLASIndex(), instance_[Index]->GetShaderID(), haveMeshCollisionData, Index);

	// �ۑ����Ă����s����Z�b�g�B
	instance_[Index]->ChangeRotate(matRot);
	instance_[Index]->ChangeScale(matScale);
	instance_[Index]->ChangeTrans(matTrans);

	instance_[Index]->CalWorldMat(instanceDesc_[Index]);

}

void PolygonInstanceRegister::NonDisplay(std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*====== ��\�� ======*/

	instanceDesc_[Instance.lock()->GetInstanceIndex()] = {};

}

void PolygonInstanceRegister::NonDisplay(const int& Index)
{

	/*====== ��\�� ======*/

	instanceDesc_[Index] = {};

}

const std::vector<FHelper::CheckHitPorygon>& PolygonInstanceRegister::GetMeshCollisionData(const int& Index)
{

	/*===== ���b�V���̓����蔻��f�[�^���擾 =====*/

	// ���b�V���̓����蔻��f�[�^�������Ă��邩���`�F�b�N�B
	if (!instance_[Index]->GetHaveMeshCollisionData()) assert(0);

	return instance_[Index]->GetMeshCollisionData();

}
