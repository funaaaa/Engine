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

	/*===== コンストラクタ =====*/

	// ロケットを生成。
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

	/*===== 初期化処理 =====*/

	rocketEasingTimer_ = 0;
	rocketRotationY_ = 0;
	rocketAddRotationY_ = 0;
	isDisplayRocket_ = false;

}

void CharacterRocket::Update(const UpdateInData& InData)
{

	/*===== 更新処理 =====*/

	// ロケットをカメラの左上に移動させる。
	Vec3 nowRocketPos = rocketIns_[0].lock()->GetPos();
	Vec3 rocketPos = InData.pos_;
	rocketPos -= InData.forwardVec_ * 60.0f;
	rocketPos -= InData.upVec_ * 20.0f;
	// 補間する。
	if (InData.isPlayer_) {
		nowRocketPos += (rocketPos - nowRocketPos) / 3.0f;
	}
	else {
		nowRocketPos += (rocketPos - nowRocketPos);
	}
	for (auto& index : rocketIns_) {

		index.lock()->ChangeTrans(nowRocketPos);

	}

	// 回転量を本来の値に補間する。
	const float ADD_ROTATION_Y = 0.8f;
	const float MIN_ROTATION_Y = 0.05f;
	rocketAddRotationY_ += (MIN_ROTATION_Y - rocketAddRotationY_) / 10.0f;
	// 回転させる。
	rocketRotationY_ += rocketAddRotationY_;

	// ロケットを斜めにする。
	for (auto& index : rocketIns_) {

		// 初期の回転に戻す。
		index.lock()->ChangeRotate(Vec3(0.4f, 0.0f, 0.0f));

		// カメラ上ベクトル方向のクォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(Camera::Ins()->up_.ConvertXMVECTOR(), rocketRotationY_);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		index.lock()->AddRotate(quaternionMat);

	}

	// アイテムを取得した瞬間や使った瞬間にイージングタイマーを初期化。
	bool prevFrameDisplayRocket = isDisplayRocket_;
	if (InData.isGetItem_ || InData.isUseItem_) {

		rocketEasingTimer_ = 0.1f;
		rocketAddRotationY_ = ADD_ROTATION_Y;

		if (!isDisplayRocket_) {
			isDisplayRocket_ = true;
		}

	}

	// アイテムのロケットのサイズを更新。
	const float MAX_SCALE = 12.0f;
	rocketEasingTimer_ += 0.06f;
	if (1.0f < rocketEasingTimer_) rocketEasingTimer_ = 1.0f;
	if (InData.hasItem_) {

		// イージング量を求める。
		float easingAmount = FEasing::EaseOutQuint(rocketEasingTimer_);
		float scale = MAX_SCALE * easingAmount;
		if (scale <= 0) scale = 0.01f;
		for (auto& index : rocketIns_) {
			index.lock()->ChangeScale(Vec3(scale, scale, scale));
		}

	}
	else {

		// イージング量を求める。
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

	// 描画始まったトリガーだったら描画する。
	if (!prevFrameDisplayRocket && isDisplayRocket_) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->Display(index.lock()->GetInstanceIndex());
		}
	}

	// 描画終わったトリガーだったら描画を消す。
	if ((prevFrameDisplayRocket && !isDisplayRocket_) || (!isDisplayRocket_ && rocketIns_[0].lock()->GetScaleVec3().x_ <= 0)) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->NonDisplay(index.lock()->GetInstanceIndex());
		}
	}

}
