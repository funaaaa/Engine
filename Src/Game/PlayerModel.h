#pragma once
#include <vector>

class PlayerModel {

public:

	/*===== メンバ変数 =====*/

	// 車体
	int carBodyBlasIndex_;		// 車のモデルのBLASのインデックス
	int carBodyInsIndex_;		// 車のモデルのインスタンスのインデックス
	// バンパー
	int carBumperBlasIndex_;
	int carBumperInsIndex_;
	// ライト
	int carLeftLightBlasIndex_;
	int carLeftLightInsIndex_;
	// ライト
	int carRightLightBlasIndex_;
	int carRightLightInsIndex_;
	// ライト
	int carFrontLightBlasIndex_;
	int carFrontLightInsIndex_;
	// 鏡
	int carMirrorBlasIndex_;
	int carMirrorInsIndex_;
	// 鏡のカバー
	int carMirrorCoverBlasIndex_;
	int carMirrorCoverInsIndex_;
	// 窓
	int carWindowBlasIndex_;
	int carWindowInsIndex_;
	// 後ろタイヤ
	int carBehindTireBlasIndex_;
	int carBehindTireInsIndex_;
	// 後ろタイヤのフレーム
	int carBehindTireFrameBlasIndex_;
	int carBehindTireFrameInsIndex_;
	// 右前タイヤ
	int carRightTireBlasIndex_;
	int carRightTireInsIndex_;
	// 右前タイヤのフレーム
	int carRightTireFrameBlasIndex_;
	int carRightTireFrameInsIndex_;
	// 左前タイヤ
	int carLeftTireBlasIndex_;
	int carLeftTireInsIndex_;
	// 左前タイヤのフレーム
	int carLeftTireFrameBlasIndex_;
	int carLeftTireFrameInsIndex_;

	// 車体とタイヤで処理を分けるための変数
	std::vector<int> bodyInsIndex_;
	std::vector<int> tireInsIndex_;

public:

	/*===== メンバ関数 =====*/

	enum class COLOR {

		RED,
		BLUE,

	};

	// ロード
	void Load(COLOR ColorID, bool IsGhost);

};

/*

・設置した瞬間に沈める処理を書く。ｗｗ

*/