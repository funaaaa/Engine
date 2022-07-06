#pragma once
#include <vector>
#include <memory>
#include "BaseGimmick.h"
#include "Singleton.h"

class GimmickMgr : public Singleton<GimmickMgr> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<BaseGimmick>> gimmicks;


public:

	/*===== メンバ変数 =====*/

	// 追加する。
	void AddGimmick(const BaseGimmick::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);

	// ギミック集を追加。
	std::vector<std::shared_ptr<BaseGimmick>> GetGimmickData() { return gimmicks; }

};
