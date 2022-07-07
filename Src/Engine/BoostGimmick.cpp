#include "BoostGimmick.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"

void BoostGimmick::Setting(const ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== セッティング処理 =====*/

	// オブジェクトをロード
	blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	insIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID);

	// 各種変数を初期化。
	gimmickID = GimmickID;
	isActive = false;

}

void BoostGimmick::Update()
{

	/*===== 更新処理 =====*/

}
