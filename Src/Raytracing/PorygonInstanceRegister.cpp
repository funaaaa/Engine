#include "PorygonInstanceRegister.h"
#include "PorygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>

int PorygonInstanceRegister::CreateInstance(const int& BlasIndex, const UINT& InstanceID)
{

	/*===== インスタンスを生成する =====*/

	// 最後尾にプッシュ。
	instance.emplace_back(std::make_shared<PorygonMeshInstance>());

	// 最後尾のやつを生成する。
	D3D12_RAYTRACING_INSTANCE_DESC buff = instance[instance.size() - 1]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(BlasIndex), BlasIndex, InstanceID);

	// InstanceDescを保持。
	instanceDesc.emplace_back(buff);

	return instance.size() - 1;

}

// 移動(引数を加算)関数
void PorygonInstanceRegister::AddTrans(const int& Index, const float& X, const float& Y, const float Z) {

	instance[Index]->AddTrans(Vec3(X, Y, Z));

}

void PorygonInstanceRegister::AddTrans(const int& Index, const Vec3& Pos) {

	instance[Index]->AddTrans(Pos);

}

void PorygonInstanceRegister::ChangeTrans(const int& Index, const float& X, const float& Y, const float Z)
{

	instance[Index]->ChangeTrans(Vec3(X, Y, Z));

}

void PorygonInstanceRegister::ChangeTrans(const int& Index, const Vec3& Pos)
{

	instance[Index]->ChangeTrans(Pos);

}

// 回転(ラジアン、引数を加算)関数
void PorygonInstanceRegister::AddRotate(const int& Index, const float& X, const float& Y, const float Z) {

	instance[Index]->AddRotate(Vec3(X, Y, Z));

}

void PorygonInstanceRegister::AddRotate(const int& Index, const Vec3& Rot) {

	instance[Index]->AddRotate(Rot);

}

void PorygonInstanceRegister::AddScale(const int& Index, const float& X, const float& Y, const float Z)
{

	instance[Index]->AddScale(Vec3(X, Y, Z));

}

void PorygonInstanceRegister::AddScale(const int& Index, const Vec3& Scale)
{

	instance[Index]->AddScale(Scale);

}

void PorygonInstanceRegister::ChangeScale(const int& Index, const float& X, const float& Y, const float Z)
{

	instance[Index]->ChangeScale(Vec3(X, Y, Z));

}

void PorygonInstanceRegister::ChangeScale(const int& Index, const Vec3& Scale)
{

	instance[Index]->ChangeScale(Scale);

}

void PorygonInstanceRegister::CalWorldMat()
{

	const int INSTANCE_SIZE = instance.size();
	for (int index = 0; index < INSTANCE_SIZE; ++index) {

		instance[index]->CalWorldMat(instanceDesc[index]);

	}

}
