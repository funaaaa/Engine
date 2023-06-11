#include "BaseScene.h"
#include "imgui.h"
#include "Engine.h"
#include "RayEngine.h"
#include "Camera.h"
#include "FHelper.h"

void BaseScene::ImGuiDebug(bool& IsMoveOnly1F, float& SunAngle)
{

	/*===== ��b�I�ȃf�o�b�O�@�\ =====*/

	// ImGui�̕\����\����؂�ւ���B
	if (Input::Ins()->IsKey(DIK_O) && Input::Ins()->IsKeyTrigger(DIK_P)) {
		Engine::Ins()->isReservActivateImGui_ = !Engine::Ins()->isReservActivateImGui_;
	}

	// ImGui���L��������Ă�����B
	if (Engine::Ins()->isActivateImGui_) {

		// �^�C�g���ƃE�B���h�E��ݒ�
		ImGui::Begin("Menu");
		ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

		// FPS��\��
		std::string fps = "FPS : " + std::to_string(FHelper::FPS());
		ImGui::Text(fps.c_str());

		// ��
		ImGui::Text("");

		// �Q�[���ꎞ��~�t���O
		ImGui::Checkbox("StopGame", &Engine::Ins()->isStopGame_);

		// �Q�[���̈ꎞ��~���������ꂽ��t���[�J��������������B
		if (!Engine::Ins()->isStopGame_) {

			Engine::Ins()->isFreeCamera_ = false;

		}

		// �Q�[���ꎞ��~����1F�����������f�o�b�O�@�\��ǉ��B
		if (Engine::Ins()->isStopGame_) {

			ImGui::SameLine();
			ImGui::Checkbox("MoveOnly1F", &IsMoveOnly1F);

		}

		bool prevFreeCamera = Engine::Ins()->isFreeCamera_;
		ImGui::Checkbox("FreeCamera", &Engine::Ins()->isFreeCamera_);

		// �t���[�J�������̓Q�[�����ꎞ��~����B
		if (Engine::Ins()->isFreeCamera_) {

			Engine::Ins()->isStopGame_ = true;

		}

		// ���z�̊p�x��ݒ�B
		ImGui::DragFloat("SunAngle", &SunAngle, 0.01f);
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(SunAngle), -sin(SunAngle), 0.5f);
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

		// ���b�V���̃f�o�b�O���s�����B
		bool isDebugMesh = static_cast<bool>(RayEngine::Ins()->GetConstBufferData().debug_.isDebugMesnInfo_);
		ImGui::Checkbox("DebugMesh", &isDebugMesh);
		RayEngine::Ins()->GetConstBufferData().debug_.isDebugMesnInfo_ = static_cast<int>(isDebugMesh);

		// ImGui�̏������݂��I��
		ImGui::End();

		// ���b�V���̃f�o�b�O���j���[��ʃE�B���h�E��
		if (isDebugMesh) {

			ImGui::Begin("DebugMesh");
			ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);
			ImGui::DragInt("MaskX", &RayEngine::Ins()->GetConstBufferData().debug_.debugMeshMoveX_, 1);
			ImGui::End();

		}


		// �{�����[���m�C�Y�p�f�o�b�O���
		ImGui::Begin("3DNoise");
		ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);

		//���̑��x
		ImGui::DragFloat("WindSpeed", &RayEngine::Ins()->noiseConstData_.windSpeed_, 0.1f, 0.1f, 10.0f);

		//���̋��x
		ImGui::DragFloat("WindStrength", &RayEngine::Ins()->noiseConstData_.windStrength_, 0.1f, 0.1f, 1.0f);

		//���̂������l �m�C�Y�𕗂Ƃ��Ĕ��f���邽�߂̂���
		ImGui::DragFloat("WindThreshold", &RayEngine::Ins()->noiseConstData_.threshold_, 0.01f, 0.01f, 1.0f);

		//�m�C�Y�̃X�P�[��
		ImGui::DragFloat("NoiseScale", &RayEngine::Ins()->noiseConstData_.scale_, 1.0f, 1.0f, 2000.0f);

		//�m�C�Y�̃I�N�^�[�u��
		ImGui::DragInt("NoiseOctaves", &RayEngine::Ins()->noiseConstData_.octaves_, 1, 1, 10);

		//�m�C�Y�̎����x �Ⴄ���g���̃m�C�Y���v�Z����ۂɂǂ̂��炢�m�C�Y�����������邩�B ���x�ɂȂ�B
		ImGui::DragFloat("NoisePersistance", &RayEngine::Ins()->noiseConstData_.persistence_, 0.01f, 0.01f, 1.0f);

		//�m�C�Y�̍����ۂ�
		ImGui::DragFloat("NoiseLacunarity", &RayEngine::Ins()->noiseConstData_.lacunarity_, 0.01f, 0.01f, 10.0f);


		ImGui::End();


		// �{�����[���t�H�O�p�f�o�b�O���
		ImGui::Begin("VolumeFog");
		ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);

		//�{�����[���e�N�X�`���̍��W
		std::array<float, 3> boxPos = { RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_.x_,RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_.y_, RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_.z_};
		ImGui::DragFloat3("Position", boxPos.data(), 0.1f);
		RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_ = Vec3(boxPos[0], boxPos[1], boxPos[2]);

		//�t�H�O�̐F
		std::array<float, 3> fogColor = { RayEngine::Ins()->constBufferData_.volumeTextureData_.color_.x_,RayEngine::Ins()->constBufferData_.volumeTextureData_.color_.y_, RayEngine::Ins()->constBufferData_.volumeTextureData_.color_.z_ };
		ImGui::DragFloat3("FogColor", fogColor.data(), 0.001f, 0.001f, 1.0f);
		RayEngine::Ins()->constBufferData_.volumeTextureData_.color_ = Vec3(fogColor[0], fogColor[1], fogColor[2]);

		ImGui::DragFloat("WrapCount", &RayEngine::Ins()->constBufferData_.volumeTextureData_.wrapCount_, 1.0f, 1.0f, 100.0f);

		ImGui::DragFloat("GridSize", &RayEngine::Ins()->constBufferData_.volumeTextureData_.gridSize_, 0.1f, 0.1f, 1000.0f);

		ImGui::DragFloat("SamplingLength", &RayEngine::Ins()->constBufferData_.volumeTextureData_.samplingLength_, 0.1f, 1.0f, 1000.0f);

		ImGui::DragFloat("MaxSamplingCount", &RayEngine::Ins()->constBufferData_.volumeTextureData_.sanplingMaxCount_, 1.0f, 1.0f, 1000.0f);

		ImGui::DragFloat("Density", &RayEngine::Ins()->constBufferData_.volumeTextureData_.density_, 0.01f, 0.0f, 10.0f);


		ImGui::End();




	}

	// �t���[�J������Ԃ�������
	if (Engine::Ins()->isFreeCamera_) {
		Camera::Ins()->FreeCamera();
	}

}
