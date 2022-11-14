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

	/*===== コンストラクタ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	//title_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");

}

#include "TextureManager.h"

void TitleScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	// インスタンスを初期化。
	PolygonInstanceRegister::Ins()->Setting();

	// プレイヤーを生成。
	player_ = std::make_shared<Character>(Character::CHARA_ID::P1, 0, 0);
	player_->pos_ = Vec3(0, 10000, 0);

	// 環境マップを生成。
	envMap1Blas_ = BLASRegister::Ins()->GenerateObj("Resource/Title/", "envMap.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	envMap2Blas_ = BLASRegister::Ins()->GenerateObj("Resource/Title/", "envMap.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	envMap2Blas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Title/envMap2.dds"));
	envMap3Blas_ = BLASRegister::Ins()->GenerateObj("Resource/Title/", "envMap.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
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
			width.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Title/red.png"));

		}
	}
	for (int height = 0; height < 5; ++height) {
		for (int width = 0; width < 5; ++width) {

			pbrTest_[height][width] = PolygonInstanceRegister::Ins()->CreateInstance(pbrTestBlas_[height][width], static_cast<int>(PolygonInstanceRegister::PBR_TEST));
			pbrTest_[height][width].lock()->AddScale(Vec3(10, 10, 10));
			pbrTest_[height][width].lock()->ChangeTrans(Vec3(0, 10000, 0));

		}
	}



	// コーネルボックスをロード
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

	// TLASを生成。
	RayEngine::Ins()->SettingTLAS();

	cameraAngle = 0;
	invMapIndex_ = 0;
	objectIndex_ = 3;

	isStopRotate_ = false;

}

#include "Camera.h"
void TitleScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		//isTransition_ = true;

	}

	ImGui::Text("SceneSelect");

	ImGui::RadioButton("Street", &invMapIndex_, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Building", &invMapIndex_, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Grasslands", &invMapIndex_, 2);
	ImGui::SameLine();
	ImGui::RadioButton("CornellBox", &invMapIndex_, 3);

	ImGui::Text("SelectObject");

	if (invMapIndex_ == 3) {
		ImGui::RadioButton("SoftShadow", &objectIndex_, 2);
	}
	else {
		ImGui::RadioButton("ReflectionObject", &objectIndex_, 2);
	}
	ImGui::SameLine();
	ImGui::RadioButton("Car", &objectIndex_, 3);

	ImGui::Checkbox("StopRotation", &isStopRotate_);



	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;

	// 環境マップを更新。
	RayEngine::Ins()->GetConstBufferData().light_.pointLight_[0].lightColor_ = Vec3(1.0f, 1.0f, 1.0f);
	if (invMapIndex_ == 0) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 0;

	}
	else if (invMapIndex_ == 1) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 1;

	}
	else if (invMapIndex_ == 2) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 0, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 2;


	}
	else if (invMapIndex_ == 3) {

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


	}

	if (objectIndex_ == 2) {

		// スケールを元に戻す。
		pbrTest_[0][0].lock()->ChangeScale(Vec3(10, 10, 10));

		player_->pos_ = Vec3(0, 1000, 0);

		// コーネルボックスだったら
		if (invMapIndex_ == 3) {

			for (int height = 0; height < 5; ++height) {
				for (int width = 0; width < 5; ++width) {

					pbrTest_[height][width].lock()->ChangeTrans(Vec3(0, 10000, 0));

				}
			}

			pbrTest_[0][0].lock()->ChangeTrans(Vec3(0, 0, 0));
			pbrTest_[0][0].lock()->ChangeScale(Vec3(30, 30, 30));
			pbrTestBlas_[0][0].lock()->GetMaterial().metalness_ = 0.021f;
			pbrTestBlas_[0][0].lock()->GetMaterial().roughness_ = 0.390f;
			pbrTestBlas_[0][0].lock()->GetMaterial().specular_ = 0.999f;
			pbrTestBlas_[0][0].lock()->IsChangeMaterial();

		}
		else {

			float interval = 20.0f;
			Vec3 offsetPos = Vec3(-interval * 2.5f, -interval * 2.5f + 10.0f, 0.0f);
			for (int height = 0; height < 5; ++height) {
				for (int width = 0; width < 5; ++width) {

					pbrTest_[height][width].lock()->ChangeTrans(offsetPos + Vec3(width * interval, height * interval, 0));

					float roughness = (1.0f / 5.0f) * height;
					if (roughness <= 0.0f) roughness = 0.001f;
					if (1.0f <= roughness) roughness = 0.999f;
					pbrTestBlas_[height][width].lock()->GetMaterial().roughness_ = roughness;
					float metalness = (1.0f / 5.0f) * (5 - width);
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

		// コーネルボックスだったら。
		if (invMapIndex_ == 3) {
			player_->pos_ = Vec3(0, -70, 0);
		}


	}

	// TLASやパイプラインを更新。
	RayEngine::Ins()->Update();

	// 乱数の種を更新。
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// プレイヤーの位置を調整。
	player_->UpdateTitle();

	if (!isStopRotate_) {

		// カメラの位置を調整。
		cameraAngle += 0.01f;

	}

	// カメラの角度から位置を求める。
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

	// レイトレーシングを実行。
	if (isNoise) {

		RayEngine::Ins()->NoiseDraw();

	}
	else {

		RayEngine::Ins()->Draw();

	}

	//title_.Draw();

}