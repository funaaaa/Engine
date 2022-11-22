#pragma once
#include "BaseStage.h"
#include "Vec.h"
#include <vector>
#include <string>

// 無限型ステージ
class MugenStage : public BaseStage {

private:

	/*===== メンバ変数 =====*/

	int timer_;	// 各オブジェクトで使用するタイマー。セッティングされてからインクリメントし続ける。
	int goalInsIndex;	// ゴールのインスタンスのインデックス。
	int tunnelIndex_;

	std::vector<Vec3> pointLightPos;

public:

	enum class STATUS {

		DEF,
		REFLECTION,

	};

private:

	STATUS status_;

public:

	/*===== メンバ関数 =====*/

	void Setting(int TireMaskIndex)override;
	void Destroy()override;
	void Update()override;
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input)override;


private:

	enum class OBJ_DATA_ID {
		ID,
		POSITION,
		SCALE,
		ROTATE,
		X,
		Y,
		Z,
	};
	void LoadBuilding();
	void GetBuildingData(std::string FilePath);

};