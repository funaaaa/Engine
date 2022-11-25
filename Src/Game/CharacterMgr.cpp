#include "CharacterMgr.h"
#include "Character.h"

#pragma warning(push)
#pragma warning(disable:4324)

CharacterMgr::CharacterMgr()
{

	/*===== コンストラクタ =====*/

	playerIndex_ = 0;

}

void CharacterMgr::Init()
{

	/*===== 初期化 =====*/

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

	/*===== 更新処理 =====*/

	for (auto& index : character_) {

		index->Update(Stage, IsBeforeStart, IsGameFinish);

	}

	if (static_cast<int>(character_.size()) <= 0) return;

	// 当たり判定の際にレイを飛ばす位置とベクトル
	RayPosDir frontRight;
	RayPosDir frontLeft;
	RayPosDir behindRight;
	RayPosDir behindLeft;
	CalRayPosDir(0, frontRight, frontLeft, behindRight, behindLeft);

	// Index0番目の車とその他の車の当たり判定。
	auto& firstCar = character_[0];
	for (auto& index : character_) {

		// Index0番目の車だった場合は処理を飛ばす。
		if (character_[0] == index) continue;

		// 一定距離内にいたら当たり判定を行う。
		float distance = Vec3(firstCar->GetPos() - index->GetPos()).Length();

		const float RANGE_IGNORE_CHECK_HIT = 150.0f;	// 当たり判定を棄却する距離。
		if (RANGE_IGNORE_CHECK_HIT <= distance) continue;

		// メッシュの当たり判定用構造体。
		FHelper::RayToModelCollisionData collisionData;

		// 右正面
		{

			// 当たり判定用データを入力。
			collisionData.rayPos_ = frontRight.pos_;
			collisionData.rayDir_ = frontRight.dir_;
			collisionData.targetPolygonData_ = index->hitBox_.lock()->GetMeshCollisionData();

			CheckHitCar(collisionData, index);

		}

		// 左正面
		{

		}

		// 右背面
		{

		}

		// 左背面
		{

		}

	}

}

void CharacterMgr::Draw()
{
}

void CharacterMgr::AddChara(int CharaID, bool IsPlayer, int Param)
{

	/*===== キャラクターを追加 =====*/

	character_.emplace_back(std::make_shared<Character>(static_cast<Character::CHARA_ID>(CharaID), static_cast<int>(character_.size()), Param));

	if (IsPlayer) playerIndex_ = static_cast<int>(character_.size()) - 1;

}

bool CharacterMgr::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, std::vector<Character::TireMaskUV>& TireMaskUVData)
{

	/*===== タイヤ痕チェック =====*/

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

	/*===== 指定の車のレイの射出地点と方向を求める =====*/

	// 範囲外参照チェック。
	const int INDEX_SIZE = static_cast<int>(character_.size());
	if (Index < 0 || INDEX_SIZE <= Index) assert(0);

	// 指定のインデックスのキャラの回転行列
	DirectX::XMMATRIX charaMatRot = character_[Index]->playerModel_.carBodyInstance_.lock()->GetRotate();

	// 各ワールド方向ベクトル。
	Vec3 worldFrontVec = character_[Index]->forwardVec_;
	Vec3 worldRightVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), charaMatRot);	// メモ : -xがデフォルト右ベクトル
	Vec3 worldUpVec = character_[Index]->upVec_;
	Vec3 worldPos = character_[Index]->GetPos();

	// 右正面の座標と方向を求める。
	{

		FrontRight.pos_ = worldPos;
		FrontRight.dir_ = FHelper::MulRotationMatNormal(Vec3(worldFrontVec + worldRightVec).GetNormal(), charaMatRot);

	}

	// 左正面の座標と方向を求める。
	{

		FrontLeft.pos_ = FrontRight.pos_;
		FrontLeft.dir_ = FHelper::MulRotationMatNormal(Vec3(worldFrontVec - worldRightVec).GetNormal(), charaMatRot);

	}

	// 右背面の座標と方向を求める。
	{

		BehindRight.pos_ = FrontRight.pos_;
		BehindRight.dir_ = FHelper::MulRotationMatNormal(Vec3(-worldFrontVec + worldRightVec).GetNormal(), charaMatRot);

	}

	// 右背面の座標と方向を求める。
	{

		BehindRight.pos_ = FrontRight.pos_;
		BehindRight.dir_ = FHelper::MulRotationMatNormal(Vec3(-worldFrontVec - worldRightVec).GetNormal(), charaMatRot);

	}

}

void CharacterMgr::CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter)
{

	/*===== 車との当たり判定 =====*/

	// 当たり判定結果受け取り用変数
	Vec3 impactPos;
	Vec3 hitNormal;
	Vec2 hitUV;
	float impactLength;

	// 当たり判定を行う。
	bool isHit = FHelper::RayToModelCollision(CollisionData, impactPos, impactLength, hitNormal, hitUV);

	// 衝突していなかったら処理を飛ばす。
	if (!isHit) return;

	// 当たり判定を許容する長さを決める。レイはオブジェクトの中心からローカルXZ平面上で斜めに飛ばす。のでYの値を0にする。
	Vec3 worldScale = IndexCharacter.lock()->size_;
	worldScale.y_ = 0.0f;
	const float MAX_RAY_LENGTH = worldScale.Length();

	// 衝突点までの長さが0より下 or レイの最大の長さより大きかったら処理を飛ばす。
	if (impactLength < 0 || MAX_RAY_LENGTH < impactLength) return;

	// 押し戻す。
	IndexCharacter.lock()->pos_ -= hitNormal * impactLength;

}
