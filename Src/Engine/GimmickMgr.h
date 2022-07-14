#pragma once
#include <vector>
#include <memory>
#include "BaseGimmick.h"
#include "Singleton.h"

class GimmickMgr : public Singleton<GimmickMgr> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<BaseGimmick>> gimmicks_;


public:

	/*===== メンバ変数 =====*/

	// 追加する。
	int AddGimmick(const BaseGimmick::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);

	// ギミック集を追加。
	std::vector<std::shared_ptr<BaseGimmick>> GetGimmickData() { return gimmicks_; }

	// 移動関係
	void AddTrans(const int& GimmickIndex, const Vec3& Trans);
	void ChangeTrans(const int& GimmickIndex, const Vec3& Trans);

	// サイズ関係
	void AddScale(const int& GimmickIndex, const Vec3& Scale);
	void ChangeScale(const int& GimmickIndex, const Vec3& Scale);

	// 回転関係
	void AddRotate(const int& GimmickIndex, const Vec3& Rotate);
	void ChangeRotate(const int& GimmickIndex, const Vec3& Rotate);

};
