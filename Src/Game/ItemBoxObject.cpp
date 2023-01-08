#include "ItemBoxObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "CharacterMgr.h"

void ItemBoxObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== セッティング処理 =====*/

	BasicInit(ObjectID, CollisionID, Instance);

	scale_ = DEF_SCALE;
	charaIndex_ = 0;

}

void ItemBoxObject::Destroy()
{

	/*===== インスタンス破棄 =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);
	isActive_ = false;
	scale_ = DEF_SCALE;

}

void ItemBoxObject::Update(int Timer, std::weak_ptr<CharacterMgr> Character)
{

	/*===== 更新処理 =====*/

	Timer;

	// オブジェクトが無効だったら。
	if (!isActive_) {

		// 再表示するまでのタイマーが規定値に達していたら再表示する。
		++activationTimer_;
		if (timerToActivation_ < activationTimer_) {

			isActive_ = true;

			Display();
			scale_ = 0.01f;
			instance_.lock()->ChangeScale(Vec3(scale_, scale_, scale_));

			// 初期座標に戻す。
			pos_ = defPos_;
			instance_.lock()->ChangeTrans(pos_);

		}
		else {

			// オブジェクトのサイズを小さくする。
			scale_ -= scale_ / 5.0f;

			// オブジェクトのサイズが限りなく0に近かったら
			if (scale_ <= 0.01f) {

				// 非表示にする。
				NonDisplay();

			}
			else {

				// サイズを設定。
				instance_.lock()->ChangeScale(Vec3(scale_, scale_, scale_));

				// アイテムボックスを取得したキャラに近づける。
				pos_ += (Character.lock()->GetPos(charaIndex_) - pos_) / 4.0f;
				instance_.lock()->ChangeTrans(pos_);

			}

		}

	}
	else {

		// スケールを規定値に近づける。
		scale_ += (DEF_SCALE - scale_) / 5.0f;
		instance_.lock()->ChangeScale(Vec3(scale_, scale_, scale_));

	}

	// 回転させる。
	instance_.lock()->AddRotate(Vec3(0.01f, 0.02f, 0.0f));


}

void ItemBoxObject::Disable(int TimerToActivation, int CharaIndex)
{

	/*===== 無効化して再度有効化されるまでの時間をセット =====*/

	isActive_ = false;

	pos_ = defPos_;
	timerToActivation_ = TimerToActivation;
	charaIndex_ = CharaIndex;
	activationTimer_ = 0;

}
