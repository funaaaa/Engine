#include "BaseScene.h"
#include "imgui.h"
#include "Engine.h"
#include "RayEngine.h"
#include "Camera.h"
#include "FHelper.h"

void BaseScene::ImGuiDebug(bool& IsMoveOnly1F, float& SunAngle)
{

	/*===== 基礎的なデバッグ機能 =====*/

	// ImGuiの表示非表示を切り替える。
	if (Input::Ins()->IsKey(DIK_O) && Input::Ins()->IsKeyTrigger(DIK_P)) {
		Engine::Ins()->isReservActivateImGui_ = !Engine::Ins()->isReservActivateImGui_;
	}

	// ImGuiが有効化されていたら。
	if (Engine::Ins()->isActivateImGui_) {

		// タイトルとウィンドウを設定
		ImGui::Begin("Menu");
		ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

		// FPSを表示
		std::string fps = "FPS : " + std::to_string(FHelper::FPS());
		ImGui::Text(fps.c_str());

		// 空白
		ImGui::Text("");

		// ゲーム一時停止フラグ
		ImGui::Checkbox("StopGame", &Engine::Ins()->isStopGame_);

		// ゲームの一時停止が解除されたらフリーカメラも解除する。
		if (!Engine::Ins()->isStopGame_) {

			Engine::Ins()->isFreeCamera_ = false;

		}

		// ゲーム一時停止中は1Fだけ動かすデバッグ機能を追加。
		if (Engine::Ins()->isStopGame_) {

			ImGui::SameLine();
			ImGui::Checkbox("MoveOnly1F", &IsMoveOnly1F);

		}

		bool prevFreeCamera = Engine::Ins()->isFreeCamera_;
		ImGui::Checkbox("FreeCamera", &Engine::Ins()->isFreeCamera_);

		// フリーカメラ中はゲームを一時停止する。
		if (Engine::Ins()->isFreeCamera_) {

			Engine::Ins()->isStopGame_ = true;

		}

		// 太陽の角度を設定。
		ImGui::DragFloat("SunAngle", &SunAngle, 0.01f);
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_ = Vec3(-cos(SunAngle), -sin(SunAngle), 0.5f);
		RayEngine::Ins()->GetConstBufferData().light_.dirLight_.lihgtDir_.Normalize();

		// メッシュのデバッグを行うか。
		bool isDebugMesh = static_cast<bool>(RayEngine::Ins()->GetConstBufferData().debug_.isDebugMesnInfo_);
		ImGui::Checkbox("DebugMesh", &isDebugMesh);
		RayEngine::Ins()->GetConstBufferData().debug_.isDebugMesnInfo_ = static_cast<int>(isDebugMesh);

		// ImGuiの書き込みを終了
		ImGui::End();

		// メッシュのデバッグメニューを別ウィンドウへ
		if (isDebugMesh) {

			ImGui::Begin("DebugMesh");
			ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);
			ImGui::DragInt("MaskX", &RayEngine::Ins()->GetConstBufferData().debug_.debugMeshMoveX_, 1);
			ImGui::End();

		}

	}

	// フリーカメラ状態だったら
	if (Engine::Ins()->isFreeCamera_) {
		Camera::Ins()->FreeCamera();
	}

}
