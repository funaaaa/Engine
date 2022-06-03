#pragma once
#include "Singleton.h"
#include <vector>
#include <memory>

class RaytracingOutput;
class RayPiplineBakeAO;
class RayRootsignature;

// AOをテクスチャにベイク
class BakeAO : public Singleton<BakeAO> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<RaytracingOutput>> bakeTex;		// 書き込み先テクスチャ
	std::shared_ptr<RayPiplineBakeAO> pipline;					// 使用するパイプライン
	std::shared_ptr<RayRootsignature> rootsignature;			// 使用するルートシグネチャ


public:

	/*===== メンバ変数 =====*/

	// ベイクを実行。
	void ExecutionBake(const int& TexCount);

	// ベイク結果のテクスチャを取得。
	std::vector<std::shared_ptr<RaytracingOutput>>& GetBakeTex() { return bakeTex; }

};