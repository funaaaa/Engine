#include "DoubleResourceWrapper.h"
#include "FHelper.h"
#include "Engine.h"

void DoubleResourceWrapper::CreateBuffer(size_t Size, void* Data, const wchar_t* Name)
{

	/*===== �o�b�t�@�𐶐� =====*/

	// �o�b�t�@�̃T�C�Y��ۑ�
	bufferSize_ = Size;

	// Map�p�T�C�Y
	D3D12_RANGE range{ 0, Size };

	// �o�b�t�@�{�̂𐶐�����B
	resource_[0] = FHelper::CreateBuffer(
		Size,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_BARRIER barrier = { CD3DX12_RESOURCE_BARRIER::Transition(resource_[0].Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST) };
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	resource_[0]->SetName(Name);
	resource_[1] = FHelper::CreateBuffer(
		Size,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT);
	barrier = { CD3DX12_RESOURCE_BARRIER::Transition(resource_[1].Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST) };
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	resource_[1]->SetName(Name);

	// �A�b�v���[�h�p�o�b�t�@�𐶐�����B
	uploadResource_[0] = FHelper::CreateBuffer(
		Size,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	uploadResource_[0]->SetName(Name);
	uploadResource_[0]->Map(0, &range, (void**)&mapAddress_[0]);
	uploadResource_[1] = FHelper::CreateBuffer(
		Size,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	uploadResource_[1]->SetName(Name);
	uploadResource_[1]->Map(0, &range, (void**)&mapAddress_[1]);

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	memcpy(mapAddress_[0], Data, Size);
	memcpy(mapAddress_[1], Data, Size);

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(resource_[0].Get(), uploadResource_[0].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_[0].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(resource_[1].Get(), uploadResource_[1].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_[1].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

}

void DoubleResourceWrapper::WriteData(int Index, size_t Size, void* Data)
{

	/*===== �f�[�^���������� =====*/

	// �������ƃT�C�Y�̈Ⴄ�o�b�t�@����������ł�����x��
	if (Size != bufferSize_) assert(0);

	memcpy(mapAddress_[Index], Data, Size);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_[Index].Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(resource_[Index].Get(), uploadResource_[Index].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_[Index].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

}
