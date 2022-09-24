#include "FirstAIWayPoint.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "HitGroupMgr.h"

FirstAIWayPoint::FirstAIWayPoint()
{

	/*===== コンストラクタ =====*/

	pos_ = Vec3();

#ifdef DEBUG

	// BlackRedの階層変わってます！
	blasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Debug/", "NormalSphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/blackRed.png" });
	insIndex_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->ChangeScale(insIndex_, Vec3(RADIUS, RADIUS, RADIUS));

#endif

	isActive_ = true;

}

void FirstAIWayPoint::SetPos(Vec3 Pos)
{

	pos_ = Pos;

#ifdef DEBUG

	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, Pos);

#endif

}
