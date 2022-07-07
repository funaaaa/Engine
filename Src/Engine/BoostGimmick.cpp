#include "BoostGimmick.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"

void BoostGimmick::Setting(const ID& GimmickID, const UINT& InstanceID, const UINT& ShaderID)
{

	/*===== セッティング処理 =====*/

	// オブジェクトをロード
	insIndex = InstanceID;
	blasIndex = PolygonInstanceRegister::Ins()->GetBLASIndex(InstanceID);

	// 各種変数を初期化。
	gimmickID = GimmickID;
	isActive = false;

}

void BoostGimmick::Update()
{

	/*===== 更新処理 =====*/

}
