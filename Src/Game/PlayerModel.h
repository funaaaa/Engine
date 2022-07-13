#pragma once

class PlayerModel {

public:

	/*===== メンバ変数 =====*/

	// 車体
	int carBodyBlasIndex;		// 車のモデルのBLASのインデックス
	int carBodyInsIndex;		// 車のモデルのインスタンスのインデックス
	// バンパー
	int carBumperBlasIndex;
	int carBumperInsIndex;
	// ライト
	int carLightBlasIndex;
	int carLightInsIndex;
	// 後ろタイヤ
	int carBehindTireBlasIndex;
	int carBehindTireInsIndex;
	// 後ろタイヤのフレーム
	int carBehindTireFrameBlasIndex;
	int carBehindTireFrameInsIndex;
	// 右前タイヤ
	int carRightTireBlasIndex;
	int carRightTireInsIndex;
	// 右前タイヤのフレーム
	int carRightTireFrameBlasIndex;
	int carRightTireFrameInsIndex;
	// 左前タイヤ
	int carLeftTireBlasIndex;
	int carLeftTireInsIndex;
	// 左前タイヤのフレーム
	int carLeftTireFrameBlasIndex;
	int carLeftTireFrameInsIndex;
	// 鏡
	int carMirrorBlasIndex;
	int carMirrorInsIndex;
	// 鏡のカバー
	int carMirrorCoverBlasIndex;
	int carMirrorCoverInsIndex;
	// 窓
	int carWindowBlasIndex;
	int carWindowInsIndex;

public:

	/*===== メンバ関数 =====*/

	// ロード
	void Load();

};