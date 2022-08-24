#include "ConcentrationLine.h"
#include "Sprite.h"
#include "FHelper.h"
#include "Pipline.h"

ConcentrationLine::ConcentrationLine()
{

	/*===== �R���X�g���N�^ =====*/

	sprite_ = std::make_shared<Sprite>();
	sprite_->GenerateForTexture(Vec3(0, 0, 0), Vec2(512.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::BLEND_ID::BLENDMODE_ALPHA, L"Resource/Game/concentrationLine.png");
	alpha_ = 0;
	exitTimer_ = 0;
	isActive_ = false;

}

void ConcentrationLine::Init()
{

	/*===== ���������� =====*/

	alpha_ = 0;
	exitTimer_ = 0;
	isActive_ = false;

}

void ConcentrationLine::Generate(const Vec3& Pos)
{

	/*===== ���������� =====*/

	alpha_ = 0.5f;
	exitTimer_ = 0;
	isActive_ = true;


	sprite_->ChangePosition(Pos);

	// ���S�܂ł̊p�x�����߂�B
	float angle = atan2f((720.0f / 2.0f) - Pos.y_, (1280.0f / 2.0f) - Pos.x_);

	// ��]������B
	sprite_->ChangeRotation(Vec3(0, 0, angle));

	// �A���t�@�l��K��������B
	sprite_->SetColor(DirectX::XMFLOAT4(1, 1, 1, alpha_));

}

void ConcentrationLine::Update()
{

	/*===== �X�V���� =====*/

	++exitTimer_;
	if (EXIT_TIME <= exitTimer_) {

		Init();

	}

	// �A���t�@�l�����߂�B
	float timerRate = static_cast<float>(exitTimer_) / static_cast<float>(EXIT_TIME);

	// �C�[�W���O��������B
	alpha_ = 0.5f - FEasing::EaseInCubic(timerRate) * 0.5f;

	// �A���t�@�l��K��������B
	sprite_->SetColor(DirectX::XMFLOAT4(1, 1, 1, alpha_));


	Vec3 dir = sprite_->GetPos() - Vec3(1280.0f / 2.0f, 720.0f / 2.0f, 0);
	dir.Normalize();
	sprite_->ChangePositionAdd(dir * 10.0f);

}

void ConcentrationLine::Draw()
{

	/*===== �`�揈�� =====*/

	sprite_->Draw();

}
