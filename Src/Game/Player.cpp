#include "Player.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"
#include "DriftParticleMgr.h"
#include "Camera.h"
#include "HitGroupMgr.h"
#include "OBB.h"
#include "CircuitStage.h"
#include "TextureManager.h"
#include "PlayerTire.h"
#include "BoostItem.h"
#include "ShellItem.h"

Player::Player()
{

	/*===== ���������� =====*/

	// �Ԃ̃��f�������[�h
	playerModel_.Load();

	// �^�C�����Z�b�g�B
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireFrameInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireFrameInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireFrameInsIndex_, true));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireInsIndex_, true));

	pos_ = PLAYER_DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(15, 15, 15);
	rotY_ = 0;
	speed_ = 0;
	gravity_ = 0;
	boostSpeed_ = 0;
	returnDefPosTimer_ = 0;
	boostTimer_ = 0;
	isDrift_ = false;
	onGround_ = true;
	onGrass_ = false;
	isTireMask_ = false;
	IsTurningIndicatorRed_ = false;
	turningIndicatorTimer_ = 0;

	// OBB�𐶐��B
	obb_ = std::make_shared<OBB>();
	obb_->Setting(playerModel_.carBodyBlasIndex_, playerModel_.carBodyInsIndex_);

}

void Player::Init()
{

	/*===== ���������� =====*/

	pos_ = PLAYER_DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(30, 30, 30);
	returnDefPosTimer_ = 0;
	rotY_ = 0;
	speed_ = 0;
	gravity_ = 0;
	boostSpeed_ = 0;
	boostTimer_ = 0;
	turningIndicatorTimer_ = 0;
	isDrift_ = false;
	onGround_ = true;
	onGrass_ = false;
	IsTurningIndicatorRed_ = false;
	isTireMask_ = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));

}

void Player::Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input(ConstBufferData);

	// �ړ�����
	Move();

	// �����蔻��
	CheckHit(StageData, IsPassedMiddlePoint, RapCount);

	// ���W���X�V�B
	PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, pos_);

	// �󒆂ɂ���Ƃ��͏����n�_�܂Ŗ߂�^�C�}�[���X�V�B�n��ɗv��Ƃ��̓^�C�}�[���������B
	if (onGround_) {

		returnDefPosTimer_ = 0;

	}
	else {

		++returnDefPosTimer_;

		if (RETURN_DEFPOS_TIMER < returnDefPosTimer_) {

			pos_ = PLAYER_DEF_POS;
			PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
			forwardVec_ = Vec3(0, 0, -1);
			rotY_ = 0;
			upVec_ = Vec3(0, 1, 0);
			returnDefPosTimer_ = 0;

		}

	}

	// OBB���X�V�B
	obb_->SetMat(playerModel_.carBodyInsIndex_);

	// ���W��ۑ��B
	prevPos_ = pos_;

	// �^�C�����X�V����B
	for (auto& index : tires_) {

		index->Update();

	}



	// �A�C�e���e�X�g�p
	if (Input::Ins()->IsKeyTrigger(DIK_P) && item_.operator bool()) {

		if (item_->GetItemID() == BaseItem::ItemID::BOOST) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 30;
			item_.reset();

		}
		else {

			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND));

		}


	}

	if (Input::Ins()->IsKeyRelease(DIK_P) && item_.operator bool()) {

		if (isShotBehind_) {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND_THROW));
		}
		else {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::FORWARD_THROW));
		}
		item_.reset();


	}


	// �u�[�X�g����^�C�}�[�����ȏゾ�����������������B
	if (0 < boostTimer_) {

		--boostTimer_;

		boostSpeed_ = MAX_BOOST_SPEED;

	}

}

void Player::Draw()
{

	/*===== �`�揈�� =====*/

}

float Player::GetNowSpeedPer()
{

	/*====== �ړ����x�̊������v�Z =====*/

	float per = (speed_ + boostSpeed_) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input(RayConstBufferData& ConstBufferData)
{

	/*===== ���͏��� =====*/

	isTireMask_ = false;

	// RT��������Ă���������B
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if ((INPUT_DEADLINE_TRI < inputRightTriValue) && onGround_) {

		speed_ += inputRightTriValue * ADD_SPEED;

	}
	else if (Input::Ins()->IsKey(DIK_W) && onGround_) {

		speed_ += ADD_SPEED;

	}
	else if (onGround_) {

		// �ړ����Ă��Ȃ��Ēn��ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 10.0f;

	}
	else {

		// �ړ����Ă��Ȃ��ċ󒆂ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 200.0f;

	}

	// O�L�[��������Ă�������ɓ�����t���O�𗧂Ă�B�@�R���g���[���[�p�̏���������X�����B
	if (Input::Ins()->IsKey(DIK_O)) {
		isShotBehind_ = true;
	}
	else {
		isShotBehind_ = false;
	}

	// ���݂̃t���[���̉E�X�e�B�b�N�̌X����B
	float nowFrameInputLeftStickHori = 0;

	// �E�X�e�B�b�N�̉��̌X���ʂŃL��������]������B
	float inputLeftStickHori = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	const float LEFT_STICK_INPUT_DEADLINE = 0.2f;
	int inputADKey = Input::Ins()->IsKey(DIK_D) - Input::Ins()->IsKey(DIK_A);
	if (LEFT_STICK_INPUT_DEADLINE < std::fabs(inputLeftStickHori) || inputADKey != 0) {

		// ��]�� �ʏ��Ԃƃh���t�g��ԂňႤ�B
		float handleAmount = HANDLE_NORMAL;

		// �h���t�g��Ԃ��������]�ʂ𑽂����ɂ���B
		if (isDrift_) {

			handleAmount = HANDLE_DRIFT;

			// ���łɃh���t�g��Ԃ̎��̃u�[�X�g����܂ł̃^�C�}�[���X�V����B
			if (onGround_) {
				++driftBoostTimer_;
				if (DRIFT_BOOST_TIMER < driftBoostTimer_) driftBoostTimer_ = DRIFT_BOOST_TIMER;
			}

			// �^�C������]������B
			for (auto& index : tires_) {

				index->Rot(true, static_cast<float>(inputADKey));
				index->Rot(true, static_cast<float>(inputLeftStickHori));

			}

			isTireMask_ = true;


		}
		// �h���t�g��Ԃ���Ȃ�������B
		else {

			// �^�C������]������B
			for (auto& index : tires_) {

				index->Rot(false, static_cast<float>(inputADKey));
				index->Rot(false, static_cast<float>(inputLeftStickHori));

			}

		}

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec_.ConvertXMVECTOR(), handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, quaternionMat);
		rotY_ += handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;
		nowFrameInputLeftStickHori = inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));


		// �E�C���J�[�̐F��ς���^�C�}�[���X�V�B
		++turningIndicatorTimer_;
		if (TURNING_INDICATOR_TIMER < turningIndicatorTimer_) {

			turningIndicatorTimer_ = 0;
			IsTurningIndicatorRed_ = IsTurningIndicatorRed_ ? false : true;

		}

		// �E�C���J�[�̐F��ς���B
		if (IsTurningIndicatorRed_) {

			// �Ȃ����Ă���̂��E��������B
			if (0 < inputADKey || 0 < inputLeftStickHori) {

				BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/blackRed.png"));
				BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

			}
			else {

				BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/blackRed.png"));
				BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

			}

		}
		else {

			BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));
			BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

		}

	}
	else {

		// �Ԃ̃��C�g�̐F�����ɖ߂��B
		BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));
		BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

		// �e�ϐ����������B
		IsTurningIndicatorRed_ = false;
		turningIndicatorTimer_ = 100;	// �`�J�`�J����^�C�}�[���������B

	}

	// LT��������Ă�����h���t�g��Ԃɂ���B
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori) || inputADKey != 0;
	bool isInputLShift = Input::Ins()->IsKey(DIK_LSHIFT);
	if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isInputNowFrameLeftStrick) {

		isDrift_ = true;

		// �h���t�g�̃x�N�g�������߂�B
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}

		// �h���t�g���̃p�[�e�B�N���𐶐��B
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PolygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);
		ConstBufferData;

	}
	// ���łɃh���t�g���������珟��ɉ������Ȃ��悤�ɂ���B
	else if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isDrift_) {
	}
	else {

		// �h���X�g�̃u�[�X�g����܂ł̃^�C�}�[���K��l�ȏゾ������u�[�X�g����B
		if (DRIFT_BOOST_TIMER <= driftBoostTimer_) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 10;

		}

		// �h���t�g�̃u�[�X�g����܂ł̃^�C�}�[������������B
		driftBoostTimer_ = 0;

		isDrift_ = false;

	}

	// �f�o�b�O�p B�{�^���������ꂽ�珉���ʒu�ɖ߂��B
	if (Input::Ins()->IsPadBottom(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_SPACE)) {

		pos_ = PLAYER_DEF_POS;
		PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
		forwardVec_ = Vec3(0, 0, -1);
		rotY_ = 0;
		upVec_ = Vec3(0, 1, 0);

	}

}

void Player::Move()
{

	/*===== �ړ����� =====*/

	// �ړ����x�����E�𒴂��Ȃ��悤�ɂ���B
	if (MAX_SPEED < speed_) {

		speed_ = MAX_SPEED;

	}

	// ���̏�ɂ�����ړ����x�̌��E�l��������B
	if (onGrass_ && MAX_SPEED_ON_GRASS < speed_) {

		speed_ = MAX_SPEED_ON_GRASS;

	}

	// ���W�ړ�������B
	pos_ += forwardVec_ * (speed_ + boostSpeed_);

	// �h���t�g���̃u�[�X�g�ړ��ʂ�0�ɋ߂Â���B
	if (0 < boostSpeed_) {

		boostSpeed_ -= SUB_BOOST_SPEED;

	}
	else {

		boostSpeed_ = 0;

	}

	// �n��ɂ�����d�͂𖳌�������B
	if (onGround_) {

		gravity_ = 0;

	}
	// �󒆂ɂ�����d�͂����Z����B
	else {

		gravity_ += ADD_GRAV;

		// �d�͗ʂ����E�𒴂��Ȃ��悤�ɂ���B
		if (MAX_GRAV < gravity_) {

			gravity_ = MAX_GRAV;

		}

	}

	// ���W�ɏd�͂����Z����B
	pos_ += Vec3(0, -1, 0) * gravity_;

	// ���x�N�g�����Ԃ̉�]�s�񕪉�]������B
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

}

void Player::CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== �����蔻�� =====*/

	// �����蔻��֐��ɓ����l��ݒ�B
	BaseStage::ColliderInput input;
	input.targetInsIndex_ = playerModel_.carBodyInsIndex_;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
	input.targetPrevPos_ = prevPos_;
	input.targetRotY_ = rotY_;
	input.targetSize_ = size_;
	input.isInvalidateRotY_ = false;
	input.isPlayer_ = true;

	// �����蔻��֐�����Ԃ��Ă���l�B
	BaseStage::ColliderOutput output;

	// �����蔻����s���B
	output = StageData.lock()->Collider(input);

	// �����蔻��̌��ʂ��珈�����s���B
	if (output.isHitBoostGimmick_) {

		// �u�[�X�g���}�b�N�X�ɂ���B
		boostSpeed_ = MAX_BOOST_SPEED;
		boostTimer_ = 10;

	}
	if (output.isHitGoal_) {

		// �S�[��
		if (IsPassedMiddlePoint) {
			IsPassedMiddlePoint = false;
			++RapCount;

		}

	}
	if (output.isHitMiddlePoint_) {

		// ���Ԓn�_�Ƃ̓����蔻��
		IsPassedMiddlePoint = true;

	}
	if (output.isHitOrnament_) {

		// �����I�u�W�F�N�g�Ƃ̓�����������
		speed_ = 0;
		boostSpeed_ = 0;

	}
	// �ݒu������������B
	onGround_ = false;
	onGrass_ = false;
	if (output.isHitStage_) {

		// �X�e�[�W�Ƃ̓����蔻��
		onGround_ = true;

		forwardVec_ = output.forwardVec_;
		upVec_ = output.upVec_;

	}
	if (output.isHitStageGrass_) {

		// �X�e�[�W�Ɠ������Ă��Ȃ�������
		if (!onGround_) {

			// ���Ƃ�����������
			onGround_ = true;
			onGrass_ = true;

			forwardVec_ = output.forwardVec_;
			upVec_ = output.upVec_;

		}

	}
	if (output.isHitItemBox_) {

		// �����_���ŃA�C�e���𐶐�����B
		int random = FHelper::GetRand(0, 1000);

		if (random % 2 == 0) {

			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInsIndex_);

		}
		else {

			item_ = std::make_shared<ShellItem>();
			item_->Generate(playerModel_.carBodyInsIndex_);

		}

	}

	// ���̑��̕ϐ����������B
	pos_ = output.resultPos_;

}

void Player::RotObliqueFloor(const Vec3& HitNormal)
{

	/*===== �΂ߏ��̉�]���� =====*/

	/*-- �O�ς����]�����擾 --*/

	// �f�t�H���g�̏�x�N�g���B
	Vec3 defUpVec = Vec3(0, 1, 0);

	// ��]�������߂�B
	Vec3 axisOfRevolution = defUpVec.Cross(HitNormal);

	// ��]���𐳋K������B
	if (axisOfRevolution.Length() != 0) {
		axisOfRevolution.Normalize();
	}

	/*-- ���ς����]�ʂ��擾 --*/

	// ��]�ʂ����߂�B
	float amountOfRotation = defUpVec.Dot(HitNormal);

	// �t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ��B
	amountOfRotation = acosf(amountOfRotation);


	/*-- �N�H�[�^�j�I�����g���ĉ�] --*/

	// ��]����{0,0,0}�������珈�����΂��B
	if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, quaternionMat);

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// ��x�N�g�����X�V�B
		upVec_ = normal_;

		//���ʃx�N�g�����X�V�B
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// ���������@�����ゾ�����ꍇ�͍s������������ĉ�]������B
	if (HitNormal == Vec3(0, 1, 0)) {

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, DirectX::XMMatrixIdentity());

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// ��x�N�g�����X�V�B
		upVec_ = normal_;

		//���ʃx�N�g�����X�V�B
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}