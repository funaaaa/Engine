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


		// ボリュームノイズ用デバッグ情報
		ImGui::Begin("3DNoise");
		ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);

		//風の速度
		ImGui::DragFloat("WindSpeed", &RayEngine::Ins()->noiseConstData_.windSpeed_, 0.1f, 0.1f, 10.0f);

		//風の強度
		ImGui::DragFloat("WindStrength", &RayEngine::Ins()->noiseConstData_.windStrength_, 0.1f, 0.1f, 1.0f);

		//風のしきい値 ノイズを風として判断するためのもの
		ImGui::DragFloat("WindThreshold", &RayEngine::Ins()->noiseConstData_.threshold_, 0.01f, 0.01f, 1.0f);

		//ノイズのスケール
		ImGui::DragFloat("NoiseScale", &RayEngine::Ins()->noiseConstData_.scale_, 1.0f, 1.0f, 2000.0f);

		//ノイズのオクターブ数
		ImGui::DragInt("NoiseOctaves", &RayEngine::Ins()->noiseConstData_.octaves_, 1, 1, 10);

		//ノイズの持続度 違う周波数のノイズを計算する際にどのくらいノイズを持続させるか。 粒度になる。
		ImGui::DragFloat("NoisePersistance", &RayEngine::Ins()->noiseConstData_.persistence_, 0.01f, 0.01f, 1.0f);

		//ノイズの黒っぽさ
		ImGui::DragFloat("NoiseLacunarity", &RayEngine::Ins()->noiseConstData_.lacunarity_, 0.01f, 0.01f, 10.0f);


		ImGui::End();


		// ボリュームフォグ用デバッグ情報
		ImGui::Begin("VolumeFog");
		ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);

		//ボリュームテクスチャの座標
		std::array<float, 3> boxPos = { RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_.x_,RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_.y_, RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_.z_};
		ImGui::DragFloat3("Position", boxPos.data(), 0.1f);
		RayEngine::Ins()->constBufferData_.volumeTextureData_.pos_ = Vec3(boxPos[0], boxPos[1], boxPos[2]);

		//フォグの色
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

	// フリーカメラ状態だったら
	if (Engine::Ins()->isFreeCamera_) {
		Camera::Ins()->FreeCamera();
	}

}
