#pragma once

// Characterで使用するフラグをまとめた構造体。
struct CharacterFlags {

public:

	/*===== メンバ変数 =====*/

	/*-- 一般的なフラグ --*/
	bool isShotBehind_;			// 後ろ側に甲羅を投げるかのフラグ
	bool onGround_;				// 地上にいるか t=地上 f=空中
	bool onGroundPrev_;			// 前フレームのonGround_
	bool onGrass_;				// 草の上にいるか t=草の上 f=草の上じゃない
	bool isConcentrationLine_;	// 集中線を出すかフラグ。
	
	/*-- アイテム関係 --*/
	bool isGetItem_;	// アイテムを取得したフレームの判定
	bool isUseItem_;			// アイテムを使った瞬間。
	
	/*-- ジャンプアクション、ギミック関係 --*/
	bool isHitJumpActionGimmick_;
	bool isJumpActionTrigger_;	// ジャンプアクションのトリガー
	bool isJumpAction_;			// ジャンプアクションが行われたか。	

	/*-- ゲーム開始前関係 --*/
	bool isAccel_;
	bool isBeforeStartPrev_;


public:

	/*===== メンバ関数 =====*/

	void Init();

};