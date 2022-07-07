#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "BLASRegister.h"
#include <assert.h>

int PolygonInstanceRegister::CreateInstance(const int& BlasIndex, const UINT& InstanceID)
{

	/*===== インスタンスを生成する =====*/

	// 最後尾にプッシュ。
	instance.emplace_back(std::make_shared<PolygonMeshInstance>());

	// 最後尾のやつを生成する。
	D3D12_RAYTRACING_INSTANCE_DESC buff = instance[instance.size() - 1]->CreateInstance(BLASRegister::Ins()->GetBLASBuffer(BlasIndex), BlasIndex, InstanceID);

	// InstanceDescを保持。
	instanceDesc.emplace_back(buff);

	return static_cast<int>(instance.size()) - 1;

}

// 移動(引数を加算)関数
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

DirectX::XMMATRIX PolygonInstanceRegister::GetTrans(const int& Index)
{
	return instance[Index]->GetTrans();
}

Vec3 PolygonInstanceRegister::GetPos(const int& Index)
{
	return instance[Index]->GetPos();
}

// 回転(ラジアン、引数を加算)関数
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

void PolygonInstanceRegister::ChangeRotate(const int& Index, const float& X, const float& Y, const float Z)
{
	instance[Index]->ChangeRotate(Vec3(X, Y, Z));
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, const Vec3& Rot)
{
	instance[Index]->ChangeRotate(Rot);
}

void PolygonInstanceRegister::ChangeRotate(const int& Index, const DirectX::XMMATRIX& Rot)
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

void PolygonInstanceRegister::ChangeScale(const int& Index, const float& X, const float& Y, const float Z)
{

	instance[Index]->ChangeScale(Vec3(X, Y, Z));

}

void PolygonInstanceRegister::ChangeScale(const int& Index, const Vec3& Scale)
{

	instance[Index]->ChangeScale(Scale);

}

UINT PolygonInstanceRegister::GetBLASIndex(const int& Index)
{
	return instance[Index]->GetBLASIndex();
}

void PolygonInstanceRegister::CalWorldMat()
{

	const int INSTANCE_SIZE = static_cast<int>(instance.size());
	for (int index = 0; index < INSTANCE_SIZE; ++index) {

		instance[index]->CalWorldMat(instanceDesc[index]);

	}

}
