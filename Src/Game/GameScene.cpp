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
#include "CircuitStage.h"
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

#include "RayEngine.h"

#include "GLTF.h"

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
	tireMaskConstBuffer_->Generate(sizeof(Character::TireMaskUV) * 2, L"TireMaskUV");

	// ���h��R���s���[�g�V�F�[�_�[
	whiteOutComputeShader_ = std::make_shared<RayComputeShader>();
	whiteOutComputeShader_->Setting(L"Resource/ShaderFiles/WhiteMakeUpShader.hlsl", 0, 0, 0, {});

	characterMgr_ = std::make_shared<CharacterMgr>();

	// Instance�̃��[���h�s��𐶐��B
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// �ݒ�
	DriftParticleMgr::Ins()->Setting();

	// ���z�Ɋւ���ϐ�
	sunAngle_ = 0.1f;
	sunSpeed_ = 0.01f;

	isDisplayFPS_ = false;

	nextScene_ = SCENE_ID::RESULT;
	isTransition_ = false;

	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;



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
	//nowRapCountSprite_ = std::make_shared<Sprite>();
	//nowRapCountSprite_->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), Vec2(16.0f, 32.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	//maxRapCountSprite_ = std::make_shared<Sprite>();
	//maxRapCountSprite_->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), Vec2(16.0f, 32.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	//rapSlashSprite_ = std::make_shared<Sprite>();
	//rapSlashSprite_->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), Vec2(16.0f, 32.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	coinUI_ = std::make_shared<Sprite>();
	coinUI_->GenerateForTexture(Vec3(140, 647, 0.1f), Vec2(192 / 2.0f, 64 / 2.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/coinUI.dds");
	rapUI_ = std::make_shared<Sprite>();
	rapUI_->GenerateForTexture(Vec3(327, 647, 0.1f), Vec2(224 / 2.0f, 64 / 2.0f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Game/UI/rapUI.dds");

	coinCountUI_[0] = std::make_shared<Sprite>();
	coinCountUI_[0]->GenerateSpecifyTextureID(Vec3(145, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[0]);
	coinCountUI_[1] = std::make_shared<Sprite>();
	coinCountUI_[1]->GenerateSpecifyTextureID(Vec3(178, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[0]);

	nowRapCountUI_ = std::make_shared<Sprite>();
	nowRapCountUI_->GenerateSpecifyTextureID(Vec3(321, 647, 0.1f), Vec2(16.0f * 0.8f, 32.0f * 0.8f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[1]);
	slashUI_ = std::make_shared<Sprite>();
	slashUI_->GenerateSpecifyTextureID(Vec3(356, 651, 0.1f), Vec2(16.0f * 0.6f, 32.0f * 0.6f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[10]);
	maxRapCountUI_ = std::make_shared<Sprite>();
	maxRapCountUI_->GenerateSpecifyTextureID(Vec3(381, 651, 0.1f), Vec2(16.0f * 0.5f, 32.0f * 0.5f), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, numFontHandle_[3]);

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

	nextScene_ = SCENE_ID::RESULT;
	isTransition_ = false;

	concentrationLine_->Init();

	countDownSprite_->ChangePosition(Vec3(100000, 10000, 100));
	goSprite_->ChangePosition(Vec3(100000, 10000, 100));

	characterMgr_ = std::make_shared<CharacterMgr>();

	if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::AI) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// AI�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false, GameSceneMode::Ins()->level_);

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

	// ��U�T�[�L�b�g�X�e�[�W��L��������B
	stages_[STAGE_ID::MUGEN]->Setting(tireMaskTexture_->GetUAVIndex());


	// TLAS�𐶐��B
	RayEngine::Ins()->SettingTLAS();


	Camera::Ins()->Init();

	DriftParticleMgr::Ins()->Init();

	isBeforeStart_ = true;
	isCountDown_ = false;
	countDownEasingTimer_ = 0;
	isCountDownExit_ = false;
	isGameFinish_ = false;
	countDownStartTimer_ = 0;
	transitionTimer = 0;
	countDownNumber_ = 2;
	sunAngle_ = 0.8f;

}

void GameScene::Update()
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input();

	// �E�B���h�E�̖��O���X�V�B
	//if (true) {

	FPS();

	//}
	//else {

	//SetWindowText(Engine::Ins()->windowsAPI_->hwnd_, L"LE3A_20_�t�i�N���x_�^�N�~");

	//}

	// �L�������X�V�B
	characterMgr_->Update(stages_[STAGE_ID::MUGEN], isBeforeStart_, isGameFinish_);

	// �����̎���X�V�B
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.seed_ = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update(characterMgr_->GetPlayerIns().lock()->GetPos(), characterMgr_->GetPlayerIns().lock()->GetCameraForwardVec(), characterMgr_->GetPlayerIns().lock()->GetUpVec(), characterMgr_->GetPlayerIns().lock()->GetNowSpeedPer(), isBeforeStart_, isGameFinish_);

	// �����ꂩ�̃L�������S�[�����Ă����烊�U���g�V�[���Ɉړ�����B
	if (characterMgr_->CheckGoal()) {


		isGameFinish_ = true;

		++transitionTimer;
		if (TRANSION_TIME < transitionTimer) {

			isTransition_ = true;

			characterMgr_->Init();

			return;

		}

		//isTransition_ = true;

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
	stages_[STAGE_ID::MUGEN]->Update();

	// �S�[���̕\����\����؂�ւ��B
	if (characterMgr_->GetPlayerIns().lock()->GetIsPassedMiddlePoint()) {
		stages_[STAGE_ID::MUGEN]->DisplayGoal();
	}
	else {
		stages_[STAGE_ID::MUGEN]->NonDisplayGoal();
	}

	// TLAS��p�C�v���C�����X�V�B
	RayEngine::Ins()->Update();

	// ���z�̊p�x���X�V�B
	//sunAngle_ = 0.8f;
	//if (0.0f < RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.y_) {

	//	sunAngle_ += sunSpeed_;
	//	sunAngle_ += sunSpeed_;
	//	sunAngle_ += sunSpeed_;

	//}
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.isActive_ = true;
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();
	// �V�����̂���]������B
	//skyDomeIns_.lock()->AddRotate(Vec3(0.001f, 0, 0));

	// �b�����X�V�B
	ShellObjectMgr::Ins()->Update(stages_[STAGE_ID::MUGEN]);

	// �����X�V����B
	DriftParticleMgr::Ins()->Update();

	// �W�������X�V�B
	if (characterMgr_->GetPlayerIns().lock()->GetIdConcentrationLine()) {
		concentrationLine_->Generate();
	}
	concentrationLine_->Update();

}

void GameScene::Draw()
{

	/*===== �`�揈�� =====*/

	// ���C�g���[�V���O�����s�B
	RayEngine::Ins()->Draw();


	// ���𔒓h��
	static int a = 0;
	if (Input::Ins()->IsKeyTrigger(DIK_R) || a == 0) {

		whiteOutComputeShader_->Dispatch(4096 / 32, 4096 / 32, 1, tireMaskTexture_->GetUAVIndex());
		++a;

	}

	// �^�C�������������ށB
	std::vector<Character::TireMaskUV> tireMaskUV;
	bool isWriteTireMask = characterMgr_->CheckTireMask(stages_[STAGE_ID::MUGEN], tireMaskUV);

	if (isWriteTireMask) {

		// UAV���������ށB
		tireMaskConstBuffer_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), tireMaskUV.data(), sizeof(Character::TireMaskUV) * 2);
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
	static int firstTime = 0;
	if (firstTime != 0) {

		UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

		concentrationLine_->Draw();

		// �R�C���̎擾����ui�B
		coinCountUI_[0]->Draw();
		coinCountUI_[1]->Draw();

		// ���݂̃��b�v����ui�B
		nowRapCountUI_->Draw();
		slashUI_->Draw();
		maxRapCountUI_->Draw();

		// ������ui�̃t���[���B
		coinUI_->Draw();
		rapUI_->Draw();

		// �J�E���g�_�E���p��ui�B
		countDownSprite_->Draw();

		// �J�E���g�_�E���I������go��ui�B
		goSprite_->Draw();

		resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

	}
	if (firstTime == 0) ++firstTime;

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

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

	InputImGUI();

}

#include "BaseItem.h"
void GameScene::InputImGUI()
{

	/*===== IMGUI�X�V =====*/

	ImGui::Text("Let's do three laps!");

	ImGui::DragFloat("SunAngle", &sunAngle_, 0.005f);
	bool isMip = RayEngine::Ins()->constBufferData_.light_.pointLight_[0].pad_.x;
	ImGui::Checkbox("MipFlag", &isMip);
	RayEngine::Ins()->constBufferData_.light_.pointLight_[0].pad_.x = isMip;

	//// ���z�̈ړ����x���X�V�B
	//ImGui::SliderFloat("Sun Speed", &sunSpeed_, 0.0f, 0.1f, "%.5f");

	//// ���b�V����\������B
	//bool isMesh = constBufferData_.debug_.isMeshScene_;
	//ImGui::Checkbox("Mesh Scene", &isMesh);
	//constBufferData_.debug_.isMeshScene_ = isMesh;

	//// �@����\������B
	//bool isNormal = constBufferData_.debug_.isNormalScene_;
	//ImGui::Checkbox("Normal Scene", &isNormal);
	//constBufferData_.debug_.isNormalScene_ = isNormal;

	//// ���C�g�����������ʂ����\������t���O���X�V�B
	//bool isLightHit = constBufferData_.debug_.isLightHitScene_;
	//ImGui::Checkbox("LightHit Scene", &isLightHit);
	//constBufferData_.debug_.isLightHitScene_ = isLightHit;

	//// �f�o�b�O�p�Ńm�C�Y��ʂ��o�����߂̃t���O���Z�b�g�B
	//bool isNoise = constBufferData_.debug_.isNoiseScene_;
	//ImGui::Checkbox("Noise Scene", &isNoise);
	//constBufferData_.debug_.isNoiseScene_ = isNoise;

	//// AO���s�����̃t���O���Z�b�g�B
	//bool isNoAO_ = constBufferData_.debug_.isNoAO_;
	//ImGui::Checkbox("NoAO Scene", &isNoAO_);
	//constBufferData_.debug_.isNoAO_ = isNoAO_;

	//// GI���s�����̃t���O���Z�b�g�B
	//bool isNoGI_ = constBufferData_.debug_.isNoGI_;
	//ImGui::Checkbox("NoGI Scene", &isNoGI_);
	//constBufferData_.debug_.isNoGI_ = isNoGI_;

	//// GI�݂̂�`�悷�邩�̃t���O���Z�b�g�B
	//bool isGIOnlyScene_ = constBufferData_.debug_.isGIOnlyScene_;
	//ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene_);
	//constBufferData_.debug_.isGIOnlyScene_ = isGIOnlyScene_;

	//// FPS��\�����邩�̃t���O���Z�b�g�B
	//ImGui::Checkbox("Display FPS", &isDisplayFPS_);

	//// �A�C�e���f�o�b�O�p�B
	//bool haveItem = characterMgr_->GetPlayerIns().lock()->item_.operator bool();

	//if (haveItem) {

	//	bool haveBoostItem = characterMgr_->GetPlayerIns().lock()->item_->GetItemID() == BaseItem::ItemID::BOOST;

	//	ImGui::Checkbox("BoostItem", &haveBoostItem);

	//	bool haveShellItem = characterMgr_->GetPlayerIns().lock()->item_->GetItemID() == BaseItem::ItemID::SHELL;

	//	ImGui::Checkbox("ShellItem", &haveShellItem);

	//}

	//Vec3 position = itemFrameUI_->GetPos();
	//float pos[3] = { position.x_, position.y_, position.z_ };

	//ImGui::DragFloat3("Position", pos, 1.0f);

	//position = Vec3(pos[0], pos[1], pos[2]);
	//itemFrameUI_->ChangePosition(position);



	//int index = 37;

	//Vec3 pos = PolygonInstanceRegister::Ins()->GetPos(index);

	//float posArray[3] = { pos.x_, pos.y_, pos.z_ };

	//ImGui::DragFloat3("Pos", posArray, 1.0f);

	//pos.x_ = posArray[0];
	//pos.y_ = posArray[1];
	//pos.z_ = posArray[2];

	//PolygonInstanceRegister::Ins()->ChangeTrans(index, pos);


	//Vec3 rotate = PolygonInstanceRegister::Ins()->GetRotateVec3(index);

	//float rotateArray[3] = { rotate.x_, rotate.y_, rotate.z_ };

	//ImGui::DragFloat3("Rotate", rotateArray, 0.001f);

	//rotate.x_ = rotateArray[0];
	//rotate.y_ = rotateArray[1];
	//rotate.z_ = rotateArray[2];

	//PolygonInstanceRegister::Ins()->ChangeRotate(index, rotate);


	//DirectX::XMMATRIX scale = PolygonInstanceRegister::Ins()->GetScale(index);

	//float scaleArray[3] = { scale.r[0].m128_f32[0], scale.r[1].m128_f32[1], scale.r[2].m128_f32[2] };

	//ImGui::DragFloat3("Scale", scaleArray, 1.0f);

	//Vec3 scaleVec3;
	//scaleVec3.x_ = scaleArray[0];
	//scaleVec3.y_ = scaleArray[1];
	//scaleVec3.z_ = scaleArray[2];

	//PolygonInstanceRegister::Ins()->ChangeScale(index, scaleVec3);



	//// �f�o�b�O�p
	//if (Input::Ins()->IsKey(DIK_U)) {

	//	PolygonInstanceRegister::Ins()->ChangeTrans(index, characterMgr_->GetPlayerIns().lock()->GetPos());

	//}

	// �}�e���A���̒l������������B
	//ImGui::Text("MaterialData");
	//ImGui::SliderFloat("Metalness", &pbrSphereBlas_.lock()->GetMaterial().metalness_, 0.0f, 1.0f);
	//ImGui::SliderFloat("Specular", &pbrSphereBlas_.lock()->GetMaterial().specular_, 0.0f, 1.0f);
	//ImGui::SliderFloat("Roughness", &pbrSphereBlas_.lock()->GetMaterial().roughness_, 0.0f, 1.0f);
	//RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

	//pbrSphereBlas_.lock()->IsChangeMaterial();


}

void GameScene::GenerateGimmick()
{

	//// 1��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 2��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 3��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 4��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 5��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

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