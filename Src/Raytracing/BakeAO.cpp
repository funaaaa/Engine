#include "BakeAO.h"
#include "RaytracingOutput.h"
#include "RayPiplineBakeAO.h"
#include "RayRootsignature.h"
#include "DescriptorHeapMgr.h"
#include "HitGroupMgr.h"
#include <DirectXMath.h>

void BakeAO::ExecutionBake(const int& TexCount)
{

	/*===== �x�C�N�����s =====*/

	// �x�C�N��̃e�N�X�`���𐶐��B
	bakeTex.clear();
	bakeTex.resize(TexCount);

	for (auto& index : bakeTex) {

		index = std::make_shared<RaytracingOutput>();
		index->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	}

	// ���[�g�V�O�l�`���𐶐��B
	rootsignature = std::make_shared<RayRootsignature>();

	// ���[�g�V�O�l�`����ݒ�B
	rootsignature = HitGroupMgr::Ins()->GetLocalRootSig(HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP);

	// �p�C�v���C���𐶐��B
	pipline = std::make_shared<RayPiplineBakeAO>();

	// �p�C�v���C����ݒ�B
	vector<RayPiplineShaderData> shaders;
	shaders.push_back({ "Resource/ShaderFiles/RayTracing/TriangleShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	pipline->Setting(shaders, HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP, 2, 0, 1, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// �V�F�[�_�[�e�[�u�����\�z�B
	pipline->ConstructionShaderTableBakeAO(bakeTex);

}
