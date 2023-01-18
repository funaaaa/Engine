#include "TitleScene.h"
#include "Input.h"
#include "Pipeline.h"
#include "Engine.h"
#include "FHelper.h"
#include "GameSceneMode.h"
#include "PolygonInstanceRegister.h"
#include "RayEngine.h"
#include "Character.h"
#include "CharacterMgr.h"
#include "PolygonInstance.h"
#include "PolygonInstanceRegister.h"
#include "BLASRegister.h"
#include "BLAS.h"
#include "HitGroupMgr.h"
#include "MugenStage.h"
#include "BaseStage.h"
#include "RayComputeShader.h"
#include "RaytracingOutput.h"
#include "DriftParticleMgr.h"
#include "SceneTransition.h"

TitleScene::TitleScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	titleSpritePos_ = FHelper::WindowCenterPos();
	title_.GenerateForTexture(titleSpritePos_, FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");
	titleOperation_.GenerateForTexture(titleSpritePos_, FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/titleOperation.png");

	redSprite_.GenerateForTexture(titleSpritePos_, FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/red.png");
	redSprite_.SetColor(DirectX::XMFLOAT4(1, 1, 1, 0));

	levelSprite_[0].GenerateForTexture(FHelper::WindowCenterPos(), LEVEL_SPRITE_SCALE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/level1.png");
	levelSprite_[1].GenerateForTexture(FHelper::WindowCenterPos(), LEVEL_SPRITE_SCALE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/level2.png");
	levelSprite_[2].GenerateForTexture(FHelper::WindowCenterPos(), LEVEL_SPRITE_SCALE, Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/level3.png");


	// �X�e�[�W���Z�b�e�B���O����B
	stages_.emplace_back(std::make_shared<MugenStage>());

	// �^�C�����p�N���X���Z�b�g�B
	tireMaskTexture_ = std::make_shared<RaytracingOutput>();
	tireMaskTexture_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskTextureOutput_ = std::make_shared<RaytracingOutput>();
	tireMaskTextureOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskComputeShader_ = std::make_shared<RayComputeShader>();
	tireMaskComputeShader_->Setting(L"Resource/ShaderFiles/RayTracing/TireMaskComputeShader.hlsl", 0, 1, 1, { tireMaskTextureOutput_->GetUAVIndex() });
	tireMaskConstBuffer_ = std::make_shared<DynamicConstBuffer>();
	tireMaskConstBuffer_->Generate(sizeof(CharacterTireMask::TireMaskUV) * 2, L"TireMaskUV");

	// �ݒ�
	DriftParticleMgr::Ins()->Setting();
	logoTimer_ = 0;

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
	player_ = std::make_shared<CharacterMgr>();
	player_->AddChara(static_cast<int>(Character::CHARA_ID::GHOST), false, 0, 0);
	player_->Setting();

	// ��U�T�[�L�b�g�X�e�[�W��L��������B
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex(), false);

	// �ݒ�
	DriftParticleMgr::Ins()->Init();

	// TLAS�𐶐��B
	RayEngine::Ins()->SettingTLAS();

	// �J�����̏�����Ԃ�ݒ�B
	cameraMode_ = CAMERA_MODE::START;
	cameraTimer_ = 0;
	cameraAngle_ = 0.0f;
	cameraHeight_ = 0.0f;
	cameraDistance_ = 0.0f;
	sinTimer_ = 0.0f;
	transitionCounter_ = 0;
	nowSelectLevel_ = 0;
	redSpriteAlpha_ = 0.0f;
	redSpriteScaleRate_ = 0.0f;
	redSpriteExpSpan_ = 0;

	// ���z�Ɋւ���ϐ�
	sunAngle_ = 0.495f;
	sunSpeed_ = 0.0005f;

	isExp = false;
	easingTimerUI_ = 0.0f;
	titleStat_ = TITLE_STAT::TITLE;
	nextStat_ = TITLE_STAT::SELECT_LEVEL;
	transitionEasingTimer_ = 0;
	isExitSprite_ = false;
	isStatTransition_ = false;
	isFinishSceneTransition_ = false;
	isStartSceneTransition_ = false;
	isAppear_ = false;
	isReservationTransition_ = false;
	for (int index = 0; index < 3; ++index) {
		levelScaleRate_[index] = 0.5f;
		levelPos_[index] = Vec3(-1000, -1000, 0.1f);
	}
	titleSpritePos_ = FHelper::WindowCenterPos();

}

#include "Camera.h"
void TitleScene::Update()
{

	/*===== �X�V���� =====*/

	// ��{�I�ȃf�o�b�O�@�\
	bool isMoveOnly1F = false;
	ImGuiDebug(isMoveOnly1F, sunAngle_);

	// �Q�[���̈ꎞ��~�t���O�������Ă����珈�����Ƃ΂��B
	if (Engine::Ins()->isStopGame_ && !isMoveOnly1F) return;



	if (!isFinishSceneTransition_) {
		SceneTransition::Ins()->Exit();

		// ���S����莞�ԕ\����������B
		++logoTimer_;
		if (LOGO_TIMER <= logoTimer_) {
			isFinishSceneTransition_ = true;
		}
	}

	// ���͏���
	Input();

	// TLAS��p�C�v���C�����X�V�B
	RayEngine::Ins()->Update();

	// �����̎���X�V�B
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// �X�e�[�W���X�V�B
	stages_[STAGE_ID::MUGEN]->Update(player_);

	// ���z�̊p�x���X�V�B
	sunAngle_ += sunSpeed_;
	if (0.0f < RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_) {

		sunAngle_ += sunSpeed_ * 50.0f;

	}
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

	// �v���C���[�̈ʒu�𒲐��B
	player_->Update(stages_[0], false, false);

	// �J�����̍X�V����
	CameraUpdate();

	// �^�C�g���̃X�e�[�^�X���Ƃ̍X�V����
	UpdateTitleStat();

	const float PARAM_A_MIN = 2.4f;
	const float PARAM_B_MIN = 1.055f;

	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.x_ = PARAM_A_MIN;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lightColor_.y_ = PARAM_B_MIN;

}

void TitleScene::Draw()
{

	RayEngine::Ins()->Draw();

	// �^�C�������������ށB
	std::vector<CharacterTireMask::TireMaskUV> tireMaskUV;
	bool isWriteTireMask = player_->CheckTireMask(stages_[STAGE_ID::MUGEN], tireMaskUV);

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

	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	// �^�C�g���摜��`��
	title_.ChangePosition(Vec3(titleSpritePos_.x_, titleSpritePos_.y_, 0.1f));
	title_.Draw();
	titleOperation_.ChangePosition(Vec3(titleSpritePos_.x_, titleSpritePos_.y_, 0.1f));
	titleOperation_.Draw();


	// ���x���I����UI��`��
	for (int index = 0; index < 3; ++index) {
		levelSprite_[index].ChangePosition(levelPos_[index]);
		levelSprite_[index].Draw();
	}
	redSprite_.Draw();

	SceneTransition::Ins()->Draw();

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

}

void TitleScene::Input()
{

	/*===== ���͏��� =====*/

	switch (titleStat_)
	{
	case TitleScene::TITLE_STAT::TITLE:
	{

		/*-- �^�C�g���摜���o�Ă����� --*/

		if (!isStatTransition_ && (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_RETURN))) {

			isStatTransition_ = true;
			isAppear_ = false;
			nextStat_ = TITLE_STAT::SELECT_LEVEL;
			transitionEasingTimer_ = 0.0f;

		}

		break;

	}
	case TitleScene::TITLE_STAT::SELECT_LEVEL:
	{

		/*-- ���x���I���摜���o�Ă����� --*/

		// �������͂��ꂽ��B
		if (Input::Ins()->IsKeyTrigger(DIK_LEFT) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_DPAD_LEFT)) {

			--nowSelectLevel_;

		}
		// �E�����͂��ꂽ��B
		if (Input::Ins()->IsKeyTrigger(DIK_RIGHT) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_DPAD_RIGHT)) {

			++nowSelectLevel_;

		}

		// ���肳�ꂽ��B
		if (!isStatTransition_ && (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_RETURN))) {

			isStatTransition_ = true;
			isReservationTransition_ = true;
			isAppear_ = false;
			nextStat_ = TITLE_STAT::SELECT_LEVEL;
			transitionEasingTimer_ = 0.0f;

		}

		nowSelectLevel_ = static_cast<int>(FHelper::Clamp(static_cast<float>(nowSelectLevel_), LEVEL_RANGE.x_, LEVEL_RANGE.y_));

		break;

	}
	default:
		break;
	}
}

void TitleScene::CameraUpdate()
{

	/*===== �J�����̍X�V���� =====*/

	if (Engine::Ins()->isStopGame_) return;

	// �J�����̏��B
	Vec3& eye = Camera::Ins()->eye_;
	Vec3& target = Camera::Ins()->target_;
	Vec3& up = Camera::Ins()->up_;

	// �J�����̏�Ԃɂ���ď�����ς���B
	switch (cameraMode_)
	{
	case TitleScene::CAMERA_MODE::START:
	{

		/*-- �J�n���p�J������������ --*/

		// �������グ�ĉ�]������B
		const float MAX_HEIGHT = 200.0f;
		const float ADD_HEIGHT = MAX_HEIGHT / CAMERA_TIMER[static_cast<int>(TitleScene::CAMERA_MODE::START)];
		const float ADD_ANGLE = 0.03f;
		const float MAX_DISTANCE = 200.0f;
		const float ADD_DISTANCE = MAX_DISTANCE / CAMERA_TIMER[static_cast<int>(TitleScene::CAMERA_MODE::START)];;
		cameraHeight_ = FHelper::Clamp(cameraHeight_ + ADD_HEIGHT, 0.0f, MAX_HEIGHT);
		cameraAngle_ += ADD_ANGLE;
		cameraDistance_ = FHelper::Clamp(cameraDistance_ + ADD_DISTANCE, 0.0f, MAX_DISTANCE);

		// �p�x���x�N�g���ɒ����B
		Vec3 cameraVec = Vec3(cosf(cameraAngle_), 0.0f, sinf(cameraAngle_));

		// �J�����̈ʒu�����߂�B
		target = player_->GetPlayerIns().lock()->GetPos();
		eye = target + cameraVec * cameraDistance_ + Vec3(0, cameraHeight_, 0);
		up = player_->GetPlayerIns().lock()->GetUpVec();

		break;

	}
	case TitleScene::CAMERA_MODE::PUT_BEFORE:
	{

		/*-- �O�ݒu�p�J������������ --*/

		float divCount = 10.0f;
		if (transitionCounter_ == 1) {
			divCount = 30.0f;
		}

		eye += (cameraPutBeforePos_ - eye) / divCount;
		target = player_->GetPlayerIns().lock()->GetPos();

		break;

	}
	case TitleScene::CAMERA_MODE::UP_CLOSE:
	{

		/*-- ���ڋ߃J������������ --*/

		const float CAMERA_FOCUS = 300.0f;
		const float CAMERA_DISTANCE = 25.0f;
		const float CAMERA_BEHIND_DISTANCE = 10.0f;

		Vec3 baseEye = player_->GetPlayerIns().lock()->GetPos() + Vec3(0, 1, 0) * CAMERA_DISTANCE + player_->GetPlayerIns().lock()->GetForwardVec() * CAMERA_BEHIND_DISTANCE;
		Vec3 baseTarget = player_->GetPlayerIns().lock()->GetPos() + player_->GetPlayerIns().lock()->GetForwardVec() * CAMERA_FOCUS;
		up = player_->GetPlayerIns().lock()->GetUpVec();

		// ���W���Ԃ���B
		eye += (baseEye - eye) / 10.0f;
		target += (baseTarget - target) / 10.0f;

		break;

	}
	case TitleScene::CAMERA_MODE::FROM_FRONT:
	{

		/*-- �O�ǔ��p�J������������ --*/

		const Vec3 CAMERA_DIR = Vec3(2.5f, 1.0f, 0).GetNormal();

		// �J�����̊e�X�e�[�^�X��ݒ�B
		target = player_->GetPlayerIns().lock()->GetPos();
		float nowDistance = UPCLOSE_DISTANCE_MIN + (UPCLOSE_DISTANCE_MAX - UPCLOSE_DISTANCE_MIN) * (static_cast<float>(cameraTimer_) / CAMERA_TIMER[static_cast<int>(cameraMode_)]);
		Vec3 baseEye = target + CAMERA_DIR * nowDistance;
		eye += (baseEye - eye) / 10.0f;
		up = Vec3(0, 1, 0);

		break;

	}
	default:
		break;
	}

	// �J�����̃^�C�}�[���X�V���ăJ�����̏�Ԃ�ς���B
	++cameraTimer_;
	if (CAMERA_TIMER[static_cast<int>(cameraMode_)] <= cameraTimer_) {

		// �����ŃJ������ς���B
		if (cameraMode_ == CAMERA_MODE::START) {
			cameraMode_ = CAMERA_MODE::PUT_BEFORE;
		}
		else {

			// �O�t���[���Ɠ����ɂȂ�Ȃ��悤�ɂ���B
			while (1) {
				CAMERA_MODE randomMode = static_cast<CAMERA_MODE>(FHelper::GetRand(1, 3));
				if (randomMode != cameraMode_) {
					cameraMode_ = randomMode;
					break;
				}
			}
		}

		// �J�����p�ϐ����������B
		cameraHeight_ = 0.0f;
		cameraAngle_ = 0.0f;
		cameraDistance_ = 0.0f;
		cameraTimer_ = 0;
		++transitionCounter_;

		// ���[�h���Ƃ̏���������
		switch (cameraMode_)
		{
		case TitleScene::CAMERA_MODE::PUT_BEFORE:
			cameraPutBeforePos_ = player_->GetPlayerIns().lock()->GetPos() + player_->GetPlayerIns().lock()->GetForwardVec() * 1000.0f + Vec3(100, 100, 0);
			break;
		case TitleScene::CAMERA_MODE::UP_CLOSE:
			cameraDistance_ = UPCLOSE_DISTANCE_MIN;
			break;
		case TitleScene::CAMERA_MODE::FROM_FRONT:
			break;
		default:
			break;
		}

	}

	Camera::Ins()->GenerateMatView();

}

void TitleScene::UpdateTitleStat()
{

	/*===== �^�C�g���̃X�e�[�^�X���Ƃ̍X�V���� =====*/

	switch (titleStat_)
	{
	case TitleScene::TITLE_STAT::TITLE:
	{

		/*-- �^�C�g���摜��`�� --*/

		// ������@��UI�̍X�V����
		easingTimerUI_ += ADD_EASING_TIMER_UI;
		if (1.0f < easingTimerUI_) {
			easingTimerUI_ = 0.0f;
			isExp = isExp ? false : true;
		}

		// �C�[�W���O�ŃA���t�@��ς���B
		float easingValue = 0;
		if (isExp) {
			easingValue = FEasing::EaseOutQuint(easingTimerUI_);
		}
		else {
			easingValue = 1.0f - FEasing::EaseInQuint(easingTimerUI_);
		}
		titleOperation_.SetColor(DirectX::XMFLOAT4(1, 1, 1, easingValue));

		// �J�ڒ���������B
		if (isStatTransition_) {

			// �^�C�}�[���X�V���ăC�[�W���O������B
			transitionEasingTimer_ += TRANSITION_EASING_ADD;
			if (1.0f < transitionEasingTimer_) {

				// �o������������
				if (isAppear_) {
					isStatTransition_ = false;
				}
				else {
					titleStat_ = nextStat_;
					isAppear_ = true;
				}

				transitionEasingTimer_ = 0.0f;

			}
			else {

				// �o������������B
				if (isAppear_) {
					float easingAmount = FEasing::EaseOutQuint(transitionEasingTimer_);
					titleSpritePos_ = FHelper::WindowCenterPos() + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
				}
				else {
					float easingAmount = FEasing::EaseInQuint(transitionEasingTimer_);
					titleSpritePos_ = FHelper::WindowCenterPos() + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
				}

			}

		}

		break;

	}
	case TitleScene::TITLE_STAT::SELECT_LEVEL:
	{

		/*-- ���x����I�������ʁB --*/

		const float NOT_SELECTED_IMAGE_SIZE_RATE = 0.5f;
		const float SELECTED_IMAGE_SIZE_RATE = 1.0f;

		// �T�C���g�őI������Ă��郌�x����UI���g�k������悤�^�C�}�[
		sinTimer_ += ADD_SIN_TIMER;
		float sinAmount = sinf(sinTimer_);
		float sinExpRate = sinAmount * SIN_SCALE_RATE;


		// �I������Ă���摜��傫�����āA�I������Ă��Ȃ��摜���������Â�����B
		for (int index = 0; index < 3; ++index) {

			// �I������Ă�����B
			if (nowSelectLevel_ == index) {
				levelScaleRate_[index] += ((sinExpRate + SELECTED_IMAGE_SIZE_RATE) - levelScaleRate_[index]) / 3.0f;
			}
			else {
				levelScaleRate_[index] += (NOT_SELECTED_IMAGE_SIZE_RATE - levelScaleRate_[index]) / 3.0f;
			}
			levelSprite_[index].ChangeScale(LEVEL_SPRITE_SCALE.x_ * levelScaleRate_[index], LEVEL_SPRITE_SCALE.y_ * levelScaleRate_[index], 1.0f);
			levelSprite_[index].SetColor(DirectX::XMFLOAT4(levelScaleRate_[index], levelScaleRate_[index], levelScaleRate_[index], levelScaleRate_[index]));

		}

		// ���x����UI�̂���̐Ԃ�����X�V����B
		redSprite_.ChangePosition(levelSprite_[nowSelectLevel_].GetPos());

		// ���Ԋu�ŐԂ�����o���悤�ɂ���B
		const int RED_SPRITE_EXP_SPAN = 12;
		++redSpriteExpSpan_;
		if (RED_SPRITE_EXP_SPAN <= redSpriteExpSpan_) {
			redSpriteExpSpan_ = 0;
			redSpriteAlpha_ = 1.0f;
			redSpriteScaleRate_ = 0.0f;
		}

		// �Ԃ���̃T�C�Y���A���t�@���X�V�B
		const float SUB_REDSPRITE_ALPHA = 0.09f;
		const float SUB_REDSPRITE_SCALE_RATE = 0.09f;
		redSpriteAlpha_ = FHelper::Clamp(redSpriteAlpha_ - SUB_REDSPRITE_ALPHA, 0.0f, 1.0f);
		redSpriteScaleRate_ = FHelper::Clamp(redSpriteScaleRate_ + SUB_REDSPRITE_SCALE_RATE, 0.0f, 1.0f);
		redSprite_.SetColor(DirectX::XMFLOAT4(1, 1, 1, redSpriteAlpha_));
		redSprite_.ChangeScale(Vec3((LEVEL_SPRITE_SCALE * 1.5f).x_ * redSpriteScaleRate_, (LEVEL_SPRITE_SCALE * 1.5f).y_ * redSpriteScaleRate_, 1.0f));



		// �J�ڒ���������B
		if (isStatTransition_) {

			// �^�C�}�[���X�V���ăC�[�W���O������B
			transitionEasingTimer_ += TRANSITION_EASING_ADD;
			if (1.0f < transitionEasingTimer_) {

				// �o������������
				if (isAppear_) {
					isStatTransition_ = false;
					transitionEasingTimer_ = 0.0f;
				}
				else {

					// �V�[���J�ڂ��\�񂳂�Ă�����J�ڂ���B
					if (isReservationTransition_) {

						// �J�ډ��o���n�܂��Ă��Ȃ������牉�o������B
						if (!isStartSceneTransition_) {
							SceneTransition::Ins()->Appear();
							isStartSceneTransition_ = true;
						}

						// �J�ډ��o���I����Ă����玟�ցB
						if (isStartSceneTransition_ && SceneTransition::Ins()->GetIsFinish()) {

							isTransition_ = true;
							GameSceneMode::Ins()->level_ = nowSelectLevel_;

						}

					}
					else {
						transitionEasingTimer_ = 0.0f;
						titleStat_ = nextStat_;
						isAppear_ = true;
					}

				}

			}
			else {

				// �o������������B
				if (isAppear_) {
					float easingAmount = FEasing::EaseOutQuint(transitionEasingTimer_);
					levelPos_[0] = LEVEL_POS[0] + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
					levelPos_[1] = LEVEL_POS[1] + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
					levelPos_[2] = LEVEL_POS[2] + Vec3(0, SPRITE_EXIT_POS_Y - (SPRITE_EXIT_POS_Y * easingAmount), 0);
				}
				else {
					float easingAmount = FEasing::EaseInQuint(transitionEasingTimer_);
					levelPos_[0] = LEVEL_POS[0] + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
					levelPos_[1] = LEVEL_POS[1] + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
					levelPos_[2] = LEVEL_POS[2] + Vec3(0, SPRITE_EXIT_POS_Y * easingAmount, 0);
				}

			}

		}

		break;

	}
	default:
		break;
	}

}
