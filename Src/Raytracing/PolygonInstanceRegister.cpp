#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>

void PolygonInstanceRegister::Setting()
{

	/*===== �C���X�^���X���Z�b�e�B���O =====*/

	for (auto& index_ : instance) {

		index_ = std::make_shared<PolygonMeshInstance>();
		instanceDesc[&index_ - &instance[0]] = {};

	}

}

int PolygonInstanceRegister::CreateInstance(const int& BlasIndex, const UINT& InstanceID)
{

	/*===== �C���X�^���X�𐶐����� =====*/

	// ��������C���X�^���X�̃C���f�b�N�X�����߂�B
	int instanceIndex = -1;
	for (auto& index_ : instance) {

		if (index_->GetIsActive()) continue;

		instanceIndex = static_cast<int>(&index_ - &instance[0]);

		break;

	}

	// �ŏ���Instance��������A�S�Ă̗v�f��BLAS���A�h���X�𖄂߂�B��������Ȃ��ƁATLAS�𐶐�����ۂ�BLAS�̃A�h���X�������ĂȂ��T�C�Y���J�b�g����Ă��܂��A
	// �ŏ���1F�ł���Instance�𐶐��ł��Ȃ��Ȃ��Ă��܂�����B
	// �����ꏈ����ς������B
	if (instanceIndex == 0) {

		for (auto& index_ : instanceDesc) {

			index_.AccelerationStructure = BLASRegister::Ins()->GetBLASBuffer(0)->GetGPUVirtualAddress();

		}

	}

	// InstanceIndex��-1��������ő吔�𒴂��Ă���Ƃ������ƂȂ̂ŁAassert���o���B
	if (instanceIndex == -1) {

		assert(0);

	}

	// �Ō���̂�𐶐�����B
	D3D12_RAYTRACING_INSTANCE_DESC buff = instance[instanceIndex]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(BlasIndex), BlasIndex, InstanceID);

	instanceDesc[instanceIndex] = buff;

	return instanceIndex;

}

// �ړ�(���������Z)�֐�
void PolygonInstanceRegister::AddTrans(const int& Index, const float& X, const float& Y, const float Z) {

	instance[Index]->AddTrans(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::AddTrans(const int& Index, const Vec3& Pos) {

	instance[Index]->AddTrans(Pos);

}

void PolygonInstanceRegister::ChangeTrans(const int& Index, const float& X, const float& Y, const float Z)
{

	instance[Index]->ChangeTrans(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::ChangeTrans(const int& Index, const Vec3& Pos)
{

	instance[Index]->ChangeTrans(Pos);

}

void PolygonInstanceRegister::ChangeTrans(const int& Index, DirectX::XMMATRIX Trans)
{

	instance[Index]->ChangeTrans(Trans);

}

DirectX::XMMATRIX PolygonInstanceRegister::GetTrans(const int& Index)
{
	return instance[Index]->GetTrans();
}

Vec3 PolygonInstanceRegister::GetPos(const int& Index)
{
	return instance[Index]->GetPos();
}

// ��](���W�A���A���������Z)�֐�
void PolygonInstanceRegister::AddRotate(const int& Index, const float& X, const float& Y, const float Z) {

	instance[Index]->AddRotate(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::AddRotate(const int& Index, const Vec3& Rot) {

	instance[Index]->AddRotate(Rot);

}

void PolygonInstanceRegister::AddRotate(const int& Index, const DirectX::XMMATRIX& Rot)
{
	instance[Index]->AddRotate(Rot);
}

DirectX::XMMATRIX PolygonInstanceRegister::GetRotate(const int& Index)
{
	return instance[Index]->GetRotate();
}

Vec3 PolygonInstanceRegister::GetRotVec3(const int& Index)
{
	return instance[Index]->GetRotVec3();
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, const float& X, const float& Y, const float Z)
{
	instance[Index]->ChangeRotate(Vec3(X, Y, Z));
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, const Vec3& Rot)
{
	instance[Index]->ChangeRotate(Rot);
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, DirectX::XMMATRIX Rot)
{
	instance[Index]->ChangeRotate(Rot);
}

void PolygonInstanceRegister::AddScale(const int& Index, const float& X, const float& Y, const float Z)
{

	instance[Index]->AddScale(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::AddScale(const int& Index, const Vec3& Scale)
{

	instance[Index]->AddScale(Scale);

}

DirectX::XMMATRIX PolygonInstanceRegister::GetScale(const int& Index)
{
	return instance[Index]->GetScale();
}

Vec3 PolygonInstanceRegister::GetScaleVec3(const int& Index)
{
	return instance[Index]->GetScaleVec3();
}

void PolygonInstanceRegister::ChangeScale(const int& Index, const float& X, const float& Y, const float Z)
{

	instance[Index]->ChangeScale(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::ChangeScale(const int& Index, const Vec3& Scale)
{

	instance[Index]->ChangeScale(Scale);

}

void PolygonInstanceRegister::ChangeScale(const int& Index, DirectX::XMMATRIX Scale)
{

	instance[Index]->ChangeScale(Scale);

}

void PolygonInstanceRegister::SetParentInstance(const int& Index, const int& ParentIndex)
{

	/*===== �e�s���ݒ� =====*/

	instance[Index]->SetParentInstance(instance[ParentIndex]);

}

UINT PolygonInstanceRegister::GetBLASIndex(const int& Index)
{
	return instance[Index]->GetBLASIndex();
}

void PolygonInstanceRegister::CalWorldMat()
{

	for (auto& index_ : instance) {

		if (!index_->GetIsActive())continue;

		index_->CalWorldMat(instanceDesc[&index_ - &instance[0]]);

	}

}

void PolygonInstanceRegister::DestroyInstance(const int& Index)
{

	/*===== �w��̃C���X�^���X��j�� =====*/

	// �C���f�b�N�X���͈͊O��������assert	�B
	if (Index < 0 || MAX_INSTANCE < Index) assert(0);

	instance[Index]->Disable();
	instanceDesc[Index] = {};

}
