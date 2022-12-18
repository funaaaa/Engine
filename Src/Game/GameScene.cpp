#include "GameScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Character.h"
#include "FHelper.h"
#include "Camera.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipeline.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "Input.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Pipeline.h"
#include "WindowsAPI.h"
#include "MugenStage.h"
#include "RayComputeShader.h"
#include "StageObjectMgr.h"
#include "BLAS.h"
#include "ShellObjectMgr.h"
#include "CharacterMgr.h"
#include "GameSceneMode.h"
#include "DriftParticleMgr.h"
#include "ConcentrationLineMgr.h"
#include "PolygonInstance.h"
#include "BaseItem.h"
#include "BrightnessParam.h"
#include "CharacterTireMask.h"

#include "RayEngine.h"

#include "GLTF.h"
#include "SceneTransition.h"

GameScene::GameScene()
{

	/*===== ���������� =====*/

	Vec3 triangleNormal = Vec3(100, 100, 0).GetNormal();
	Vec3 worldRayOrigin = Vec3(100, 100, 0);
	Vec3 worldPosition = Vec3(0, 0, 0);
	Vec3 rayDirX = Vec3(-100, -100, 0).GetNormal();

	float lengthX = Vec3(-triangleNormal).Dot(worldRayOrigin - worldPosition) / triangleNormal.Dot(rayDirX);
	Vec3 impPosX = rayDirX * lengthX + worldRayOrigin;



	// �b���I�u�W�F�N�g���Z�b�e�B���O�B
	ShellObjectMgr::Ins()->Setting();

	// �^�C�����p�N���X���Z�b�g�B
	tireMaskTexture_ = std::make_shared<RaytracingOutput>();
	tireMaskTexture_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskTextureOutput_ = std::make_shared<RaytracingOutput>();
	tireMaskTextureOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskComputeShader_ = std::make_shared<RayComputeShader>();
	tireMaskComputeShader_->Setting(L"Resource/ShaderFiles/RayTracing/TireMaskComputeShader.hlsl", 0, 1, 1, { tireMaskTextureOutput_->GetUAVIndex() });
	tireMaskConstBuffer_ = std::make_shared<DynamicConstBuffer>();
	tireMaskConstBuffer_->Generate(sizeof(CharacterTireMask::TireMaskUV) * 2, L"TireMaskUV");

	// ���h��R���s���[�g�V�F�[�_�[
	whiteOutComputeShader_ = std::make_shared<RayComputeShader>();
	whiteOutComputeShader_->Setting(L"Resource/ShaderFiles/WhiteMakeUpShader.hlsl", 0, 0, 0, {});

	characterMgr_ = std::make_shared<CharacterMgr>();

	// Instance�̃��[���h�s��𐶐��B
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// �ݒ�
	DriftParticleMgr::Ins()->Setting();

	// ���z�Ɋւ���ϐ�
	sunAngle_ = 0.495f;
	sunSpeed_ = 0.0005f;

	isDisplayFPS_ = false;

	nextScene_ = SCENE_ID::TITLE;
	isTransition_ = false;

	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;
	BrightnessParam::Ins()->isBright_ = false;


	// �t�H���g�����[�h
	{
		numFontHandle_[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/0.dds");
		numFontHandle_[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/1.dds");
		numFontHandle_[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/2.dds");
		numFontHandle_[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/3.dds");
		numFontHandle_[4] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/4.dds");
		numFontHandle_[5] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/5.dds");
		numFontHandle_[6] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/6.dds");
		numFontHandle_[7] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/7.dds");
		numFontHandle_[8] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/8.dds");
		numFontHandle_[9] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/9.dds");
		numFontHandle_[10] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/slash.dds");
	}


	// �X�v���C�g�𐶐��B
	rapUI_ = std::make_shared<Sprite>();
	rapUI_->GenerateForTexture(Vec3(140, 647, 0.1f), Vec2(224 / 2.0f, 64 / 2.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/rapUI.dds");

	nowRapCountUI_ = std::make_shared<Sprite>();
	nowRapCountUI_->GenerateSpecifyTextureID(Vec3(134, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[1]);
	slashUI_ = std::make_shared<Sprite>();
	slashUI_->GenerateSpecifyTextureID(Vec3(169, 651, 0.1f), Vec2(16.0f * 0.6f, 32.0f * 0.6f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[10]);
	maxRapCountUI_ = std::make_shared<Sprite>();
	maxRapCountUI_->GenerateSpecifyTextureID(Vec3(194, 651, 0.1f), Vec2(16.0f * 0.5f, 32.0f * 0.5f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[3]);

	// �����L���O�Ɏg�p����t�H���g�����[�h
	rankingFont_[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank1.png");
	rankingFont_[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank2.png");
	rankingFont_[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank3.png");
	rankingFont_[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/rank4.png");
	rankingUI_ = std::make_shared<Sprite>();
	rankingUI_->GenerateSpecifyTextureID(RANK_UI_POS, Vec2(64, 64), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, rankingFont_[0]);
	rankUIEasingTimer_ = 1;
	isRankUIExp_ = true;

	// ���s��UI�𐶐�
	Vec3 WIN_GAMEOVER_POS = Vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.25f, 0.1f);
	winUI_ = std::make_shared<Sprite>();
	winUI_->GenerateForTexture(WIN_GAMEOVER_POS, Vec2(0, 0), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/win.png");
	gameoverUI_ = std::make_shared<Sprite>();
	gameoverUI_->GenerateForTexture(WIN_GAMEOVER_POS, Vec2(0, 0), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/gameover.png");
	gameFinishUISizeRate_ = 0.0f;
	gameFinishUIEasingTimer_ = 1;

	// �W����
	concentrationLine_ = std::make_shared<ConcentrationLineMgr>();

	// �J�E���g�_�E���pUI
	countDownSprite_ = std::make_shared<Sprite>();
	countDownSprite_->GenerateSpecifyTextureID(COUNT_DOWN_START_POS, COUNT_DOWN_FONT_SIZE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[2]);
	countDownSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));
	goSprite_ = std::make_shared<Sprite>();
	goSprite_->GenerateForTexture(WINDOW_CENTER, GO_FONT_SIZE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/go.dds");
	goSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));

	// �X�e�[�W���Z�b�e�B���O����B
	stages_.emplace_back(std::make_shared<MugenStage>());

}

void GameScene::Init()
{

	/*===== ���������� =====*/

	// �C���X�^���X���������B
	BLASRegister::Ins()->Setting();
	PolygonInstanceRegister::Ins()->Setting();

	nextScene_ = SCENE_ID::TITLE;
	isTransition_ = false;

	concentrationLine_->Init();

	countDownSprite_->ChangePosition(Vec3(100000, 10000, 100));
	goSprite_->ChangePosition(Vec3(100000, 10000, 100));

	characterMgr_ = std::make_shared<CharacterMgr>();

	if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::AI) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// AI�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_, 0);
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_, 1);
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_, 2);

	}
	else if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::DEF) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

	}
	else if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::WRITE_GHOST) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1_WGHOST), true);

	}
	else if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::GHOST) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// �S�[�X�g�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::GHOST), false, GameSceneMode::Ins()->level_);

	}

	// �L�����̏����ʒu��ݒ�B
	characterMgr_->Setting();

	// ��U�T�[�L�b�g�X�e�[�W��L��������B
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex());


	// TLAS�𐶐��B
	RayEngine::Ins()->SettingTLAS();


	Camera::Ins()->Init();

	DriftParticleMgr::Ins()->Init();

	BrightnessParam::Ins()->isBright_ = false;
	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;
	isStartTransition_ = false;
	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	sunAngle_ = 0.495f;
	isFinishTransition_ = false;
	rankUIEasingTimer_ = 1;
	isRankUIExp_ = true;

	gameFinishUISizeRate_ = 0.0f;
	gameFinishUIEasingTimer_ = 0;

	Camera::Ins()->eye_ = Vec3(0, 0, 0);
	Camera::Ins()->target_ = Vec3(10, 0, 0);


	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ = 2.4f;

}

#include <ostream>
#include <iostream>
#include <fstream>
void GameScene::Update()
{

	/*===== �X�V���� =====*/

	// �V�[���J�ڂ��I��点�Ă��Ȃ�������I��点��B
	if (!isFinishTransition_) {
		SceneTransition::Ins()->Exit();
		isFinishTransition_ = true;
	}

	// ���͏���
	Input();

	// �L�������X�V�B
	characterMgr_->Update(stages_[STAGE_ID::MUGEN], isBeforeStart_, isGameFinish_);

	// �����̎���X�V�B
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 100000);

	// �J�������X�V�B
	Camera::Ins()->Update(characterMgr_->GetPlayerIns().lock()->GetPos(), characterMgr_->GetPlayerIns().lock()->GetCameraForwardVec(), characterMgr_->GetPlayerIns().lock()->GetUpVec(), characterMgr_->GetPlayerIns().lock()->GetNowSpeedPer(), isBeforeStart_, isGameFinish_);

	// �����ꂩ�̃L�������S�[�����Ă����烊�U���g�V�[���Ɉړ�����B
	bool isGoalPlayer = false;
	if (characterMgr_->CheckGoal(isGoalPlayer)) {

		// �S�[�������g���K�[��������
		if (!isGameFinish_) {
			// �X�P�[����ς���B
			winUI_->ChangeScale(0, 0, 1.0f);
			gameoverUI_->ChangeScale(0, 0, 1.0f);
		}

		isGameFinish_ = true;
		isWin_ = isGoalPlayer;

		++transitionTimer;
		if (TRANSION_TIME < transitionTimer) {

			if (!isStartTransition_) {
				isStartTransition_ = true;
				SceneTransition::Ins()->Appear();
			}

		}

	}

	// �J�n���Ă��Ȃ�������J�E���g�_�E���̏������s���B
	UpdateCountDown();


	// ���b�v����UI���X�V�B
	int rapCount = characterMgr_->GetPlayerIns().lock()->GetRapCount();
	if (3 <= rapCount) {

		rapCount = 2;

	}
	nowRapCountUI_->ChangeTextureID(numFontHandle_[rapCount + 1], 0);

	// �X�e�[�W���X�V�B
	stages_[STAGE_ID::MUGEN]->Update(characterMgr_);

	// TLAS��p�C�v���C�����X�V�B
	RayEngine::Ins()->Update();

	// ���z�̊p�x���X�V�B
	sunAngle_ += sunSpeed_;
	if (0.0f < RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_) {

		sunAngle_ += sunSpeed_ * 50.0f;

	}
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

	// �b�����X�V�B
	ShellObjectMgr::Ins()->Update(stages_[STAGE_ID::MUGEN]);

	// �����X�V����B
	DriftParticleMgr::Ins()->Update();

	// �W�������X�V�B
	if (characterMgr_->GetPlayerIns().lock()->GetIdConcentrationLine()) {
		concentrationLine_->Generate();
	}
	concentrationLine_->Update();

	// �V�[���J�ڂ̏���
	if (isStartTransition_ && SceneTransition::Ins()->GetIsFinish()) {
		isTransition_ = true;

		characterMgr_->Init();

	}

	// �Q�[�����I����Ă�����
	if (isGameFinish_) {

		// UI�̍X�V�������s���B
		gameFinishUIEasingTimer_ += GAME_FINISH_UI_EASING_TIMER;

		// �C�[�W���O�̏������s���B
		float easingAmount = FEasing::EaseInOutExpo(gameFinishUIEasingTimer_);
		if (1.0f < gameFinishUIEasingTimer_) {

			gameFinishUIEasingTimer_ = 1.0f;

		}

		// �X�P�[����ς���B
		winUI_->ChangeScale(WIN_UI_SIZE.x_, WIN_UI_SIZE.y_ * easingAmount, 1.0f);
		gameoverUI_->ChangeScale(GAMEOVER_UI_SIZE.x_, GAMEOVER_UI_SIZE.y_ * easingAmount, 1.0f);

	}


	const float PARAM_A_MIN = 2.4f;
	const float PARAM_A_MAX = 10.4f;
	const float PARAM_B_MIN = 1.055f;
	const float PARAM_B_MAX = 1.204f;

	float div = 10.0f;
	// ��ʂ̖��邳��␳�B
	if (BrightnessParam::Ins()->isBright_) {
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ += (PARAM_A_MAX - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_) / div;
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_ += (PARAM_B_MAX - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_) / div;
	}
	else {
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ += (PARAM_A_MIN - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_) / div;
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_ += (PARAM_B_MIN - RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_) / div;
	}

	// �����L���O��UI���X�V����B
	rankUIEasingTimer_ += RANK_UI_EASING_TIMER;
	if (1.0f < rankUIEasingTimer_) {

		// �g������������B
		if (isRankUIExp_) {
			rankUIEasingTimer_ = 1.0f;
		}
		else {
			rankUIEasingTimer_ = 0.0f;
			isRankUIExp_ = true;
			rankingUI_->ChangeTextureID(rankingFont_[characterMgr_->GetPlayerRanking()], 0);
		}

	}

	// �X�P�[�����X�V����B
	float rankingEasingAmount = 0.0f;
	if (isRankUIExp_) {
		rankingEasingAmount = FEasing::EaseInCubic(rankUIEasingTimer_);
	}
	else {
		rankingEasingAmount = 1.0f - FEasing::EaseOutCubic(rankUIEasingTimer_);
	}
	rankingUI_->ChangeScale(Vec3(RANK_UI_SIZE.x_, RANK_UI_SIZE.y_ * rankingEasingAmount, 1.0f));
	rankingUI_->SetColor(DirectX::XMFLOAT4(1, 1, 1, rankingEasingAmount));

	// �����L���O��UI���ς�����u�Ԃ�������C�[�W���O�����������ăe�N�X�`����ς���B
	if (characterMgr_->GetIsChangePlayerRanking()) {
		rankUIEasingTimer_ = 0;
		isRankUIExp_ = false;
	}

}

void GameScene::Draw()
{

	/*===== �`�揈�� =====*/


	// ���C�g���[�V���O�����s�B
	RayEngine::Ins()->Draw();


	// ���𔒓h��
	static int firstTimeClean = 0;
	if (firstTimeClean == 0) {

		whiteOutComputeShader_->Dispatch(4096 / 32, 4096 / 32, 1, tireMaskTexture_->GetUAVIndex()); \
			++firstTimeClean;

	}

	// �^�C�������������ށB
	std::vector<CharacterTireMask::TireMaskUV> tireMaskUV;
	bool isWriteTireMask = characterMgr_->CheckTireMask(stages_[STAGE_ID::MUGEN], tireMaskUV);

	if (isWriteTireMask) {

		// UAV���������ށB
		tireMaskConstBuffer_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), tireMaskUV.data(), sizeof(CharacterTireMask::TireMaskUV) * 2);
		tireMaskComputeShader_->Dispatch(1, 1, 1, tireMaskTexture_->GetUAVIndex(), { tireMaskConstBuffer_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTexture_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTextureOutput_->GetRaytracingOutput().Get())
			};

			Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(2, barrierToUAV);
		}

	}

	// UI��`��
	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	concentrationLine_->Draw();

	// ���݂̃��b�v����ui�B
	nowRapCountUI_->Draw();
	slashUI_->Draw();
	maxRapCountUI_->Draw();

	// ������ui�̃t���[���B
	rapUI_->Draw();

	// �J�E���g�_�E���p��ui�B
	countDownSprite_->Draw();

	// �J�E���g�_�E���I������go��ui�B
	goSprite_->Draw();

	// ���s��`��
	if (isGameFinish_) {

		if (isWin_) {
			winUI_->Draw();
		}
		else {
			gameoverUI_->Draw();
		}

	}

	// �����L���O��UI��\��
	rankingUI_->Draw();

	// �V�[���J�ڂ̉摜��`��B
	SceneTransition::Ins()->Draw();

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

}

#pragma comment (lib, "winmm.lib")

void GameScene::FPS()
{

	/*===== �^�C�g���o�[��FPS�\�� =====*/

	static DWORD prev_time = timeGetTime();	// �O��̎���
	static int frame_count = 0;		// �t���[���J�E���g
	DWORD now_time = timeGetTime();		// ����̃t���[���̎���

	frame_count++;	// �t���[�������J�E���g����

	// �o�ߎ��Ԃ��P�b�𒴂�����J�E���g�Ǝ��Ԃ����Z�b�g
	if (now_time - prev_time >= 1000)
	{
		wchar_t fps[1000];
		_itow_s(frame_count, fps, 10);
		wchar_t moji[] = L"FPS";
		wcscat_s(fps, moji);
		SetWindowText(Engine::Ins()->windowsAPI_->hwnd_, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}

}

void GameScene::Input()
{

	/*===== ���͏��� =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_START) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		if (!isStartTransition_) {
			isStartTransition_ = true;
			SceneTransition::Ins()->Appear();
		}

	}

	InputImGUI();

}

#include "BaseItem.h"
#include "Character.h"
#include "BLAS.h"
void GameScene::InputImGUI()
{

	/*===== IMGUI�X�V =====*/

	ImGui::DragFloat("SunAngle", &sunAngle_, 0.005f);

}

void GameScene::UpdateCountDown()
{

	/*===== �Q�[���J�n�O�̍X�V���� =====*/

	// �Q�[�����J�n���Ă�����B
	if (!isBeforeStart_) {

		if (countDownEasingTimer_ < 1.0f) {

			// UI�𓮂����p�̃^�C�}�[���X�V�B
			countDownEasingTimer_ += COUNT_DOWN_EASING_TIMER;

		}
		// �^�C�}�[���J���X�g������B
		else {

			if (!isCountDownExit_) {

				isCountDownExit_ = true;
				countDownEasingTimer_ = 0.0f;

			}
			else {

				return;

			}

		}

		// GO���o������������B
		if (!isCountDownExit_) {

			// �C�[�W���O�ʂ��v�Z�B
			float easingAmount = FEasing::EaseOutQuint(countDownEasingTimer_);

			// �A���t�@�l��ς���B
			goSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, easingAmount));

			// �ł����o�[�W�����̃T�C�Y�B
			const float BIG_SIZE = 3.0f;
			Vec3 BIG_GO_FONT_SIZE = Vec3(GO_FONT_SIZE.x_ * BIG_SIZE, GO_FONT_SIZE.y_ * BIG_SIZE, 1.0f);
			Vec3 DEF_FONT_SIZE = Vec3(GO_FONT_SIZE.x_, GO_FONT_SIZE.y_, 1.0f);

			goSprite_->ChangeScale(BIG_GO_FONT_SIZE + (DEF_FONT_SIZE - BIG_GO_FONT_SIZE) * easingAmount);

			goSprite_->ChangePosition(WINDOW_CENTER);

		}
		else {

			// �C�[�W���O�ʂ��v�Z�B
			float easingAmount = FEasing::EaseOutSine(countDownEasingTimer_);

			// �C�[�W���O�Ɏg�p����t�H���g�T�C�Y�B
			Vec3 DEF_FONT_SIZE = Vec3(GO_FONT_SIZE.x_, GO_FONT_SIZE.y_, 1.0f);
			Vec3 EXIT_SIZE = DEF_FONT_SIZE + DEF_FONT_SIZE / 5.0f;

			goSprite_->ChangeScale((DEF_FONT_SIZE + (EXIT_SIZE - DEF_FONT_SIZE) * easingAmount));

			// �C�[�W���O�̒l�𔽓]������B
			easingAmount = 1.0f - easingAmount;
			goSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, easingAmount));

		}

		return;

	}

	// �J�E���g�_�E�����n�߂�܂ł̃^�C�}�[���X�V�B
	if (!isCountDown_) {
		++countDownStartTimer_;
		if (COUNT_DOWN_TIMER < countDownStartTimer_) {

			isCountDown_ = true;

			// �J�E���g�_�E���Ɏg�p����ϐ����������B
			countDownEasingTimer_ = 0;
			isCountDownExit_ = false;
			countDownNumber_ = 3;
			countDownSprite_->ChangeTextureID(numFontHandle_[countDownNumber_], 0);

		}
	}

	// �J�E���g�_�E��
	if (isCountDown_) {

		// UI�𓮂����p�̃^�C�}�[���X�V�B
		countDownEasingTimer_ += COUNT_DOWN_EASING_TIMER;

		// �������I������B
		if (1.0f <= countDownEasingTimer_) {

			countDownEasingTimer_ = 0;

			// �ォ��^�񒆂ɗ��Ă���i�K��������A���̓A���t�@�l��������B
			if (!isCountDownExit_) {

				isCountDownExit_ = true;

			}
			// �A���t�@�l��������i�K�������玟�̐������J�E���g����B
			else {

				isCountDownExit_ = false;

				--countDownNumber_;
				// �J�E���g�_�E���̔ԍ���0�ɂȂ�����(�J�E���g�_�E�����I�������)
				if (countDownNumber_ < 1) {

					// �Q�[�����J�n����B
					isBeforeStart_ = false;

					// �e�ϐ����������B
					isCountDownExit_ = false;
					countDownSprite_->ChangePosition(WINDOW_CENTER);


				}
				else {

					countDownSprite_->ChangeTextureID(numFontHandle_[countDownNumber_], 0);

				}

			}

		}

		// UI���X�V����B
		if (isCountDownExit_) {

			// �C�[�W���O�ʂ����߂�B
			float easingAmount = FEasing::EaseInQuint(countDownEasingTimer_);

			// �A���t�@�l��������B
			countDownSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f - easingAmount));

		}
		else {

			// �C�[�W���O�ʂ����߂�B
			float easingAmount = FEasing::EaseOutQuint(countDownEasingTimer_);

			// �A���t�@�l���グ��B
			countDownSprite_->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, easingAmount));

			// �ړ�������B
			countDownSprite_->ChangePosition(COUNT_DOWN_START_POS + (WINDOW_CENTER - COUNT_DOWN_START_POS) * easingAmount);

		}

	}

}