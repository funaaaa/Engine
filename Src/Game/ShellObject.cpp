#include "ShellObject.h"
#include "OBB.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "BaseStage.h"
#include "FHelper.h"
#include "PolygonInstance.h"

ShellObject::ShellObject()
{

	/*===== コンストラクタ =====*/

	isDestroyed_ = true;
	isActive_ = false;

}

void ShellObject::Destroy()
{

	/*===== 破棄処理 =====*/

	isActive_ = false;

	if (isDestroyed_) return;

	PolygonInstanceRegister::Ins()->DestroyInstance(instance);
	isDestroyed_ = true;


}

void ShellObject::Generate(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, std::weak_ptr<PolygonMeshInstance> CharaInstance)
{

	/*===== 生成処理 =====*/

	// BLASとInstanceを生成。
	blas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Item/Shell/", "shell.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::HITGROUP_ID::DEF]);
	instance = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::DEF);

	size_ = Vec3(1.0f, 1.0f, 1.0f);
	instance.lock()->ChangeScale(size_);

	// OBBを設定。
	obb_ = std::make_shared<OBB>();
	obb_->Setting(blas_, instance);

	// 変数を設定。
	pos_ = Pos;
	prevPos_ = Pos;
	forwardVec_ = ForwardVec;
	rotY_ = CharaRotY;
	charaInstance = CharaInstance;
	shellID_ = static_cast<SHELL_ID>(ShellID);
	exitTimer_ = 0;
	speed_ = SPEED;
	isActive_ = true;
	isDestroyed_ = false;

}

void ShellObject::CheckHit(std::weak_ptr<BaseStage> StageData)
{

	/*===== 当たり判定 =====*/

	// オブジェクトの状態がTHROW以外だったら当たり判定を行わない。
	if (shellID_ == SHELL_ID::BEHIND) return;

	// 当たり判定関数に入れる値を設定。
	BaseStage::ColliderInput input;
	input.targetInstance_ = instance;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
	input.targetPrevPos_ = prevPos_;
	input.targetRotY_ = rotY_;
	input.targetSize_ = size_;
	input.isInvalidateRotY_ = false;
	input.isPlayer_ = false;

	// 当たり判定関数から返ってくる値。
	BaseStage::ColliderOutput output;

	// 当たり判定を行う。
	output = StageData.lock()->Collider(input);

	// 当たり判定の結果から処理を行う。
	if (output.isHitOrnament_) {

		// 装飾オブジェクトとの当たった判定
		speed_ = 0;

	}

	// 設置判定を初期化。
	onGround_ = false;
	if (output.isHitStage_) {

		// ステージとの当たり判定
		onGround_ = true;

		forwardVec_ = output.forwardVec_;

		// 後ろに飛ばしている状態だったらベクトルを逆にする。
		if (shellID_ == SHELL_ID::BEHIND_THROW) {

			forwardVec_ *= Vec3(-1, -1, -1);

		}

	}
	if (output.isHitStageGrass_) {

		// ステージとの当たり判定
		onGround_ = true;

		forwardVec_ = output.forwardVec_;

		// 後ろに飛ばしている状態だったらベクトルを逆にする。
		if (shellID_ == SHELL_ID::BEHIND_THROW) {

			forwardVec_ *= Vec3(-1, -1, -1);

		}

	}

	// 装飾オブジェクトと衝突していたら。
	if (output.isHitOrnament_) {

		// ベクトルを反射させる。
		Vec3 reflectVec = forwardVec_.Reflection(output.ornamentHitNormal_);

		// ベクトル間の角度を求める。
		float vecDot = forwardVec_.Dot(reflectVec);
		vecDot = acosf(vecDot);

		// 左右判定用
		Vec3 cross = forwardVec_.Cross(reflectVec);

		// RotYを回転させる。
		rotY_ += vecDot * (cross.y_ < 0 ? -1.0f : 1.0f);

		// 反射ベクトルを代入
		forwardVec_ = reflectVec;

	}

	// その他の変数を初期化。
	pos_ = output.resultPos_;

}

void ShellObject::Update(std::weak_ptr<BaseStage> StageData)
{

	/*===== 更新処理 =====*/

	// オブジェクトが投げられている状態だったら。
	if (shellID_ == SHELL_ID::BEHIND_THROW || shellID_ == SHELL_ID::FORWARD_THROW) {

		// 移動させる。
		pos_ += forwardVec_ * SPEED;

		// 重力の処理
		if (!onGround_) {

			// 重力を更新
			gravity_ += ADD_GRAV;
			if (MAX_GRAV < gravity_) {

				gravity_ = MAX_GRAV;

			}

			// 消滅までの時間を更新。
			++exitTimer_;
			if (EXIT_TIMER < exitTimer_) {

				isActive_ = false;

			}

		}
		else {

			gravity_ = 0;
			exitTimer_ = 0;

		}

		// 重力を加算。
		pos_.y_ -= gravity_;

		// 当たり判定を行う。
		CheckHit(StageData);

	}
	// オブジェクトが後ろに保持されている状態だったら。
	else if (shellID_ == SHELL_ID::BEHIND) {

		// 保持しているキャラの座標。
		Vec3 charaPos = charaInstance.lock()->GetPos();

		// 保持しているキャラの後ろベクトル
		Vec3 charaBehindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), charaInstance.lock()->GetRotate());

		// キャラと甲羅の距離
		static const float CHARA_SHELL_DISTANCE = 70.0f;

		// 甲羅を移動させる。
		pos_ = charaPos + charaBehindVec * CHARA_SHELL_DISTANCE;

		// 保持しているキャラのしたベクトル
		Vec3 underVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), charaInstance.lock()->GetRotate());

		// 位置調整用変ぅう
		static const float UNDER_DISTANCCE = 10.0f;

		pos_ += underVec * UNDER_DISTANCCE;

		// インスタンスを回転させる。
		instance.lock()->ChangeRotate(charaInstance.lock()->GetRotate());


	}

	// インスタンスも移動させる。
	instance.lock()->ChangeTrans(pos_);

	// OBBを追従させる。
	obb_->SetMat(instance);

	// 座標を保存
	prevPos_ = pos_;

	// 大きさをデフォルトに近づける。
	size_.x_ += (SCALE - size_.x_) / 3.0f;
	size_.y_ += (SCALE - size_.y_) / 3.0f;
	size_.z_ += (SCALE - size_.z_) / 3.0f;
	instance.lock()->ChangeScale(size_);

}

void ShellObject::ChangeStatus(const Vec3& ForwardVec, const int& ShellID)
{

	/*===== ステータスを更新 ======*/

	forwardVec_ = ForwardVec;
	shellID_ = static_cast<SHELL_ID>(ShellID);

}

bool ShellObject::CheckHitOBB(std::weak_ptr<OBB> CharaOBB)
{

	if (shellID_ == SHELL_ID::BEHIND) {

		return false;

	}

	return obb_->CheckHitOBB(CharaOBB);
}
