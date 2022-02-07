#pragma once
#include "BaseLine.h"
#include "Enum.h"

class Line : public BaseLine {

public:

	/*-- メンバ関数 --*/

	// データ初期化関数
	void Generate(const XMFLOAT3& pos1 = XMFLOAT3(0, 0, 0), const XMFLOAT3& pos2 = XMFLOAT3(0, 0, 0), const XMFLOAT4& color = XMFLOAT4(1, 1, 1, 1), int projectionID = PROJECTIONID_OBJECT, int piplineID = PIPLINE_LINE_ALPHA);

	// 描画関数
	void Draw();

	// 頂点更新
	void ChangeVertexPos(const XMFLOAT3& pos1, const XMFLOAT3& pos2);

};