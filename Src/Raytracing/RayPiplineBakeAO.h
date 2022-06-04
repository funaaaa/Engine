#pragma once
#include "RaytracingPipline.h"
#include <vector>
#include <memory>

class RaytracingOutput;

// AOベイク用パイプライン
class RayPiplineBakeAO : public RaytracingPipline {

public:

	/*===== メンバ関数 =====*/

	// ベイク用UAVを書き込むためのシェーダーレコード設定処理。
	void ConstructionShaderTableBakeAO(std::vector<std::shared_ptr<RaytracingOutput>> BakeTargets, const int& DispatchX, const int& DispatchY);

};