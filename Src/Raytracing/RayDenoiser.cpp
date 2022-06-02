#include "RayDenoiser.h"

void Denoiser::Setting()
{

	/*===== �R���X�g���N�^ =====*/

	// �u���[�o�̓e�X�g�p�N���X���Z�b�g�B
	blurXOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	blurYOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �K�E�V�A���u���[�Ɏg�p����R���s���[�g�V�F�[�_�[���Z�b�g�B
	blurX.Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurX.hlsl", 0, 0, 1, { 0 });
	blurY.Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurY.hlsl", 0, 0, 1, { blurXOutput.GetUAVIndex() });
	blurFinal.Setting(L"Resource/ShaderFiles/RayTracing/DenoiseFinal.hlsl", 0, 0, 1, { blurYOutput.GetUAVIndex() });

}

void Denoiser::ApplyDenoise(const int& InputUAVIndex, const int& OutputUAVIndex)
{

	/*===== �f�m�C�Y���� =====*/

	// �o�͗pUAV�̏�Ԃ�ς���B
	blurXOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �R���s���[�g�V�F�[�_�[�����s�B
	blurX.ChangeInputUAVIndex({ InputUAVIndex });
	blurX.Dispatch((window_width / 2.0f) / 4, window_height / 4, 1, blurXOutput.GetUAVIndex(), {});
	blurY.Dispatch((window_width / 2.0f) / 4, (window_height / 2.0f) / 4, 1, blurYOutput.GetUAVIndex(), {});
	blurFinal.Dispatch(window_width / 4, window_height / 4, 1, OutputUAVIndex, {});

	// �o�͗pUAV�̏�Ԃ�ς���B
	blurXOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	blurYOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

}
