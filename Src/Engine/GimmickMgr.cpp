#include "GimmickMgr.h"
#include "BoostGimmick.h"
#include "BLASRegister.h"

void GimmickMgr::AddGimmick(const BaseGimmick::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== ギミックを追加 =====*/

	// IDがブーストだったら
	if (GimmickID == BaseGimmick::ID::BOOST) {
		gimmicks.emplace_back(std::make_shared<BoostGimmick>());
	}

	// Blasをロード
	int blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);

	gimmicks.back()->Setting(GimmickID, blasIndex, ShaderID);

}
