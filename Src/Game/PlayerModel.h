#pragma once
#include <vector>
#include <memory>

class PolygonMeshInstance;

class PlayerModel {

public:

	/*===== メンバ変数 =====*/

	// 車体
	int carBodyBlasIndex_;		// 車のモデルのBLASのインデックス
	std::weak_ptr<PolygonMeshInstance> carBodyInstance;		// 車のモデルのインスタンスのインデックス
	// バンパー
	int carBumperBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carBumperInstance;
	// ライト
	int carLeftLightBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carLeftLightInstance;
	// ライト
	int carRightLightBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carRightLightInstance;
	// ライト
	int carFrontLightBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carFrontLightInstance;
	// 鏡
	int carMirrorBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carMirrorInstance;
	// 鏡のカバー
	int carMirrorCoverBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carMirrorCoverInstance;
	// 窓
	int carWindowBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carWindowInsInstance;
	// 後ろタイヤ
	int carBehindTireBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireInstance;
	// 後ろタイヤのフレーム
	int carBehindTireFrameBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carBehindTireFrameInstance;
	// 右前タイヤ
	int carRightTireBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carRightTireInstance;
	// 右前タイヤのフレーム
	int carRightTireFrameBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carRightTireFrameInstance;
	// 左前タイヤ
	int carLeftTireBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireInstance;
	// 左前タイヤのフレーム
	int carLeftTireFrameBlasIndex_;
	std::weak_ptr<PolygonMeshInstance> carLeftTireFrameInstance;

	// 車体とタイヤで処理を分けるための変数
	std::vector<std::weak_ptr<PolygonMeshInstance>> bodyInstance;
	std::vector<std::weak_ptr<PolygonMeshInstance>> tireInstance;

	// リトライ時BLAS解放用変数
	std::vector<int> carBlasIndex_;

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