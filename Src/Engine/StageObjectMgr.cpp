#include "StageObjectMgr.h"
#include "BasicStageObject.h"
#include "FloatingStageObject.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"

int StageObjectMgr::AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== �M�~�b�N��ǉ� =====*/

	// �ӂ�ӂ퓮�������I�u�W�F�N�g��������B
	if (ObjectID == BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT) {

		objects_.emplace_back(std::make_shared<FloatingStageObject>());

	}
	// ����ȊO�̒ʏ�̃I�u�W�F�N�g��������B
	else {

		objects_.emplace_back(std::make_shared<BasicStageObject>());

	}

	// Blas�����[�h
	int blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	// instance�𐶐��B
	int instanceIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID);

	// �I�u�W�F�N�g��ݒ�B
	objects_.back()->Setting(ObjectID, CollisionID, instanceIndex);

	return static_cast<int>(objects_.size()) - 1;

}

void StageObjectMgr::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

	for (auto& index : objects_) {

		index->Update(Timer);

	}

}

void StageObjectMgr::AddTrans(const int& Index, const Vec3& Trans)
{

	/*===== �ړ������Z =====*/

	objects_[Index]->AddTrans(Trans);

}

void StageObjectMgr::ChangeTrans(const int& Index, const Vec3& Trans)
{

	/*===== �ړ����� =====*/

	objects_[Index]->ChangeTrans(Trans);

}

void StageObjectMgr::AddScale(const int& Index, const Vec3& Scale)
{

	/*===== �X�P�[�������Z =====*/

	objects_[Index]->AddScale(Scale);

}

void StageObjectMgr::ChangeScale(const int& Index, const Vec3& Scale)
{

	/*===== �X�P�[������ =====*/

	objects_[Index]->ChangeScale(Scale);

}

void StageObjectMgr::AddRotate(const int& Index, const Vec3& Rotate)
{

	/*===== ��]�����Z =====*/

	objects_[Index]->AddRotate(Rotate);

}

void StageObjectMgr::ChangeRotate(const int& Index, const Vec3& Rotate)
{

	/*===== ��]���� =====*/

	objects_[Index]->ChangeRotate(Rotate);

}
