#include "BakeAO.h"
#include "RaytracingOutput.h"
#include "RayPiplineBakeAO.h"
#include "RayRootsignature.h"
#include "DescriptorHeapMgr.h"
#include "HitGroupMgr.h"
#include <DirectXMath.h>

void BakeAO::ExecutionBake(const int& TexCount)
{

	/*===== ベイクを実行 =====*/

	// ベイク先のテクスチャを生成。
	bakeTex.clear();
	bakeTex.resize(TexCount);

	for (auto& index : bakeTex) {

		index = std::make_shared<RaytracingOutput>();
		index->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	}

	// ルートシグネチャを生成。
	rootsignature = std::make_shared<RayRootsignature>();

	// ルートシグネチャを設定。
	rootsignature = HitGroupMgr::Ins()->GetLocalRootSig(HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP);

	// パイプラインを生成。
	pipline = std::make_shared<RayPiplineBakeAO>();

	// パイプラインを設定。
	vector<RayPiplineShaderData> shaders;
	shaders.push_back({ "Resource/ShaderFiles/RayTracing/TriangleShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	pipline->Setting(shaders, HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP, 2, 0, 1, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// シェーダーテーブルを構築。
	pipline->ConstructionShaderTableBakeAO(bakeTex);

}
