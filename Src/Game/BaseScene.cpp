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

	}

	// �t���[�J������Ԃ�������
	if (Engine::Ins()->isFreeCamera_) {
		Camera::Ins()->FreeCamera();
	}

}
