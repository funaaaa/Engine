#include "ShellObject.h"
#include "OBB.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "HitGroupMgr.h"

void ShellObject::Generate(const Vec3& Pos, const Vec3& ForwardVec)
{

	/*===== �������� =====*/

	// BLAS��Instance�𐶐��B
	blasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Carapace/", "Carapace.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::HITGROUP_ID::DEF], { L"Resource/Game/blackRed.png" });
	insIndex_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);

	PolygonInstanceRegister::Ins()->AddScale(insIndex_, Vec3(30, 30, 30));

	// OBB��ݒ�B
	obb_ = std::make_shared<OBB>();
	obb_->Setting(blasIndex_, insIndex_);

	// �ϐ���ݒ�B
	pos_ = Pos;
	forwardVec_ = ForwardVec;
	speed_ = SPEED;
	isActive_ = true;

}

void ShellObject::Update()
{

	/*===== �X�V���� =====*/

	// �ړ�������B
	pos_ += forwardVec_ * SPEED;

	// �C���X�^���X���ړ�������B
	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, pos_);

}
