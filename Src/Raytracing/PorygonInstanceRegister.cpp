#include "PorygonInstanceRegister.h"
#include "PorygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>

int PorygonInstanceRegister::CreateInstance(const int& BlasIndex, const UINT& InstanceID)
{

	/*===== �C���X�^���X�𐶐����� =====*/

	// �Ō���Ƀv�b�V���B
	instance.emplace_back(std::make_shared<PorygonMeshInstance>());

	// �Ō���̂�𐶐�����B
	D3D12_RAYTRACING_INSTANCE_DESC buff = instance[instance.size() - 1]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(BlasIndex), BlasIndex, InstanceID);

	// InstanceDesc��ێ��B
	instanceDesc.emplace_back(buff);

	return instance.size() - 1;

}

// �ړ�(���������Z)�֐�
void PorygonInstanceRegister::AddTrans(const int& Index, const float& X, const float& Y, const float Z) {

	instance[Index]->AddTrans(instanceDesc[Index], Vec3(X, Y, Z));

}

void PorygonInstanceRegister::AddTrans(const int& Index, const Vec3& Pos) {

	instance[Index]->AddTrans(instanceDesc[Index], Pos);

}

// ��](���W�A���A���������Z)�֐�
void PorygonInstanceRegister::AddRotate(const int& Index, const float& X, const float& Y, const float Z) {

	instance[Index]->AddRotate(instanceDesc[Index], Vec3(X, Y, Z));

}

void PorygonInstanceRegister::AddRotate(const int& Index, const Vec3& Pos) {

	instance[Index]->AddRotate(instanceDesc[Index], Pos);

}