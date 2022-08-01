#include "PlayerTire.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

PlayerTire::PlayerTire(const int& TireInsIndex, const bool& IsBehindTire)
{

	/*===== �R���X�g���N�^ =====*/

	tireInsIndex_ = TireInsIndex;
	isBehindTire_ = IsBehindTire;
	pos_ = Vec3();
	prevPos_ = Vec3();
	rot_ = Vec3();

}

void PlayerTire::Update()
{

	/*===== �X�V���� =====*/

	// Y����]��0�ɋ߂Â���B
	rot_.y_ -= rot_.y_ / 10.0f;

	// ���t���[���̍��W�����߂�B
	pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(tireInsIndex_);

	// �O�t���[���Ƃ̋��������߂�B
	float distance = (pos_ - prevPos_).Length();

	// ���������]������ʂ����߂�B
	rot_.x_ -= distance / 10.0f;

	// ��]����U�������B
	PolygonInstanceRegister::Ins()->ChangeRotate(tireInsIndex_, Vec3(0, 0, 0));

	// �O�ւ�������B
	if (!isBehindTire_) {

		PolygonInstanceRegister::Ins()->ChangeRotate(tireInsIndex_, Vec3(0, rot_.y_, 0));

	}

	// �ړ��ɔ�����]��������B
	Vec3 rotVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(tireInsIndex_));

	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(rotVec.ConvertXMVECTOR(), rot_.x_);

	// �N�H�[�^�j�I�����s��Ɏ����B
	DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);
	PolygonInstanceRegister::Ins()->AddRotate(tireInsIndex_, quaternionMat);

	// ���W��ۑ��B
	prevPos_ = pos_;

}

void PlayerTire::Rot(const bool& IsDrift, const float& Rate)
{

	/*===== ��]���� =====*/

	const float DRIFT_ROT = 0.5f;
	const float NORMAL_ROT = 0.3f;

	// �h���t�g��������
	if (IsDrift) {

		rot_.y_ = DRIFT_ROT * Rate;

	}
	else {

		rot_.y_ = NORMAL_ROT * Rate;

	}

}
