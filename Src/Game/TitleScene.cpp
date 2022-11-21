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
#include "MugenStage.h"
#include "BaseStage.h"
#include "RayComputeShader.h"
#include "RaytracingOutput.h"
#include "DriftParticleMgr.h"

TitleScene::TitleScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	title_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");
	titleOperation_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/titleOperation.png");

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
	tireMaskConstBuffer_->Generate(sizeof(Character::TireMaskUV) * 2, L"TireMaskUV");

	// �ݒ�
	DriftParticleMgr::Ins()->Setting();

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
	player_ = std::make_shared<Character>(Character::CHARA_ID::GHOST, 0, 0);

	// ��U�T�[�L�b�g�X�e�[�W��L��������B
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex());

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
	transitionCounter_ = 0;
	isExp = false;
	easingTimerUI_ = 0.0f;

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
		GameSceneMode::Ins()->level_ = level;

	}

	// TLAS��p�C�v���C�����X�V�B
	RayEngine::Ins()->Update();

	// �����̎���X�V�B
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// �v���C���[�̈ʒu�𒲐��B
	player_->Update(stages_[0], false, false);

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
		target = player_->GetPos();
		eye = target + cameraVec * cameraDistance_ + Vec3(0, cameraHeight_, 0);
		up = player_->GetUpVec();

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
		target = player_->GetPos();

		break;

	}
	case TitleScene::CAMERA_MODE::UP_CLOSE:
	{

		/*-- ���ڋ߃J������������ --*/

		const float CAMERA_FOCUS = 300.0f;
		const float CAMERA_DISTANCE = 25.0f;
		const float CAMERA_BEHIND_DISTANCE = 10.0f;

		Vec3 baseEye = player_->GetPos() + Vec3(0, 1, 0) * CAMERA_DISTANCE + player_->GetForwardVec() * CAMERA_BEHIND_DISTANCE;
		Vec3 baseTarget = player_->GetPos() + player_->GetForwardVec() * CAMERA_FOCUS;
		up = player_->GetUpVec();

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
		target = player_->GetPos();
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
			cameraPutBeforePos_ = player_->GetPos() + player_->GetForwardVec() * 1000.0f + Vec3(100, 100, 0);
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


}

void TitleScene::Draw()
{

	RayEngine::Ins()->Draw();

	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	title_.Draw();
	titleOperation_.Draw();

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

}