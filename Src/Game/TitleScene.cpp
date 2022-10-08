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

	title_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");

}

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
	objectIndex_ = 1;

}

#include "Camera.h"
void TitleScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

	ImGui::Text("Choose your level!");

	// AIかゴーストかを選択する。
	int mode = static_cast<int>(GameSceneMode::Ins()->mode_);
	ImGui::RadioButton("AI", &mode, 1);
	ImGui::SameLine();
	ImGui::RadioButton("GHOST", &mode, 3);
	GameSceneMode::Ins()->mode_ = static_cast<GameSceneMode::MODE>(mode);

	// AIだったら。
	if (mode == 1) {

		int level = GameSceneMode::Ins()->level_;
		ImGui::RadioButton("Level : 1", &level, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 2", &level, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Level : 3", &level, 2);
		GameSceneMode::Ins()->level_ = level;

	}
	// GHOSTだったら。
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

	// 環境マップを更新。
	if (Input::Ins()->IsKeyTrigger(DIK_1) && invMapIndex_ != 0) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 0;

	}
	else if (Input::Ins()->IsKeyTrigger(DIK_2) && invMapIndex_ != 1) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 0, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 1;

	}
	else if (Input::Ins()->IsKeyTrigger(DIK_3) && invMapIndex_ != 2) {

		envMap1_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap2_.lock()->ChangeTrans(Vec3(0, 1000, 0));
		envMap3_.lock()->ChangeTrans(Vec3(0, 0, 0));
		cornellBoxGreen_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxRed_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		cornellBoxWhite_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		invMapIndex_ = 2;

	}
	else if (Input::Ins()->IsKeyTrigger(DIK_4) && invMapIndex_ != 3) {

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

		// プレイヤーを描画しないようにする。
		player_->pos_ = Vec3(0, 10000, 0);
		pbrTest2_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		pbrTest_.lock()->ChangeTrans(Vec3(0, 0, 0));

		objectIndex_ = 1;

	}
	else if (Input::Ins()->IsKeyTrigger(DIK_0) && objectIndex_ != 2) {

		pbrTest_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		pbrTest2_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		player_->pos_ = Vec3(0, 0, 0);

		// カーネルボックスだったら。
		if (invMapIndex_ == 3) {
			player_->pos_ = Vec3(0, -70, 0);
		}

		objectIndex_ = 2;


	}
	else if (Input::Ins()->IsKeyTrigger(DIK_8) && objectIndex_ != 3) {

		pbrTest_.lock()->ChangeTrans(Vec3(0, 10000, 0));
		pbrTest2_.lock()->ChangeTrans(Vec3(0, 0, 0));
		player_->pos_ = Vec3(0, 10000, 0);

		objectIndex_ = 3;


	}

	// 並行光源を無効化。
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = false;

	// TLASやパイプラインを更新。
	RayEngine::Ins()->Update();

	// 乱数の種を更新。
	RayEngine::Ins()->GetConstBufferData().debug_.seed_ = FHelper::GetRand(0, 1000);

	// プレイヤーの位置を調整。
	player_->UpdateTitle();

	// カメラの位置を調整。
	cameraAngle += 0.01f;

	// カメラの角度から位置を求める。
	Vec3 cameraDir = Vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	Camera::Ins()->eye_ = cameraDir * -150.0f;
	if (invMapIndex_ == 3) {
		Camera::Ins()->eye_ = cameraDir * -500.0f;
	}
	Camera::Ins()->eye_.y_ = 20.0f;
	Camera::Ins()->target_ = Vec3(0, 0, 0);
	Camera::Ins()->up_ = Vec3(0, 1, 0);
	Camera::Ins()->GenerateMatView();

}

void TitleScene::Draw()
{

	// レイトレーシングを実行。
	RayEngine::Ins()->Draw();

	//title_.Draw();

}


/*


コーネルボックスを作る。
→赤と緑と白い部分に分けてモデルを作成する。
→赤と緑は反射強めで、白い部分はGIがんぶり。
→完全反射のモデルと反射しないモデルを用意。
→壁のモデルの法線は反転させておく。
→並行光源が差し込まれなかったら点光源を配置する。暖色系のライトがいいかも。

コーネルボックスができたら、タイトル画面で環境マップとコーネルボックスがランダムで出るようにする。

それができたらステージを装飾。
→木のモデルをマップに配置。
→ステージの脇には柵を配置。


*/