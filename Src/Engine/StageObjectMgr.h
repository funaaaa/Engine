#pragma once
#include <vector>
#include <memory>
#include "BaseStageObject.h"
#include "Singleton.h"

class StageObjectMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<BaseStageObject>> objects_;


public:

	/*===== �����o�ϐ� =====*/

	// �ǉ�����B
	int AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);

	// �X�V����
	void Update(const int& Timer);

	// �M�~�b�N�W��ǉ��B
	std::vector<std::shared_ptr<BaseStageObject>> GetGimmickData() { return objects_; }

	// �ړ��֌W
	void AddTrans(const int& Index, const Vec3& Trans);
	void ChangeTrans(const int& Index, const Vec3& Trans);

	// �T�C�Y�֌W
	void AddScale(const int& Index, const Vec3& Scale);
	void ChangeScale(const int& Index, const Vec3& Scale);

	// ��]�֌W
	void AddRotate(const int& Index, const Vec3& Rotate);
	void ChangeRotate(const int& Index, const Vec3& Rotate);

};



//#pragma once
//#include <vector>
//#include <memory>
//#include "BaseStageObject.h"
//#include "Singleton.h"
//
//class GimmickMgr : public Singleton<GimmickMgr> {
//
//private:
//
//	/*===== �����o�ϐ� =====*/
//
//	std::vector<std::shared_ptr<BaseStageObject>> gimmicks_;
//
//
//public:
//
//	/*===== �����o�ϐ� =====*/
//
//	// �ǉ�����B
//	int AddGimmick(const BaseStageObject::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);
//
//	// �M�~�b�N�W��ǉ��B
//	std::vector<std::shared_ptr<BaseStageObject>> GetGimmickData() { return gimmicks_; }
//
//	// �ړ��֌W
//	void AddTrans(const int& GimmickIndex, const Vec3& Trans);
//	void ChangeTrans(const int& GimmickIndex, const Vec3& Trans);
//
//	// �T�C�Y�֌W
//	void AddScale(const int& GimmickIndex, const Vec3& Scale);
//	void ChangeScale(const int& GimmickIndex, const Vec3& Scale);
//
//	// ��]�֌W
//	void AddRotate(const int& GimmickIndex, const Vec3& Rotate);
//	void ChangeRotate(const int& GimmickIndex, const Vec3& Rotate);
//
//};