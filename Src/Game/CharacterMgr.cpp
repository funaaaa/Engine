#include "CharacterMgr.h"
#include "Character.h"

#pragma warning(push)
#pragma warning(disable:4324)

CharacterMgr::CharacterMgr()
{

	/*===== �R���X�g���N�^ =====*/

	playerIndex_ = 0;

}

void CharacterMgr::Init()
{

	/*===== ������ =====*/

	playerIndex_ = 0;

	for (auto& index : character_) {

		index->DeleteInstance();

	}

	character_.resize(0);

}

#include "OBB.h"
#include "imgui.h"
#include "FHelper.h"
#include "PolygonInstance.h"
void CharacterMgr::Update(std::weak_ptr<BaseStage> Stage, bool IsBeforeStart, bool IsGameFinish)
{

	/*===== �X�V���� =====*/

	for (auto& index : character_) {

		index->Update(Stage, IsBeforeStart, IsGameFinish);

	}

	if (static_cast<int>(character_.size()) <= 0) return;

	// �����蔻��̍ۂɃ��C���΂��ʒu�ƃx�N�g��
	RayPosDir frontRight;
	RayPosDir frontLeft;
	RayPosDir behindRight;
	RayPosDir behindLeft;
	CalRayPosDir(0, frontRight, frontLeft, behindRight, behindLeft);

	// Index0�Ԗڂ̎ԂƂ��̑��̎Ԃ̓����蔻��B
	auto& firstCar = character_[0];
	for (auto& index : character_) {

		// Index0�Ԗڂ̎Ԃ������ꍇ�͏������΂��B
		if (character_[0] == index) continue;

		// ��苗�����ɂ����瓖���蔻����s���B
		float distance = Vec3(firstCar->GetPos() - index->GetPos()).Length();

		const float RANGE_IGNORE_CHECK_HIT = 150.0f;	// �����蔻������p���鋗���B
		if (RANGE_IGNORE_CHECK_HIT <= distance) continue;

		// ���b�V���̓����蔻��p�\���́B
		FHelper::RayToModelCollisionData collisionData;

		// �E����
		{

			// �����蔻��p�f�[�^����́B
			collisionData.rayPos_ = frontRight.pos_;
			collisionData.rayDir_ = frontRight.dir_;
			collisionData.targetPolygonData_ = index->hitBox_.lock()->GetMeshCollisionData();

			CheckHitCar(collisionData, index);

		}

		// ������
		{

		}

		// �E�w��
		{

		}

		// ���w��
		{

		}

	}

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::AddChara(int CharaID, bool IsPlayer, int Param)
{

	/*===== �L�����N�^�[��ǉ� =====*/

	character_.emplace_back(std::make_shared<Character>(static_cast<Character::CHARA_ID>(CharaID), static_cast<int>(character_.size()), Param));

	if (IsPlayer) playerIndex_ = static_cast<int>(character_.size()) - 1;

}

bool CharacterMgr::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<Character::TireMaskUV>& TireMaskUVData)
{

	/*===== �^�C�����`�F�b�N =====*/

	bool isTireMask = false;

	for (auto& index : character_) {

		Character::TireMaskUV tireMaskUVBuff;

		isTireMask |= index->CheckTireMask(BaseStageData, tireMaskUVBuff);

		TireMaskUVData.emplace_back(tireMaskUVBuff);

	}

	return isTireMask;
}

bool CharacterMgr::CheckGoal()
{

	bool isGoal = false;

	for (auto& index : character_) {

		if (index->GetRapCount() < 3) continue;

		isGoal = true;

		break;

	}

	return isGoal;
}

#include "PolygonInstance.h"
void CharacterMgr::CalRayPosDir(int Index, RayPosDir& FrontRight, RayPosDir& FrontLeft, RayPosDir& BehindRight, RayPosDir& BehindLeft)
{

	/*===== �w��̎Ԃ̃��C�̎ˏo�n�_�ƕ��������߂� =====*/

	// �͈͊O�Q�ƃ`�F�b�N�B
	const int INDEX_SIZE = static_cast<int>(character_.size());
	if (Index < 0 || INDEX_SIZE <= Index) assert(0);

	// �w��̃C���f�b�N�X�̃L�����̉�]�s��
	DirectX::XMMATRIX charaMatRot = character_[Index]->playerModel_.carBodyInstance_.lock()->GetRotate();

	// �e���[���h�����x�N�g���B
	Vec3 worldFrontVec = character_[Index]->forwardVec_;
	Vec3 worldRightVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), charaMatRot);	// ���� : -x���f�t�H���g�E�x�N�g��
	Vec3 worldUpVec = character_[Index]->upVec_;
	Vec3 worldPos = character_[Index]->GetPos();

	// �E���ʂ̍��W�ƕ��������߂�B
	{

		FrontRight.pos_ = worldPos;
		FrontRight.dir_ = FHelper::MulRotationMatNormal(Vec3(worldFrontVec + worldRightVec).GetNormal(), charaMatRot);

	}

	// �����ʂ̍��W�ƕ��������߂�B
	{

		FrontLeft.pos_ = FrontRight.pos_;
		FrontLeft.dir_ = FHelper::MulRotationMatNormal(Vec3(worldFrontVec - worldRightVec).GetNormal(), charaMatRot);

	}

	// �E�w�ʂ̍��W�ƕ��������߂�B
	{

		BehindRight.pos_ = FrontRight.pos_;
		BehindRight.dir_ = FHelper::MulRotationMatNormal(Vec3(-worldFrontVec + worldRightVec).GetNormal(), charaMatRot);

	}

	// �E�w�ʂ̍��W�ƕ��������߂�B
	{

		BehindRight.pos_ = FrontRight.pos_;
		BehindRight.dir_ = FHelper::MulRotationMatNormal(Vec3(-worldFrontVec - worldRightVec).GetNormal(), charaMatRot);

	}

}

void CharacterMgr::CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter)
{

	/*===== �ԂƂ̓����蔻�� =====*/

	// �����蔻�茋�ʎ󂯎��p�ϐ�
	Vec3 impactPos;
	Vec3 hitNormal;
	Vec2 hitUV;
	float impactLength;

	// �����蔻����s���B
	bool isHit = FHelper::RayToModelCollision(CollisionData, impactPos, impactLength, hitNormal, hitUV);

	// �Փ˂��Ă��Ȃ������珈�����΂��B
	if (!isHit) return;

	// �����蔻������e���钷�������߂�B���C�̓I�u�W�F�N�g�̒��S���烍�[�J��XZ���ʏ�Ŏ΂߂ɔ�΂��B�̂�Y�̒l��0�ɂ���B
	Vec3 worldScale = IndexCharacter.lock()->size_;
	worldScale.y_ = 0.0f;
	const float MAX_RAY_LENGTH = worldScale.Length();

	// �Փ˓_�܂ł̒�����0��艺 or ���C�̍ő�̒������傫�������珈�����΂��B
	if (impactLength < 0 || MAX_RAY_LENGTH < impactLength) return;

	// �����߂��B
	IndexCharacter.lock()->pos_ -= hitNormal * impactLength;

}
