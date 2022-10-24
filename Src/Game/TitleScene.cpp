#include "TitleScene.h"
#include "Input.h"
#include "Pipeline.h"
#include "Engine.h"
#include "FHelper.h"
#include "GameSceneMode.h"
#include "PolygonInstanceRegister.h"
#include "RayEngine.h"
#include "Character.h"
#include "PolygonInstance.h"
#include "PolygonInstanceRegister.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include "HitGroupMgr.h"

TitleScene::TitleScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	title_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");

}

#include "TextureManager.h"

void TitleScene::Init()
{

	/*===== ���������� =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	// �C���X�^���X���������B
	PolygonInstanceRegister::Ins()->Setting();

	// �v���C���[�𐶐��B
	player_ = std::make_shared<Character>(Character::CHARA_ID::P1, 0, 0);
	player_->pos_ = Vec3(0, 10000, 0);

	// �R�[�l���{�b�N�X�����[�h
	cornellBoxGreenBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Title/cornellBoxGreen.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	cornellBoxRedBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Title/cornellBoxRed.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	cornellBoxWhiteBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Title/cornellBoxWhite.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	cornellBoxGreen_ = PolygonInstanceRegister::Ins()->CreateInstance(cornellBoxGreenBlas_, static_cast<int>(PolygonInstanceRegister::DEF));
	cornellBoxGreen_.lock()->AddScale(Vec3(100, 100, 100));
	cornellBoxRed_ = PolygonInstanceRegister::Ins()->CreateInstance(cornellBoxRedBlas_, static_cast<int>(PolygonInstanceRegister::DEF));
	cornellBoxRed_.lock()->AddScale(Vec3(100, 100, 100));
	cornellBoxWhite_ = PolygonInstanceRegister::Ins()->CreateInstance(cornellBoxWhiteBlas_, static_cast<int>(PolygonInstanceRegister::DEF));
	cornellBoxWhite_.lock()->AddScale(Vec3(100, 100, 100));
	cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 0, 0));
	cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 0, 0));
	cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 0, 0));

	player_->pos_ = Vec3(0, -70, 0);

	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightPos_ = Vec3(0, 70, 0);
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightColor_ = Vec3(1.0f, 0.2f, 0.2f);
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightPower_ = 250.0f;

	// TLAS�𐶐��B
	RayEngine::Ins()->SettingTLAS();

	cameraAngle = 0;
	invMapIndex_ = 0;
	objectIndex_ = 1;

}

#include "Camera.h"
void TitleScene::Update()
{

	/*===== �X�V���� =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;
		player_->Init();

	}

	ImGui::Text("Choose your level!");

	// AI���S�[�X�g����I������B
	int mode = static_cast<int>(GameSceneMode::Ins()->mode_);
	ImGui::RadioButton("AI", &mode, 1);
	ImGui::SameLine();
	ImGui::RadioButton("GHOST", &mode, 3);
	GameSceneMode::Ins()->mode_ = static_cast<GameSceneMode::MODE>(mode);

	// AI��������B
	if (mode == 1) {

		int level = GameSceneMode::Ins()->level_;
		ImGui::RadioButton("Level : 1", &level, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 2", &level, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 3", &level, 2);
		GameSceneMode::Ins()->level_ = level;

	}
	// GHOST��������B
	else if (mode == 3) {

		int level = GameSceneMode::Ins()->level_;
		ImGui::RadioButton("Level : 1", &level, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 2", &level, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 3", &level, 2);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 4", &level, 3);
		GameSceneMode::Ins()->level_ = level;

	}

	// �R�[�l���{�b�N�X�̈ʒu�𒲐��B
	float offset = -40;
	cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, offset, 0));
	cornellBoxRed_.lock()->ChangeTrans(Vec3(0, offset, 0));
	cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, offset, 0));

	player_->pos_ = Vec3(0, -70 + offset, 0);

	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightPos_ = Vec3(0, -40 - offset, 0);
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightColor_ = Vec3(1.0f, 0.2f, 0.2f);
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightPower_ = 250.0f;
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightSize_ = 5.0f;
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].isShadow_ = true;

	// ���s�����𖳌����B
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = false;

	// TLAS��p�C�v���C�����X�V�B
	RayEngine::Ins()->Update();

	// �����̎���X�V�B
	RayEngine::Ins()->GetConstBufferData().debug_.seed_ = FHelper::GetRand(0, 1000);

	// �v���C���[�̈ʒu�𒲐��B
	player_->UpdateTitle();

	// �J�����̈ʒu�𒲐��B
	cameraAngle += 0.01f;

	// �J�����̊p�x����ʒu�����߂�B
	Vec3 cameraDir = Vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	Camera::Ins()->eye_ = cameraDir * -500.0f;
	Camera::Ins()->eye_.y_ = 70.0f;
	Camera::Ins()->target_ = Vec3(0, 30, 0);
	Camera::Ins()->up_ = Vec3(0, 1, 0);
	Camera::Ins()->GenerateMatView();

}

void TitleScene::Draw()
{

	RayEngine::Ins()->Draw();

	title_.Draw();

}