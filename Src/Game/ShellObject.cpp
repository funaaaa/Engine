#include "ShellObject.h"
#include "OBB.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "HitGroupMgr.h"

void ShellObject::Generate(const Vec3& Pos, const Vec3& ForwardVec)
{

	/*===== 生成処理 =====*/

	// BLASとInstanceを生成。
	blasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Carapace/", "Carapace.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::HITGROUP_ID::DEF], { L"Resource/Game/blackRed.png" });
	insIndex_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);

	PolygonInstanceRegister::Ins()->AddScale(insIndex_, Vec3(30, 30, 30));

	// OBBを設定。
	obb_ = std::make_shared<OBB>();
	obb_->Setting(blasIndex_, insIndex_);

	// 変数を設定。
	pos_ = Pos;
	forwardVec_ = ForwardVec;
	speed_ = SPEED;
	isActive_ = true;

}

void ShellObject::Update()
{

	/*===== 更新処理 =====*/

	// 移動させる。
	pos_ += forwardVec_ * SPEED;

	// インスタンスも移動させる。
	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, pos_);

}
