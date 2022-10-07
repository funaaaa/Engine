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

	// ���}�b�v�𐶐��B
	envMap1Blas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Title/ecvMap.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	envMap2Blas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Title/ecvMap2.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	envMap3Blas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Title/ecvMap3.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	envMap1_ = PolygonInstanceRegister::Ins()->CreateInstance(envMap1Blas_, static_cast<int>(PolygonInstanceRegister::TEXCOLOR));
	envMap1_.lock()->AddScale(Vec3(300, 300, 300));
	envMap2_ = PolygonInstanceRegister::Ins()->CreateInstance(envMap2Blas_, static_cast<int>(PolygonInstanceRegister::TEXCOLOR));
	envMap2_.lock()->AddScale(Vec3(300, 300, 300));
	envMap3_ = PolygonInstanceRegister::Ins()->CreateInstance(envMap3Blas_, static_cast<int>(PolygonInstanceRegister::TEXCOLOR));
	envMap3_.lock()->AddScale(Vec3(300, 300, 300));

	pbrTestBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Gimmick/gltfTest.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	pbrTest_ = PolygonInstanceRegister::Ins()->CreateInstance(pbrTestBlas_, static_cast<int>(PolygonInstanceRegister::REFRACTION));
	pbrTest_.lock()->AddScale(Vec3(30, 30, 30));
	pbrTest_.lock()->ChangeTrans(Vec3(0, 10000, 0));

	pbrTest2_ = PolygonInstanceRegister::Ins()->CreateInstance(pbrTestBlas_, static_cast<int>(PolygonInstanceRegister::DEF));
	pbrTest2_.lock()->AddScale(Vec3(30, 30, 30));
	pbrTest2_.lock()->ChangeTrans(Vec3(0, 10000, 0));

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

	pbrTestBlas_.lock()->IsChangeMaterial();
	pbrTestBlas_.lock()->Update();

	// ���}�b�v���X�V�B
	if (Input::Ins()->IsKeyTrigger(DIK_1) && invMapIndex_ != 0) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		invMapIndex_ = 0;

	}
	else if (Input::Ins()->IsKeyTrigger(DIK_2) && invMapIndex_ != 1) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		invMapIndex_ = 1;

	}
	else if (Input::Ins()->IsKeyTrigger(DIK_3) && invMapIndex_ != 2) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 0, 0));
		invMapIndex_ = 2;

	}

	if (Input::Ins()->IsKey(DIK_UP)) {

		pbrTestBlas_.lock()->GetMaterial().metalness_ += 0.01f;
		pbrTestBlas_.lock()->GetMaterial().roughness_ = 0.3f;
		pbrTestBlas_.lock()->GetMaterial().metalness_ = FHelper::Saturate(pbrTestBlas_.lock()->GetMaterial().metalness_);

	}
	else if (Input::Ins()->IsKey(DIK_DOWN)) {


		pbrTestBlas_.lock()->GetMaterial().metalness_ -= 0.01f;
		pbrTestBlas_.lock()->GetMaterial().roughness_ = 0.3f;
		pbrTestBlas_.lock()->GetMaterial().metalness_ = FHelper::Saturate(pbrTestBlas_.lock()->GetMaterial().metalness_);


	}

	if (Input::Ins()->IsKeyTrigger(DIK_9) && objectIndex_ != 1) {

		// �v���C���[��`�悵�Ȃ��悤�ɂ���B
		player_->pos_ = Vec3(0, 10000, 0);
		pbrTest2_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		pbrTest_.lock()->ChangeTrans(Vec3(0, 0, 0));

		objectIndex_ = 1;

	}
	else if (Input::Ins()->IsKeyTrigger(DIK_0) && objectIndex_ != 2) {

		pbrTest_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		pbrTest2_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		player_->pos_ = Vec3(0, 0, 0);

		objectIndex_ = 2;


	}
	else if (Input::Ins()->IsKeyTrigger(DIK_8) && objectIndex_ != 3) {

		pbrTest_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		pbrTest2_.lock()->ChangeTrans(Vec3(0, 0, 0));
		player_->pos_ = Vec3(0, 10000, 0);

		objectIndex_ = 3;


	}

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
	Camera::Ins()->eye_ = cameraDir * -150.0f;
	Camera::Ins()->eye_.y_ = 20.0f;
	Camera::Ins()->target_ = Vec3(0, 0, 0);
	Camera::Ins()->up_ = Vec3(0, 1, 0);
	Camera::Ins()->GenerateMatView();

}

void TitleScene::Draw()
{

	// ���C�g���[�V���O�����s�B
	RayEngine::Ins()->Draw();

	//title_.Draw();

}
