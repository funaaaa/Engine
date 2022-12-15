#include "ItemBoxObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "CharacterMgr.h"

void ItemBoxObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== �Z�b�e�B���O���� =====*/

	BasicInit(ObjectID, CollisionID, Instance);

	scale_ = DEF_SCALE;
	charaIndex_ = 0;

}

void ItemBoxObject::Destroy()
{

	/*===== �C���X�^���X�j�� =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);
	isActive_ = false;
	scale_ = DEF_SCALE;

}

void ItemBoxObject::Update(int Timer, std::weak_ptr<CharacterMgr> Character)
{

	/*===== �X�V���� =====*/

	Timer;

	// �I�u�W�F�N�g��������������B
	if (!isActive_) {

		// �ĕ\������܂ł̃^�C�}�[���K��l�ɒB���Ă�����ĕ\������B
		++activationTimer_;
		if (timerToActivation_ < activationTimer_) {

			isActive_ = true;

			Display();
			scale_ = 0.01f;
			instance_.lock()->ChangeScale(Vec3(scale_, scale_, scale_));

			// �������W�ɖ߂��B
			pos_ = defPos_;
			instance_.lock()->ChangeTrans(pos_);

		}
		else {

			// �I�u�W�F�N�g�̃T�C�Y������������B
			scale_ -= scale_ / 5.0f;

			// �I�u�W�F�N�g�̃T�C�Y������Ȃ�0�ɋ߂�������
			if (scale_ <= 0.01f) {

				// ��\���ɂ���B
				NonDisplay();

			}
			else {

				// �T�C�Y��ݒ�B
				instance_.lock()->ChangeScale(Vec3(scale_, scale_, scale_));

				// �A�C�e���{�b�N�X���擾�����L�����ɋ߂Â���B
				pos_ += (Character.lock()->GetPos(charaIndex_) - pos_) / 4.0f;
				instance_.lock()->ChangeTrans(pos_);

			}

		}

	}
	else {

		// �X�P�[�����K��l�ɋ߂Â���B
		scale_ += (DEF_SCALE - scale_) / 5.0f;
		instance_.lock()->ChangeScale(Vec3(scale_, scale_, scale_));

	}

	// ��]������B
	instance_.lock()->AddRotate(Vec3(0.01f, 0.02f, 0.0f));


}

void ItemBoxObject::Disable(int TimerToActivation, int CharaIndex)
{

	/*===== ���������čēx�L���������܂ł̎��Ԃ��Z�b�g =====*/

	isActive_ = false;

	pos_ = defPos_;
	timerToActivation_ = TimerToActivation;
	charaIndex_ = CharaIndex;
	activationTimer_ = 0;

}
