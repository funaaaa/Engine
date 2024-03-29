#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include "Vec.h"

// 描画データクラスの基底クラス
class BaseDrawData {

public:

	/*-- メンバ関数 --*/

	// スケールを更新 (指定した大きさになる。)
	void ChangeScale(Vec3 Amount);
	void ChangeScale(float X, float Y, float Z);
	// 回転量を更新 (指定した量分回転量が足される。)
	void AddRotation(Vec3 Amount);
	void AddRotation(float X, float Y, float Z);
	// 回転量を加算。
	void ChangeRotation(Vec3 Amount);
	// 回転量を初期化
	void InitRotation();
	// 回転量を代入
	void AssignmentRotationMat(DirectX::XMMATRIX Amount);
	// 座標を更新 (指定した位置に移動。)
	void ChangePosition(Vec3 Amount);
	void ChangePosition(float X, float Y, float Z);
	// 回転行列に回転行列をかける。
	void MulRotationMat(DirectX::XMMATRIX RotationMat);
	// 座標を更新 (指定した量を足す。)
	void ChangePositionAdd(Vec3 Amount);
	void ChangePositionAdd(float X, float Y, float Z);
	// 各種行列を代入。
	void AssignmentWorldMatrix(const DirectX::XMMATRIX& PosMat, const DirectX::XMMATRIX& ScaleMat, const DirectX::XMMATRIX& RotationMat);
	// 画面に非表示
	void DoNotDisplay();
	// 画面に表示
	void DisplayOnScreen();
	// isDisplayを返す
	bool GetIsDisplay();
	// 指定されたインデックスのテクスチャIDを切り替える。(インデックスがオーバーフローかアンダーフローしていたら最大数or先頭を指定する。)
	void ChangeTextureID(int TextureID, int Index);
	// テクスチャIDを追加する。
	void AddTextureID(int TextureID);
	// テクスチャIDをクリア。
	void ClearTextureID();

protected:

	/*-- メンバ変数 --*/

	// 基本データ
	bool isDisplay_ = true;			// 描画するかどうか
	std::vector<int> textureID_;		// テクスチャのIDナンバー
	int projectionID_;				// どの投影をするかのID
	int piplineID_;					// 使用するパイプラインの名前

	// 各種行列
	DirectX::XMMATRIX scaleMat_;		// スケーリング行列
	DirectX::XMMATRIX rotationMat_;	// 回転行列(集積値)
	DirectX::XMMATRIX positionMat_;	// 座標行列

	// デバッグ用の座標
	Vec3 pos_;						// 座標




	/*-- 各種構造体 --*/

public:

	// 頂点データ構造体
	struct Vertex {
		Vec3 pos_;			// xyz座標
		Vec3 normal_;		// 法線
		Vec2 uv_;			// uv座標
	};

	// 行列用アクセッサ
	inline DirectX::XMMATRIX GetScaleMat() { return scaleMat_; }
	inline DirectX::XMMATRIX GetRotationMat() { return rotationMat_; }
	inline DirectX::XMMATRIX GetPositionMat() { return positionMat_; }

	// 座標データを取得
	inline Vec3 GetPos() { return pos_; }

	// テクスチャIDを取得
	inline int GetTextureID(int Index) { return textureID_[Index]; }

protected:

	// シェーダーに渡す行列のデータ
	struct MatrixData {
		DirectX::XMMATRIX world_;		// 回転させたり移動させたりする行列
		DirectX::XMMATRIX viewproj;		// ビューとプロジェクション合成行列
	};

	// 定数バッファ構造体b0
	struct ConstBufferDataB0 {
		MatrixData mat;					// 行列
		DirectX::XMFLOAT4 color = { 1,1,1,1 };			// 色データ
		Vec3 eye_;						// カメラ座標
		float pad1_;
	};


	/*-- メンバ関数 --*/

	// 定数バッファ構造体b0のマップ処理
	void MapConstDataB0(ConstBufferDataB0* ConstMap, const ConstBufferDataB0& ConstBufferDataB0);


public:

	// GSParticle用の定数バッファ構造体b0
	struct GSConstBufferDataB0 {
		DirectX::XMMATRIX mat;			// 行列
		DirectX::XMFLOAT4 color;		// 色データ
	};


};