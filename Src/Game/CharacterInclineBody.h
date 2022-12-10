#pragma once
#include "Vec.h"
#include "PlayerModel.h"

// キャラクターの車体を傾けさせるための処理をまとめたクラス
class CharacterInclineBody {

private:

	/*===== メンバ変数 =====*/

	DirectX::XMVECTOR handleRotQ_;		// ハンドルの回転量
	DirectX::XMVECTOR nowHandleRotQ_;	// ハンドルの回転量
	DirectX::XMVECTOR boostRotQ_;		// 加速時の正面方向への回転の行列
	DirectX::XMVECTOR nowBoostRotQ_;	// 加速時の正面方向への回転の行列
	float handleAmount_;				// ハンドル量
	const float  MAX_DRIFT_ROT = 1.6f;	// ドリフト時の車体の回転の最大量
	float baseDriftRot_;				// ドリフト時に車体を回転させる際の補間先
	float nowDriftRot_;					// ドリフト時に車体を回転させる際の現在量
	float baseBoostRot_;				// ブースト時の車体の浮き上がりを補間する際の補完元
	float nowBoostRot_;					// ブースト時に車体の浮き上がりを補間する際の現在量
	float tireLollingAmount_;			// タイヤを持ち上げる量
	int forwardTireLollingTimer_;		// タイヤを持ち上げる際のタイマー
	const int FORWARD_TIMER_LOLLING_TIMER = 20;
	int driftRotTimer_;					// ドリフトの際に車体を回転させるために使用するタイマー。この値が大きくなれば車体の傾きも大きくなる。			
	const int MAX_DRIFT_ROT_TIMER = 10;
	bool isRotRightSide_ = false;		// どちら側にドリフトしているか。車体を回転させる方向を決める際に使用する。

public:

	// 車体の回転処理に必要な更新用データ
	struct InputData {
		bool onGround_;				// 設置フラグ
		bool isDrift_;				// ドリフト中か
		bool isDriftJump_;			// ドリフトジャンプ中かフラグ
		bool isPlayer_;				// プレイヤーかフラグ
		Vec3 pos_;					// キャラの座標
		Vec3 prevPos_;				// キャラの前フレーム座標
		float boostSpeed_;			// キャラのブースト量
		float maxBoostSpeed_;		// キャラのブースト量の最大値
		float rotY_;				// キャラのY軸回転量
		PlayerModel playerModel_;	// キャラのモデル情報
	};


public:

	CharacterInclineBody();
	void Init();
	void Update(const InputData& Input);

	// 回転に使用しているクォータニオンを初期化する。
	void InitQuaternion();

	// ハンドル量を取得
	float GetHandleAmount() { return handleAmount_; }
	// ハンドル量を更新
	inline void SetHandleAmount(float HandleAmount) { handleAmount_ = HandleAmount; }


};