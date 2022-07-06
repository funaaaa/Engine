#include "GimmickMgr.h"

void GimmickMgr::AddGimmick(const BaseGimmick::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== ƒMƒ~ƒbƒN‚ð’Ç‰Á =====*/

	gimmicks.emplace_back(std::make_shared<BaseGimmick>());
	gimmicks[gimmicks.size() - 1]->Setting(GimmickID, DirectryPath, ModelName, TexturePath, HitGroupName, ShaderID);

}
