#include "RaytracingPipline.h"
#include "DirectXTex/d3dx12.h"
#include "ShaderStorage.h"
#include "HitGroupMgr.h"
#include "RayRootsignature.h"
#include "DirectXBase.h"
#include <DirectXMath.h>

void RaytracingPipline::Setting(const std::vector<RayPiplineShaderData>& InputData)
{

	/*===== �Z�b�e�B���O���� =====*/

	// ���͂��ꂽ�f�[�^��ۑ�����B
	const int INPUT_COUNT = InputData.size();
	for (int index = 0; index < INPUT_COUNT; ++index) {

		// ���͂��ꂽ�f�[�^�\���́B
		RayPiplineShaderData buff;

		// �ۑ�����B
		buff.shaderPath = InputData[index].shaderPath;

		// �ۑ�����Ă���G���g���|�C���g��ۑ��B
		const int ENTRYPOINT_COUNT = InputData[index].entryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < ENTRYPOINT_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.entryPoint.push_back(InputData[index].entryPoint[entryPointIndex]);

		}

		// �ۑ�����B
		shaderData.push_back(buff);

	}

	// �O���[�o�����[�g�V�O�l�`����ݒ�B
	globalRootSig = std::make_shared<RayRootsignature>();
	// �p�����[�^�[t0��TLAS(SRV)��ݒ�B
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	// �p�����[�^�[b0�ɃJ�����p�o�b�t�@��ݒ�B
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	// �p�����[�^�[u0�ɏo�͗p�o�b�t�@��ݒ�B
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0);

	// ���[�g�V�O�l�`���𐶐��B
	globalRootSig->Create(false, L"GlobalRootSig");

	// �X�e�[�g�I�u�W�F�N�g�̐ݒ��ۑ����Ă����悤�̍\���́B
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// �X�e�[�g�I�u�W�F�N�g�̏�Ԃ�ݒ�B
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// �V�F�[�_�[��ǂݍ��ށB
	const int SHADER_COUNT = shaderData.size();
	for (int index = 0; index < SHADER_COUNT; ++index) {

		shaderCode.emplace_back();

		// �V�F�[�_�[���R���p�C������B
		ShaderStorage::Instance()->LoadShaderForDXC(shaderData[index].shaderPath, "lib_6_4", "");

		// �V�F�[�_�[��ǂݍ��ށB
		shaderCode[index].BytecodeLength = ShaderStorage::Instance()->GetShaderBin(shaderData[index].shaderPath).size();
		shaderCode[index].pShaderBytecode = ShaderStorage::Instance()->GetShaderBin(shaderData[index].shaderPath).data();

		// �V�F�[�_�[�̊e�֐����R�[�h�̓o�^�B
		auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		dxilLib->SetDXILLibrary(&shaderCode[index]);

		// �V�F�[�_�[�̃G���g���|�C���g��o�^�B
		const int ENTRYPOINT_COUNT = shaderData[index].entryPoint.size();
		for (int entryIndex = 0; entryIndex < ENTRYPOINT_COUNT; ++entryIndex) {

			dxilLib->DefineExport(shaderData[index].entryPoint[entryIndex]);

		}

	}

	// �q�b�g�O���[�v��ݒ�B
	const int HIT_GROUP_COUNT = HitGroupMgr::Instance()->GetHitGroupCount();
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		// �q�b�g�O���[�v�̐ݒ�B
		auto hitGroup = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		// ClosestHitShader���G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetCHFlag(index)) {
			hitGroup->SetClosestHitShaderImport(HitGroupMgr::Instance()->GetCH(index));
		}
		// AnyHitShader�̃G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetAHFlag(index)) {
			hitGroup->SetAnyHitShaderImport(HitGroupMgr::Instance()->GetAH(index));
		}
		// IntersectShader�̃G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetISFlag(index)) {
			hitGroup->SetIntersectionShaderImport(HitGroupMgr::Instance()->GetIS(index));
		}
		// �q�b�g�O���[�v����ۑ��B
		hitGroup->SetHitGroupExport(HitGroupMgr::Instance()->hitGroupNames[index]);

	}

	// �O���[�o�����[�g�V�O�l�`���̐ݒ�B
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig->GetRootSig().Get());

	// HitGroup�̃��[�J�����[�g�V�O�l�`����ݒ�B
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		chLocalRootSig->SetRootSignature(HitGroupMgr::Instance()->GetLocalRootSig(index)->GetRootSig().Get());
		auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		chAssocModel->AddExport(HitGroupMgr::Instance()->hitGroupNames[index]);
		chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

	}

	// �V�F�[�_�[�̐ݒ�B
	auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfig->Config(sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// �p�C�v���C���̐ݒ�B
	auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfig->Config(30);

	// ��������B
	HRESULT resultBuff = DirectXBase::Instance()->dev->CreateStateObject(
		subobjects, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);

 	stateObject->SetName(L"StateObject");

}
