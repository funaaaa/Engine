#pragma once
#include "BaseStage.h"
#include "Vec.h"
#include <vector>
#include <string>

// 無限型ステージ 基本はこのステージを使用する。
class MugenStage : public BaseStage {

private:

	/*===== メンバ変数 =====*/

	int timer_;	// 各オブジェクトで使用するタイマー。セッティングされてからインクリメントし続ける。
	int goalInsIndex;	// ゴールのインスタンスのインデックス。
	int tunnelIndex_;

public:

	/*===== メンバ関数 =====*/

	void Setting(int TireMaskIndex)override;
	void Destroy()override;
	void Update()override;
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input)override;


private:

	// ビルをロードする際に使用するパラメーター
	enum class OBJ_DATA_ID {
		ID,			// オブジェクトのID
		POSITION,	// オブジェクトの座標パラメーター
		SCALE,		// オブジェクトのスケールパラメーター
		ROTATE,		// オブジェクトの回転パラメーター
		X,			// 各パラメーターのX成分
		Y,			// 各パラメーターのY成分
		Z,			// 各パラメーターのZ成分
	};
	void LoadBuilding();
	void GetBuildingData(std::string FilePath);

	// 加速ギミックをセット。いずれはtxtから読み取れるようにする予定です。
	void SetBoostGimmick();

};