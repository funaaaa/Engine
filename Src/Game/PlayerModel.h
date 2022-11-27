#pragma once
#include <vector>
#include <memory>

class PolygonMeshInstance;
class BLAS;

class PlayerModel {

public:

	/*===== メンバ変数 =====*/

	// 車体
	std::weak_ptr<BLAS> carBodyBlas_;		// 車のモデルのBLASのインデックス
	std::weak_ptr<PolygonMeshInstance> carBodyInstance_;		// 車のモデルのインスタンスのインデックス
	// バンパー
	std::weak_ptr<BLAS> carBumperBlas_;
	std::weak_ptr<PolygonMeshInstance> carBumperInstance_;
	// ライト
	std::weak_ptr<BLAS> carLeftLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftLightInstance_;
	// ライト
	std::weak_ptr<BLAS> carRightLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightLightInstance_;
	// ライト
	std::weak_ptr<BLAS> carFrontLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carFrontLightInstance_;
	// 鏡
	std::weak_ptr<BLAS> carMirrorBlas_;
	std::weak_ptr<PolygonMeshInstance> carMirrorInstance_;
	// 鏡のカバー
	std::weak_ptr<BLAS> carMirrorCoverBlas_;
	std::weak_ptr<PolygonMeshInstance> carMirrorCoverInstance_;
	// 窓
	std::weak_ptr<BLAS> carWindowBlas_;
	std::weak_ptr<PolygonMeshInstance> carWindowInsInstance_;
	// 後ろタイヤ
	std::weak_ptr<BLAS> carBehindTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireInstance_;
	// 後ろタイヤのフレーム
	std::weak_ptr<BLAS> carBehindTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireFrameInstance_;
	// 右前タイヤ
	std::weak_ptr<BLAS> carRightTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightTireInstance_;
	// 右前タイヤのフレーム
	std::weak_ptr<BLAS> carRightTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightTireFrameInstance_;
	// 左前タイヤ
	std::weak_ptr<BLAS> carLeftTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireInstance_;
	// 左前タイヤのフレーム
	std::weak_ptr<BLAS> carLeftTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireFrameInstance_;

	// 車体とタイヤで処理を分けるための変数
	std::vector<std::weak_ptr<PolygonMeshInstance>> bodyInstance_;
	std::vector<std::weak_ptr<PolygonMeshInstance>> tireInstance_;

	// リトライ時BLAS解放用変数
	std::vector<std::weak_ptr<BLAS>> carBlasIndex_;

public:

	/*===== メンバ関数 =====*/

	enum class COLOR {

		RED,
		BLUE,

	};

	enum class CHARA {

		PLAYER,
		GHOST,
		AI,

	};

	// ロード
	void Load(COLOR ColorID, CHARA Chara);
	// 削除
	void Delete();

	// 表示
	void Display();
	// 非表示
	void NonDisplay();

};