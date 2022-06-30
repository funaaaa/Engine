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
	void ChangeScale(Vec3 amount);
	void ChangeScale(float x, float y, float z);
	// 回転量を更新 (指定した量分回転量が足される。)
	void ChangeRotation(Vec3 amount);
	void ChangeRotation(float x, float y, float z);
	// 回転量を初期化
	void InitRotation();
	// 回転量を代入
	void AssignmentRotationMat(DirectX::XMMATRIX amount);
	// 座標を更新 (指定した位置に移動。)
	void ChangePosition(Vec3 amount);
	void ChangePosition(float x, float y, float z);
	// 回転行列に回転行列をかける。
	void MulRotationMat(DirectX::XMMATRIX rotationMat);
	// 座標を更新 (指定した量を足す。)
	void ChangePositionAdd(Vec3 amount);
	void ChangePositionAdd(float x, float y, float z);
	// 各種行列を代入。
	void AssignmentWorldMatrix(const DirectX::XMMATRIX& posMat, const DirectX::XMMATRIX& scaleMat, const DirectX::XMMATRIX& rotationMat);
	// 画面に非表示
	void DoNotDisplay();
	// 画面に表示
	void DisplayOnScreen();
	// isDisplayを返す
	bool GetIsDisplay();
	// 指定されたインデックスのテクスチャIDを切り替える。(インデックスがオーバーフローかアンダーフローしていたら最大数or先頭を指定する。)
	void ChangeTextureID(int textureID, int index);
	// テクスチャIDを追加する。
	void AddTextureID(int textureID);
	// テクスチャIDをクリア。
	void ClearTextureID();

protected:

	/*-- メンバ変数 --*/

	// 基本データ
	bool isDisplay = true;	// 描画するかどうか
	std::vector<int> textureID;	// テクスチャのIDナンバー
	int projectionID;		// どの投影をするかのID
	int piplineID;			// 使用するパイプラインの名前

	// 各種行列
	DirectX::XMMATRIX scaleMat;		// スケーリング行列
	DirectX::XMMATRIX rotationMat;	// 回転行列(集積値)
	DirectX::XMMATRIX positionMat;	// 座標行列

	// デバッグ用の座標
	Vec3 pos;			// 座標




	/*-- 各種構造体 --*/

public:

	// 頂点データ構造体
	struct Vertex {
		Vec3 pos;			// xyz座標
		Vec3 normal;		// 法線
		Vec2 uv;			// uv座標
	};

	// 行列用アクセッタ
	inline DirectX::XMMATRIX GetScaleMat() { return scaleMat; }
	inline DirectX::XMMATRIX GetRotationMat() { return rotationMat; }
	inline DirectX::XMMATRIX GetPositionMat() { return positionMat; }

	// 座標データを取得
	inline Vec3 GetPos() { return pos; }

	// テクスチャIDを取得
	inline int GetTextureID(int index) { return textureID[index]; }

protected:

	// シェーダーに渡す行列のデータ
	struct MatrixData {
		DirectX::XMMATRIX world;			// 回転させたり移動させたりする行列
		DirectX::XMMATRIX viewproj;		// ビューとプロジェクション合成行列
	};

	// 定数バッファ構造体b0
	struct ConstBufferDataB0 {
		MatrixData mat;			// 行列
		DirectX::XMFLOAT4 color = { 1,1,1,1 };			// 色データ
		Vec3 eye;			// カメラ座標
		float pad1;
	};


	/*-- メンバ関数 --*/

	// 定数バッファ構造体b0のマップ処理
	void MapConstDataB0(Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0, const ConstBufferDataB0& constBufferDataB0);


public:

	// GSParticle用の定数バッファ構造体b0
	struct GSConstBufferDataB0 {
		DirectX::XMMATRIX mat;			// 行列
		DirectX::XMFLOAT4 color;			// 色データ
	};


};