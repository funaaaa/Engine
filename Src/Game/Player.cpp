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

	/*===== 初期化処理 =====*/

	// 車のモデルをロード
	playerModel_.Load();

	// タイヤをセット。
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

	// OBBを生成。
	obb_ = std::make_shared<OBB>();
	obb_->Setting(playerModel_.carBodyBlasIndex_, playerModel_.carBodyInsIndex_);

}

void Player::Init()
{

	/*===== 初期化処理 =====*/

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

	/*===== 更新処理 =====*/

	// 入力処理
	Input(ConstBufferData);

	// 移動処理
	Move();

	// 当たり判定
	CheckHit(StageData, IsPassedMiddlePoint, RapCount);

	// 座標を更新。
	PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, pos_);

	// 空中にいるときは初期地点まで戻るタイマーを更新。地上に要るときはタイマーを初期化。
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

	// OBBを更新。
	obb_->SetMat(playerModel_.carBodyInsIndex_);

	// 座標を保存。
	prevPos_ = pos_;

	// タイヤを更新する。
	for (auto& index : tires_) {

		index->Update();

	}



	// アイテムテスト用
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


	// ブーストするタイマーが一定以上だったら加速し続ける。
	if (0 < boostTimer_) {

		--boostTimer_;

		boostSpeed_ = MAX_BOOST_SPEED;

	}

}

void Player::Draw()
{

	/*===== 描画処理 =====*/

}

float Player::GetNowSpeedPer()
{

	/*====== 移動速度の割合を計算 =====*/

	float per = (speed_ + boostSpeed_) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input(RayConstBufferData& ConstBufferData)
{

	/*===== 入力処理 =====*/

	isTireMask_ = false;

	// RTが引かれていたら加速。
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if ((INPUT_DEADLINE_TRI < inputRightTriValue) && onGround_) {

		speed_ += inputRightTriValue * ADD_SPEED;

	}
	else if (Input::Ins()->IsKey(DIK_W) && onGround_) {

		speed_ += ADD_SPEED;

	}
	else if (onGround_) {

		// 移動していなくて地上にいたら移動量を0に近づける。
		speed_ -= speed_ / 10.0f;

	}
	else {

		// 移動していなくて空中にいたら移動量を0に近づける。
		speed_ -= speed_ / 200.0f;

	}

	// Oキーが押されていたら後ろに投げるフラグを立てる。　コントローラー用の条件式も後々入れる。
	if (Input::Ins()->IsKey(DIK_O)) {
		isShotBehind_ = true;
	}
	else {
		isShotBehind_ = false;
	}

	// 現在のフレームの右スティックの傾き具合。
	float nowFrameInputLeftStickHori = 0;

	// 右スティックの横の傾き量でキャラを回転させる。
	float inputLeftStickHori = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	const float LEFT_STICK_INPUT_DEADLINE = 0.2f;
	int inputADKey = Input::Ins()->IsKey(DIK_D) - Input::Ins()->IsKey(DIK_A);
	if (LEFT_STICK_INPUT_DEADLINE < std::fabs(inputLeftStickHori) || inputADKey != 0) {

		// 回転量 通常状態とドリフト状態で違う。
		float handleAmount = HANDLE_NORMAL;

		// ドリフト状態だったら回転量を多い方にする。
		if (isDrift_) {

			handleAmount = HANDLE_DRIFT;

			// ついでにドリフト状態の時のブーストするまでのタイマーを更新する。
			if (onGround_) {
				++driftBoostTimer_;
				if (DRIFT_BOOST_TIMER < driftBoostTimer_) driftBoostTimer_ = DRIFT_BOOST_TIMER;
			}

			// タイヤを回転させる。
			for (auto& index : tires_) {

				index->Rot(true, static_cast<float>(inputADKey));
				index->Rot(true, static_cast<float>(inputLeftStickHori));

			}

			isTireMask_ = true;


		}
		// ドリフト状態じゃなかったら。
		else {

			// タイヤを回転させる。
			for (auto& index : tires_) {

				index->Rot(false, static_cast<float>(inputADKey));
				index->Rot(false, static_cast<float>(inputLeftStickHori));

			}

		}

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec_.ConvertXMVECTOR(), handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, quaternionMat);
		rotY_ += handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;
		nowFrameInputLeftStickHori = inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;

		// 正面ベクトルを車の回転行列分回転させる。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));


		// ウインカーの色を変えるタイマーを更新。
		++turningIndicatorTimer_;
		if (TURNING_INDICATOR_TIMER < turningIndicatorTimer_) {

			turningIndicatorTimer_ = 0;
			IsTurningIndicatorRed_ = IsTurningIndicatorRed_ ? false : true;

		}

		// ウインカーの色を変える。
		if (IsTurningIndicatorRed_) {

			// 曲がっているのが右だったら。
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

		// 車のライトの色を元に戻す。
		BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));
		BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

		// 各変数を初期化。
		IsTurningIndicatorRed_ = false;
		turningIndicatorTimer_ = 100;	// チカチカするタイマーを初期化。

	}

	// LTが引かれていたらドリフト状態にする。
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori) || inputADKey != 0;
	bool isInputLShift = Input::Ins()->IsKey(DIK_LSHIFT);
	if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isInputNowFrameLeftStrick) {

		isDrift_ = true;

		// ドリフトのベクトルを求める。
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}

		// ドリフト時のパーティクルを生成。
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PolygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);
		ConstBufferData;

	}
	// すでにドリフト中だったら勝手に解除しないようにする。
	else if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isDrift_) {
	}
	else {

		// ドリストのブーストするまでのタイマーが規定値以上だったらブーストする。
		if (DRIFT_BOOST_TIMER <= driftBoostTimer_) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 10;

		}

		// ドリフトのブーストするまでのタイマーを初期化する。
		driftBoostTimer_ = 0;

		isDrift_ = false;

	}

	// デバッグ用 Bボタンが押されたら初期位置に戻す。
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

	/*===== 移動処理 =====*/

	// 移動速度が限界を超えないようにする。
	if (MAX_SPEED < speed_) {

		speed_ = MAX_SPEED;

	}

	// 草の上にいたら移動速度の限界値を下げる。
	if (onGrass_ && MAX_SPEED_ON_GRASS < speed_) {

		speed_ = MAX_SPEED_ON_GRASS;

	}

	// 座標移動させる。
	pos_ += forwardVec_ * (speed_ + boostSpeed_);

	// ドリフト時のブースト移動量を0に近づける。
	if (0 < boostSpeed_) {

		boostSpeed_ -= SUB_BOOST_SPEED;

	}
	else {

		boostSpeed_ = 0;

	}

	// 地上にいたら重力を無効化する。
	if (onGround_) {

		gravity_ = 0;

	}
	// 空中にいたら重力を加算する。
	else {

		gravity_ += ADD_GRAV;

		// 重力量が限界を超えないようにする。
		if (MAX_GRAV < gravity_) {

			gravity_ = MAX_GRAV;

		}

	}

	// 座標に重力を加算する。
	pos_ += Vec3(0, -1, 0) * gravity_;

	// 下ベクトルを車の回転行列分回転させる。
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

}

void Player::CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== 当たり判定 =====*/

	// 当たり判定関数に入れる値を設定。
	BaseStage::ColliderInput input;
	input.targetInsIndex_ = playerModel_.carBodyInsIndex_;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
	input.targetPrevPos_ = prevPos_;
	input.targetRotY_ = rotY_;
	input.targetSize_ = size_;
	input.isInvalidateRotY_ = false;
	input.isPlayer_ = true;

	// 当たり判定関数から返ってくる値。
	BaseStage::ColliderOutput output;

	// 当たり判定を行う。
	output = StageData.lock()->Collider(input);

	// 当たり判定の結果から処理を行う。
	if (output.isHitBoostGimmick_) {

		// ブーストをマックスにする。
		boostSpeed_ = MAX_BOOST_SPEED;
		boostTimer_ = 10;

	}
	if (output.isHitGoal_) {

		// ゴール
		if (IsPassedMiddlePoint) {
			IsPassedMiddlePoint = false;
			++RapCount;

		}

	}
	if (output.isHitMiddlePoint_) {

		// 中間地点との当たり判定
		IsPassedMiddlePoint = true;

	}
	if (output.isHitOrnament_) {

		// 装飾オブジェクトとの当たった判定
		speed_ = 0;
		boostSpeed_ = 0;

	}
	// 設置判定を初期化。
	onGround_ = false;
	onGrass_ = false;
	if (output.isHitStage_) {

		// ステージとの当たり判定
		onGround_ = true;

		forwardVec_ = output.forwardVec_;
		upVec_ = output.upVec_;

	}
	if (output.isHitStageGrass_) {

		// ステージと当たっていなかったら
		if (!onGround_) {

			// 草とあたった判定
			onGround_ = true;
			onGrass_ = true;

			forwardVec_ = output.forwardVec_;
			upVec_ = output.upVec_;

		}

	}
	if (output.isHitItemBox_) {

		// ランダムでアイテムを生成する。
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

	// その他の変数を初期化。
	pos_ = output.resultPos_;

}

void Player::RotObliqueFloor(const Vec3& HitNormal)
{

	/*===== 斜め床の回転処理 =====*/

	/*-- 外積から回転軸を取得 --*/

	// デフォルトの上ベクトル。
	Vec3 defUpVec = Vec3(0, 1, 0);

	// 回転軸を求める。
	Vec3 axisOfRevolution = defUpVec.Cross(HitNormal);

	// 回転軸を正規化する。
	if (axisOfRevolution.Length() != 0) {
		axisOfRevolution.Normalize();
	}

	/*-- 内積から回転量を取得 --*/

	// 回転量を求める。
	float amountOfRotation = defUpVec.Dot(HitNormal);

	// 逆余弦を求める関数を使用して求めたcosθをラジアンに変換。
	amountOfRotation = acosf(amountOfRotation);


	/*-- クォータニオンを使って回転 --*/

	// 回転軸が{0,0,0}だったら処理を飛ばす。
	if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, quaternionMat);

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// 上ベクトルを更新。
		upVec_ = normal_;

		//正面ベクトルを更新。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// 当たった法線が上だった場合は行列を初期化して回転させる。
	if (HitNormal == Vec3(0, 1, 0)) {

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, DirectX::XMMatrixIdentity());

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// 上ベクトルを更新。
		upVec_ = normal_;

		//正面ベクトルを更新。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}