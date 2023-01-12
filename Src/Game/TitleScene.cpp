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
#include "RayDenoiser.h"

TitleScene::TitleScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	//title_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");

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

	// ���}�b�v�𐶐��B
	envMap1Blas_ = BLASRegister::Ins()->GenerateObj("Resource/Title/", "envMap.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	envMap2Blas_ = BLASRegister::Ins()->GenerateObj("Resource/Title/", "envMap.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	envMap2Blas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Title/envMap2.dds"));
	envMap3Blas_ = BLASRegister::Ins()->GenerateObj("Resource/Title/", "envMap.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	envMap3Blas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Title/envMap3.dds"));
	envMap1_ = PolygonInstanceRegister::Ins()->CreateInstance(envMap1Blas_, static_cast<int>(PolygonInstanceRegister::TEXCOLOR));
	envMap1_.lock()->AddScale(Vec3(300, 300, 300));
	envMap2_ = PolygonInstanceRegister::Ins()->CreateInstance(envMap2Blas_, static_cast<int>(PolygonInstanceRegister::TEXCOLOR));
	envMap2_.lock()->AddScale(Vec3(300, 300, 300));
	envMap3_ = PolygonInstanceRegister::Ins()->CreateInstance(envMap3Blas_, static_cast<int>(PolygonInstanceRegister::TEXCOLOR));
	envMap3_.lock()->AddScale(Vec3(300, 300, 300));

	for (auto& height : pbrTestBlas_) {
		for (auto& width : height) {

			width = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Gimmick/gltfTest.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
			width.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Title/white.png"));

		}
	}
	for (int height = 0; height < 10; ++height) {
		for (int width = 0; width < 10; ++width) {

			pbrTest_[height][width] = PolygonInstanceRegister::Ins()->CreateInstance(pbrTestBlas_[height][width], static_cast<int>(PolygonInstanceRegister::PBR_TEST));
			pbrTest_[height][width].lock()->ChangeScale(Vec3(5, 5, 5));
			pbrTest_[height][width].lock()->ChangeTrans(Vec3(0, 10000, 0));

		}
	}



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
	cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
	cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
	cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));

	// TLAS�𐶐��B
	RayEngine::Ins()->SettingTLAS();

	cameraAngle = 0;
	invMapIndex_ = 0;
	objectIndex_ = 3;
	isCameraAngleChange_ = true;
	Denoiser::Ins()->GetDenoiseDebugParam() = 1280.0f;

}

#include "Camera.h"
void TitleScene::Update()
{

	/*===== �X�V���� =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		//isTransition_ = true;

	}

	static int counter = 0;
	if (counter == 0) {
		++counter;
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(0.5f, 0.5f, 0.5f).GetNormal();
	}

	// FPS���v�����ĕ\��
	{
		static DWORD prev_time = timeGetTime();	// �O��̎���
		static int frame_count = 0;		// �t���[���J�E���g
		static int fps = 60;
		DWORD now_time = timeGetTime();		// ����̃t���[���̎���

		++frame_count;	// �t���[�������J�E���g����

		// �o�ߎ��Ԃ��P�b�𒴂�����J�E���g�Ǝ��Ԃ����Z�b�g
		if (now_time - prev_time >= 1000)
		{
			fps = frame_count;
			prev_time = now_time;
			frame_count = 0;
		}
		std::string fpsstr = "FPS" + std::to_string(fps);
		ImGui::Text(fpsstr.c_str(), &invMapIndex_, 0);
		ImGui::Text("", &invMapIndex_, 0);
	}

	if (ImGui::TreeNode("SceneInfo")) {

		ImGui::RadioButton("Street", &invMapIndex_, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Building", &invMapIndex_, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Grasslands", &invMapIndex_, 2);
		ImGui::SameLine();
		ImGui::RadioButton("CornellBox", &invMapIndex_, 3);

		if (invMapIndex_ == 3) {
			ImGui::RadioButton("SoftShadow", &objectIndex_, 2);
		}
		else {
			ImGui::RadioButton("Sphere", &objectIndex_, 2);
		}
		ImGui::SameLine();
		ImGui::RadioButton("Car", &objectIndex_, 3);

		ImGui::TreePop();

	}

	if (ImGui::TreeNode("CameraInfo")) {

		ImGui::Checkbox("CameraMoveFlag", &isCameraAngleChange_);
		ImGui::SameLine();
		ImGui::DragFloat("CameraAngle", &cameraAngle, 0.03f);

		bool isMipmap = !RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].pad_.x;
		ImGui::Checkbox("MipmapFlag", &isMipmap);
		RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].pad_.x = !isMipmap;

		ImGui::TreePop();

	}


	if (ImGui::TreeNode("LightInfo")) {

		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;
		ImGui::SliderFloat("DirLightX", &RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.x_, -0.999f, 0.999f);
		ImGui::SliderFloat("DirLightY", &RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_, -0.899f, 0.999f);
		ImGui::SliderFloat("DirLightZ", &RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.z_, -0.999f, 0.999f);

		ImGui::TreePop();

	}

	if (ImGui::TreeNode("DebugInfo")) {

		bool buff = static_cast<bool>(RayEngine::Ins()->GetConstBufferData().debug_.isNoiseScene_);
		ImGui::Checkbox("Noise", &buff);
		RayEngine::Ins()->GetConstBufferData().debug_.isNoiseScene_ = static_cast<int>(buff);

		// �m�C�Y�Ƀ`�F�b�N�������Ă�����B
		if (buff) {

			// Slider��p�ӁB
			ImGui::SliderFloat("DenoiseLine", &Denoiser::Ins()->GetDenoiseDebugParam(), 0.0f, 1280.0f);

		}
		else {
			
			Denoiser::Ins()->GetDenoiseDebugParam() = 1280.0f;

		}

		buff = static_cast<bool>(RayEngine::Ins()->GetConstBufferData().debug_.isMeshScene_);
		ImGui::Checkbox("Mesh", &buff);
		RayEngine::Ins()->GetConstBufferData().debug_.isMeshScene_ = static_cast<int>(buff);

		// Meesh�Ƀ`�F�b�N�������Ă�����B
		if (buff) {

			// Slider��p�ӁB
			ImGui::DragInt("MeshParamLine", &RayEngine::Ins()->GetConstBufferData().debug_.isNormalScene_, 1);

		}

		ImGui::TreePop();

	}

	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

	if (RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_ <= -0.85f) {
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_ = -0.84f;
	}

	if (std::isnan(RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.x_)) {
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(0.5f, 0.5f, 0.5f);
	}

	// ���}�b�v���X�V�B
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightColor_ = Vec3(1.0f, 1.0f, 1.0f);
	switch (invMapIndex_)
	{

	case 0:

		envMap1_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 0;

		break;

	case 1:

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 1;

		break;

	case 2:

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 0, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 2;

		break;

	case 3:

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 0, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 0, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 0, 0));
		invMapIndex_ = 3;

		RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].isActive_ = true;
		RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightPos_ = Vec3(0, 70, 0);
		RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightColor_ = Vec3(1.0f, 0.2f, 0.2f);
		RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightPower_ = 250.0f;

		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = false;

		break;

	default:
		break;
	}

	if (objectIndex_ == 2) {

		// �X�P�[�������ɖ߂��B
		pbrTest_[0][0].lock()->ChangeScale(Vec3(10, 10, 10));

		player_->pos_ = Vec3(0, 1000, 0);

		// �R�[�l���{�b�N�X��������
		if (invMapIndex_ == 3) {

			for (int height = 0; height < 10; ++height) {
				for (int width = 0; width < 10; ++width) {

					pbrTest_[height][width].lock()->ChangeTrans(Vec3(0, 10000, 0));

				}
			}

			pbrTest_[0][0].lock()->ChangeTrans(Vec3(0, 0, 0));
			pbrTest_[0][0].lock()->ChangeScale(Vec3(20, 20, 20));
			pbrTestBlas_[0][0].lock()->GetMaterial().metalness_ = 0.021f;
			pbrTestBlas_[0][0].lock()->GetMaterial().roughness_ = 0.390f;
			pbrTestBlas_[0][0].lock()->GetMaterial().specular_ = 0.999f;
			pbrTestBlas_[0][0].lock()->IsChangeMaterial();

		}
		else {

			float interval = 10.0f;
			Vec3 offsetPos = Vec3(-interval * 5.0f, -interval * 5.0f + 10.0f, 0.0f);
			for (int height = 0; height < 10; ++height) {
				for (int width = 0; width < 10; ++width) {

					pbrTest_[height][width].lock()->ChangeTrans(offsetPos + Vec3(width * interval, height * interval, 0));
					pbrTest_[height][width].lock()->ChangeScale(Vec3(5, 5, 5));

					float roughness = (1.0f / 10.0f) * height;
					if (roughness <= 0.0f) roughness = 0.001f;
					if (1.0f <= roughness) roughness = 0.999f;
					pbrTestBlas_[height][width].lock()->GetMaterial().roughness_ = roughness;
					float metalness = (1.0f / 10.0f) * (10.0f - width);
					if (metalness <= 0.0f) metalness = 0.001f;
					if (1.0f <= metalness) metalness = 0.999f;
					pbrTestBlas_[height][width].lock()->GetMaterial().metalness_ = metalness;
					pbrTestBlas_[height][width].lock()->IsChangeMaterial();

				}

			}

		}

	}
	else if (objectIndex_ == 3) {

		for (auto& height : pbrTest_) {
			for (auto& width : height) {

				width.lock()->ChangeTrans(Vec3(0, 1000, 0));

			}
		}

		player_->pos_ = Vec3(0, 0, 0);

		// �R�[�l���{�b�N�X��������B
		if (invMapIndex_ == 3) {
			player_->pos_ = Vec3(0, -70, 0);
		}


	}

	// �v���C���[�̈ʒu�𒲐��B
	player_->UpdateTitle();

	// TLAS��p�C�v���C�����X�V�B
	RayEngine::Ins()->Update();

	// �����̎���X�V�B
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	if (isCameraAngleChange_) {
		cameraAngle += 0.005f;
	}

	// �J�����̊p�x����ʒu�����߂�B
	Vec3 cameraDir = Vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	Camera::Ins()->eye_ = cameraDir * -150.0f;
	if (invMapIndex_ == 3) {
		Camera::Ins()->eye_ = cameraDir * -350.0f;
	}
	Camera::Ins()->eye_.y_ = 20.0f;
	Camera::Ins()->target_ = Vec3(0, 0, 0);
	Camera::Ins()->up_ = Vec3(0, 1, 0);
	Camera::Ins()->GenerateMatView();

}

void TitleScene::Draw()
{

	static bool isNoise = false;
	if (Input::Ins()->IsKeyTrigger(DIK_SPACE)) isNoise = isNoise ? false : true;

	// ���C�g���[�V���O�����s�B
	if (isNoise) {

		RayEngine::Ins()->NoiseDraw();

	}
	else {

		RayEngine::Ins()->Draw();

	}

	//title_.Draw();

}