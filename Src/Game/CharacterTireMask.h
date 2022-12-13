#pragma once
#include "Vec.h"
#include "PlayerModel.h"
#include <memory>

class CharacterGameFinish;
class CharacterDrift;
class BaseStage;
struct CharacterFlags;

// キャラクターのタイヤ痕に関する処理をまとめたクラス
class CharacterTireMask {

public:

	/*===== メンバ変数 =====*/

	// UV情報
	struct TireUVSet {
		Vec2 uv_;
		Vec2 prevuv_;
		TireUVSet() : uv_(Vec2()), prevuv_(Vec2()) {}
	};

	// タイヤ痕書き込み用
	struct TireMaskUV {
		TireUVSet forwardLeftUV_;
		TireUVSet forwardRightUV_;
		TireUVSet behindLeftUV_;
		TireUVSet behindRightUV_;
		TireMaskUV() : forwardLeftUV_(TireUVSet()), forwardRightUV_(TireUVSet()), behindLeftUV_(TireUVSet()), behindRightUV_(TireUVSet()) {};
	};

	// タイヤ痕更新処理似必要な入力情報
	struct TireMaskInData {
		std::weak_ptr<CharacterGameFinish> gameFinish_;
		std::weak_ptr<CharacterFlags> flags_;
		std::weak_ptr<CharacterDrift> drift_;
		std::weak_ptr<BaseStage> stageData_;
		PlayerModel playerModel_;
		Vec3 bottomVec_;
	};


private:

	TireMaskUV tireMaskUV_;				// タイヤ痕を出す際に使用する。


public:

	/*===== メンバ関数 =====*/

	CharacterTireMask();
	void Init();
	bool CheckTireMask(const TireMaskInData& InData, TireMaskUV& TireMaskUVData);

};