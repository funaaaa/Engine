#include "RaytracingPipline.h"
#include "DirectXTex/d3dx12.h"
#include "ShaderStorage.h"
#include "HitGroupMgr.h"
#include "RayRootsignature.h"
#include "DirectXBase.h"
#include <DirectXMath.h>

void RaytracingPipline::Setting(const std::vector<RayPiplineShaderData>& InputData)
{

	/*===== セッティング処理 =====*/

	// 入力されたデータを保存する。
	const int INPUT_COUNT = InputData.size();
	for (int index = 0; index < INPUT_COUNT; ++index) {

		// 入力されたデータ構造体。
		RayPiplineShaderData buff;

		// 保存する。
		buff.shaderPath = InputData[index].shaderPath;

		// 保存されているエントリポイントを保存。
		const int ENTRYPOINT_COUNT = InputData[index].entryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < ENTRYPOINT_COUNT; ++entryPointIndex) {

			// 保存する。
			buff.entryPoint.push_back(InputData[index].entryPoint[entryPointIndex]);

		}

		// 保存する。
		shaderData.push_back(buff);

	}

	// グローバルルートシグネチャを設定。
	globalRootSig = std::make_shared<RayRootsignature>();
	// パラメーターt0にTLAS(SRV)を設定。
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	// パラメーターb0にカメラ用バッファを設定。
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	// パラメーターu0に出力用バッファを設定。
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0);

	// ルートシグネチャを生成。
	globalRootSig->Create(false, L"GlobalRootSig");

	// ステートオブジェクトの設定を保存しておくようの構造体。
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// ステートオブジェクトの状態を設定。
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// シェーダーを読み込む。
	const int SHADER_COUNT = shaderData.size();
	for (int index = 0; index < SHADER_COUNT; ++index) {

		shaderCode.emplace_back();

		// シェーダーをコンパイルする。
		ShaderStorage::Instance()->LoadShaderForDXC(shaderData[index].shaderPath, "lib_6_4", "");

		// シェーダーを読み込む。
		shaderCode[index].BytecodeLength = ShaderStorage::Instance()->GetShaderBin(shaderData[index].shaderPath).size();
		shaderCode[index].pShaderBytecode = ShaderStorage::Instance()->GetShaderBin(shaderData[index].shaderPath).data();

		// シェーダーの各関数レコードの登録。
		auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		dxilLib->SetDXILLibrary(&shaderCode[index]);

		// シェーダーのエントリポイントを登録。
		const int ENTRYPOINT_COUNT = shaderData[index].entryPoint.size();
		for (int entryIndex = 0; entryIndex < ENTRYPOINT_COUNT; ++entryIndex) {

			dxilLib->DefineExport(shaderData[index].entryPoint[entryIndex]);

		}

	}

	// ヒットグループを設定。
	const int HIT_GROUP_COUNT = HitGroupMgr::Instance()->GetHitGroupCount();
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		// ヒットグループの設定。
		auto hitGroup = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		// ClosestHitShaderをエントリポイントを保存。
		if (HitGroupMgr::Instance()->GetCHFlag(index)) {
			hitGroup->SetClosestHitShaderImport(HitGroupMgr::Instance()->GetCH(index));
		}
		// AnyHitShaderのエントリポイントを保存。
		if (HitGroupMgr::Instance()->GetAHFlag(index)) {
			hitGroup->SetAnyHitShaderImport(HitGroupMgr::Instance()->GetAH(index));
		}
		// IntersectShaderのエントリポイントを保存。
		if (HitGroupMgr::Instance()->GetISFlag(index)) {
			hitGroup->SetIntersectionShaderImport(HitGroupMgr::Instance()->GetIS(index));
		}
		// ヒットグループ名を保存。
		hitGroup->SetHitGroupExport(HitGroupMgr::Instance()->hitGroupNames[index]);

	}

	// グローバルルートシグネチャの設定。
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig->GetRootSig().Get());

	// HitGroupのローカルルートシグネチャを設定。
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		chLocalRootSig->SetRootSignature(HitGroupMgr::Instance()->GetLocalRootSig(index)->GetRootSig().Get());
		auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		chAssocModel->AddExport(HitGroupMgr::Instance()->hitGroupNames[index]);
		chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

	}

	// シェーダーの設定。
	auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfig->Config(sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// パイプラインの設定。
	auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfig->Config(30);

	// 生成する。
	HRESULT resultBuff = DirectXBase::Instance()->dev->CreateStateObject(
		subobjects, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);

 	stateObject->SetName(L"StateObject");

}
