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
	std::weak_ptr<PolygonMeshInstance> carBodyInstance;		// 車のモデルのインスタンスのインデックス
	// バンパー
	std::weak_ptr<BLAS> carBumperBlas_;
	std::weak_ptr<PolygonMeshInstance> carBumperInstance;
	// ライト
	std::weak_ptr<BLAS> carLeftLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftLightInstance;
	// ライト
	std::weak_ptr<BLAS> carRightLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightLightInstance;
	// ライト
	std::weak_ptr<BLAS> carFrontLightBlas_;
	std::weak_ptr<PolygonMeshInstance> carFrontLightInstance;
	// 鏡
	std::weak_ptr<BLAS> carMirrorBlas_;
	std::weak_ptr<PolygonMeshInstance> carMirrorInstance;
	// 鏡のカバー
	std::weak_ptr<BLAS> carMirrorCoverBlas_;
	std::weak_ptr<PolygonMeshInstance> carMirrorCoverInstance;
	// 窓
	std::weak_ptr<BLAS> carWindowBlas_;
	std::weak_ptr<PolygonMeshInstance> carWindowInsInstance;
	// 後ろタイヤ
	std::weak_ptr<BLAS> carBehindTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireInstance;
	// 後ろタイヤのフレーム
	std::weak_ptr<BLAS> carBehindTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireFrameInstance;
	// 右前タイヤ
	std::weak_ptr<BLAS> carRightTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightTireInstance;
	// 右前タイヤのフレーム
	std::weak_ptr<BLAS> carRightTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carRightTireFrameInstance;
	// 左前タイヤ
	std::weak_ptr<BLAS> carLeftTireBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireInstance;
	// 左前タイヤのフレーム
	std::weak_ptr<BLAS> carLeftTireFrameBlas_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireFrameInstance;

	// 車体とタイヤで処理を分けるための変数
	std::vector<std::weak_ptr<PolygonMeshInstance>> bodyInstance;
	std::vector<std::weak_ptr<PolygonMeshInstance>> tireInstance;

	// リトライ時BLAS解放用変数
	std::vector<std::weak_ptr<BLAS>> carBlasIndex_;

public:

	/*===== メンバ関数 =====*/

	enum class COLOR {

		RED,
		BLUE,

	};

	// ロード
	void Load(COLOR ColorID, bool IsGhost);
	// 削除
	void Delete();

};