#pragma once
#include "Vec.h"
#include <memory>
#include <array>

class BLAS;
class PolygonMeshInstance;

// キャラクターの加速アイテムのロケットを可視化するためのオブジェクトに関する機能をまとめたクラス
class CharacterRocket {

private:

	/*===== メンバ変数 =====*/

	std::array<std::weak_ptr<BLAS>, 4> rocketBlas_;					// ロケットの形状データ
	std::array<std::weak_ptr<PolygonMeshInstance>, 4 > rocketIns_;	// ロケットの描画データ
	float rocketEasingTimer_;										// ロケットのイージング全般に使用するタイマー
	float rocketRotationY_;											// ロケットがY軸回転した量。
	float rocketAddRotationY_;										// ロケットのY軸回転量。常にY軸回転させる。
	bool isDisplayRocket_;											// ロケットを描画するかのフラグ。

public:

	// 更新処理に必要な入力情報
	struct UpdateInData {
		Vec3 pos_;
		Vec3 forwardVec_;
		Vec3 upVec_;
		bool isPlayer_;
		bool isGetItem_;
		bool isUseItem_;
		bool hasItem_;
	};


public:

	CharacterRocket();
	void Init();
	void Update(const UpdateInData& InData);

};