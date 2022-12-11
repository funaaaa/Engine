#include "CharacterRocket.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"
#include "BLAS.h"
#include "PolygonInstance.h"
#include "PolygonInstanceRegister.h"
#include "TextureManager.h"
#include "Camera.h"

CharacterRocket::CharacterRocket()
{

	/*===== �R���X�g���N�^ =====*/

	// ���P�b�g�𐶐��B
	rocketBlas_[0] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketHead.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[1] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[2] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketLegs.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[3] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	for (int index = 0; index < 4; ++index) {
		rocketIns_[index] = PolygonInstanceRegister::Ins()->CreateInstance(rocketBlas_[index], PolygonInstanceRegister::DEF_EMISSIVE);
		rocketIns_[index].lock()->ChangeScale(Vec3(30, 30, 30));
		rocketBlas_[index].lock()->ChangeMapTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/metalness2.png"), BLAS::MAP_PARAM::SPECULAR);
	}

	rocketEasingTimer_ = 0;
	rocketRotationY_ = 0;
	rocketAddRotationY_ = 0;
	isDisplayRocket_ = false;

}

void CharacterRocket::Init()
{

	/*===== ���������� =====*/

	rocketEasingTimer_ = 0;
	rocketRotationY_ = 0;
	rocketAddRotationY_ = 0;
	isDisplayRocket_ = false;

}

void CharacterRocket::Update(const UpdateInData& InData)
{

	/*===== �X�V���� =====*/

	// ���P�b�g���J�����̍���Ɉړ�������B
	Vec3 nowRocketPos = rocketIns_[0].lock()->GetPos();
	Vec3 rocketPos = InData.pos_;
	rocketPos -= InData.forwardVec_ * 60.0f;
	rocketPos -= InData.upVec_ * 20.0f;
	// ��Ԃ���B
	if (InData.isPlayer_) {
		nowRocketPos += (rocketPos - nowRocketPos) / 3.0f;
	}
	else {
		nowRocketPos += (rocketPos - nowRocketPos);
	}
	for (auto& index : rocketIns_) {

		index.lock()->ChangeTrans(nowRocketPos);

	}

	// ��]�ʂ�{���̒l�ɕ�Ԃ���B
	const float ADD_ROTATION_Y = 0.8f;
	const float MIN_ROTATION_Y = 0.05f;
	rocketAddRotationY_ += (MIN_ROTATION_Y - rocketAddRotationY_) / 10.0f;
	// ��]������B
	rocketRotationY_ += rocketAddRotationY_;

	// ���P�b�g���΂߂ɂ���B
	for (auto& index : rocketIns_) {

		// �����̉�]�ɖ߂��B
		index.lock()->ChangeRotate(Vec3(0.4f, 0.0f, 0.0f));

		// �J������x�N�g�������̃N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(Camera::Ins()->up_.ConvertXMVECTOR(), rocketRotationY_);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		index.lock()->AddRotate(quaternionMat);

	}

	// �A�C�e�����擾�����u�Ԃ�g�����u�ԂɃC�[�W���O�^�C�}�[���������B
	bool prevFrameDisplayRocket = isDisplayRocket_;
	if (InData.isGetItem_ || InData.isUseItem_) {

		rocketEasingTimer_ = 0.1f;
		rocketAddRotationY_ = ADD_ROTATION_Y;

		if (!isDisplayRocket_) {
			isDisplayRocket_ = true;
		}

	}

	// �A�C�e���̃��P�b�g�̃T�C�Y���X�V�B
	const float MAX_SCALE = 12.0f;
	rocketEasingTimer_ += 0.06f;
	if (1.0f < rocketEasingTimer_) rocketEasingTimer_ = 1.0f;
	if (InData.hasItem_) {

		// �C�[�W���O�ʂ����߂�B
		float easingAmount = FEasing::EaseOutQuint(rocketEasingTimer_);
		float scale = MAX_SCALE * easingAmount;
		if (scale <= 0) scale = 0.01f;
		for (auto& index : rocketIns_) {
			index.lock()->ChangeScale(Vec3(scale, scale, scale));
		}

	}
	else {

		// �C�[�W���O�ʂ����߂�B
		float easingAmount = FEasing::EaseInQuint(rocketEasingTimer_);
		float scale = MAX_SCALE - MAX_SCALE * easingAmount;
		if (scale <= 0) scale = 0.01f;
		for (auto& index : rocketIns_) {
			index.lock()->ChangeScale(Vec3(scale, scale, scale));
		}
		if (isDisplayRocket_ && scale <= 0) {
			isDisplayRocket_ = false;
		}

	}

	// �`��n�܂����g���K�[��������`�悷��B
	if (!prevFrameDisplayRocket && isDisplayRocket_) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->Display(index.lock()->GetInstanceIndex());
		}
	}

	// �`��I������g���K�[��������`��������B
	if ((prevFrameDisplayRocket && !isDisplayRocket_) || (!isDisplayRocket_ && rocketIns_[0].lock()->GetScaleVec3().x_ <= 0)) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->NonDisplay(index.lock()->GetInstanceIndex());
		}
	}

}
