#pragma once
#include <vector>
#include <memory>
#include "BaseStageObject.h"
#include "Singleton.h"

class StageObjectMgr {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<BaseStageObject>> objects_;


public:

	/*===== メンバ変数 =====*/

	// 追加する。
	int AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);

	// 更新処理
	void Update(const int& Timer);

	// ギミック集を追加。
	std::vector<std::shared_ptr<BaseStageObject>> GetGimmickData() { return objects_; }

	// 移動関係
	void AddTrans(const int& Index, const Vec3& Trans);
	void ChangeTrans(const int& Index, const Vec3& Trans);

	// サイズ関係
	void AddScale(const int& Index, const Vec3& Scale);
	void ChangeScale(const int& Index, const Vec3& Scale);

	// 回転関係
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
//	/*===== メンバ変数 =====*/
//
//	std::vector<std::shared_ptr<BaseStageObject>> gimmicks_;
//
//
//public:
//
//	/*===== メンバ変数 =====*/
//
//	// 追加する。
//	int AddGimmick(const BaseStageObject::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);
//
//	// ギミック集を追加。
//	std::vector<std::shared_ptr<BaseStageObject>> GetGimmickData() { return gimmicks_; }
//
//	// 移動関係
//	void AddTrans(const int& GimmickIndex, const Vec3& Trans);
//	void ChangeTrans(const int& GimmickIndex, const Vec3& Trans);
//
//	// サイズ関係
//	void AddScale(const int& GimmickIndex, const Vec3& Scale);
//	void ChangeScale(const int& GimmickIndex, const Vec3& Scale);
//
//	// 回転関係
//	void AddRotate(const int& GimmickIndex, const Vec3& Rotate);
//	void ChangeRotate(const int& GimmickIndex, const Vec3& Rotate);
//
//};