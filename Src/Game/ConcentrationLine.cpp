#include "ConcentrationLine.h"
#include "Sprite.h"
#include "FHelper.h"
#include "Pipline.h"

ConcentrationLine::ConcentrationLine()
{

	/*===== コンストラクタ =====*/

	sprite_ = std::make_shared<Sprite>();
	sprite_->GenerateForTexture(Vec3(0, 0, 0), Vec2(512.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::BLEND_ID::BLENDMODE_ALPHA, L"Resource/Game/concentrationLine.png");
	alpha_ = 0;
	exitTimer_ = 0;
	isActive_ = false;

}

void ConcentrationLine::Init()
{

	/*===== 初期化処理 =====*/

	alpha_ = 0;
	exitTimer_ = 0;
	isActive_ = false;

}

void ConcentrationLine::Generate(const Vec3& Pos)
{

	/*===== 初期化処理 =====*/

	alpha_ = 0.5f;
	exitTimer_ = 0;
	isActive_ = true;


	sprite_->ChangePosition(Pos);

	// 中心までの角度を求める。
	float angle = atan2f((720.0f / 2.0f) - Pos.y_, (1280.0f / 2.0f) - Pos.x_);

	// 回転させる。
	sprite_->ChangeRotation(Vec3(0, 0, angle));

	// アルファ値を適応させる。
	sprite_->SetColor(DirectX::XMFLOAT4(1, 1, 1, alpha_));

}

void ConcentrationLine::Update()
{

	/*===== 更新処理 =====*/

	++exitTimer_;
	if (EXIT_TIME <= exitTimer_) {

		Init();

	}

	// アルファ値を決める。
	float timerRate = static_cast<float>(exitTimer_) / static_cast<float>(EXIT_TIME);

	// イージングをかける。
	alpha_ = 0.5f - FEasing::EaseInCubic(timerRate) * 0.5f;

	// アルファ値を適応させる。
	sprite_->SetColor(DirectX::XMFLOAT4(1, 1, 1, alpha_));


	Vec3 dir = sprite_->GetPos() - Vec3(1280.0f / 2.0f, 720.0f / 2.0f, 0);
	dir.Normalize();
	sprite_->ChangePositionAdd(dir * 10.0f);

}

void ConcentrationLine::Draw()
{

	/*===== 描画処理 =====*/

	sprite_->Draw();

}
