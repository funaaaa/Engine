#include "Sprite.h"
#include "TextureManager.h"
#include "Engine.h"
#include "PipelineManager.h"
#include "DescriptorHeapMgr.h"

void Sprite::CommonGenerate(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID)
{
	// �p�C�v�����̖��O�̕ۑ�
	this->piplineID_ = PiplineID;

	// �ݒ�\����
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// �V�F�[�_�[���猩����
	descHeapDesc.NumDescriptors = 2;										// CBV2��
	// �f�B�X�N���v�^�q�[�v�̐���
	Engine::Ins()->dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&constDescHeap_));

	// ���_�o�b�t�@�̐���
	Vertex vertexBuff;
	vertexBuff.pos_ = Vec3(-1.0f, 1.0f, 10);		// ����
	vertexBuff.uv_ = Vec2(0, 1);
	vertex_.push_back(vertexBuff);
	vertexBuff.pos_ = Vec3(-1.0f, -1.0f, 10);	// ����
	vertexBuff.uv_ = Vec2(0, 0);
	vertex_.push_back(vertexBuff);
	vertexBuff.pos_ = Vec3(1.0f, 1.0f, 10);		// �E��
	vertexBuff.uv_ = Vec2(1, 1);
	vertex_.push_back(vertexBuff);
	vertexBuff.pos_ = Vec3(1.0f, -1.0f, 10);		// �E��
	vertexBuff.uv_ = Vec2(1, 0);
	vertex_.push_back(vertexBuff);

	// ���_�o�b�t�@�r���[�̐���
	CD3DX12_HEAP_PROPERTIES vtxHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vtxResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertex_.size() * sizeof(Vertex));
	HRESULT result = Engine::Ins()->dev_->CreateCommittedResource(
		&vtxHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&vtxResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_)
	);

	// ���_�o�b�t�@�r���[�̐ݒ�
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = static_cast<UINT>(vertex_.size()) * static_cast<UINT>(sizeof(Vertex));
	vbView_.StrideInBytes = sizeof(Vertex);

	/*-----�萔�o�b�t�@�̐���-----*/
	CD3DX12_HEAP_PROPERTIES constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC constResDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);
	result = Engine::Ins()->dev_->CreateCommittedResource(
		&constHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0_)
	);

	// �s���������
	projectionID_ = ProjectionID;
	rotationMat_ = DirectX::XMMatrixIdentity();
	scaleMat_ = DirectX::XMMatrixScaling(Size.x_, Size.y_, 1.0f);
	positionMat_ = DirectX::XMMatrixTranslation(CenterPos.x_, CenterPos.y_, CenterPos.z_);
	pos_ = CenterPos;

	// �}�b�v�������s��
	Vertex* vertMap = nullptr;
	vertBuff_->Map(0, nullptr, (void**)&vertMap);
	// �S���_�ɑ΂���
	for (int i = 0; i < vertex_.size(); ++i)
	{
		vertMap[i] = vertex_.at(i);   // ���W���R�s�[
	}
	// �}�b�v������
	vertBuff_->Unmap(0, nullptr);

	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
	// CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap_->GetCPUDescriptorHandleForHeapStart(), 0, Engine::Ins()->dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuffB0_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB0_->GetDesc().Width;
	Engine::Ins()->dev_->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
}

void Sprite::GenerateForTexture(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, LPCWSTR FileName)
{

	// �e�N�X�`�������[�h
	textureID_.push_back(TextureManager::Ins()->LoadTexture(FileName));

	// ����������
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);

}

void Sprite::GenerateForColor(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, DirectX::XMFLOAT4 Color)
{

	// �e�N�X�`�������[�h
	textureID_.push_back(TextureManager::Ins()->CreateTexture(Color));

	// ����������
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);

}

void Sprite::GenerateSpecifyTextureID(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, int TextureID)
{

	// �e�N�X�`�������[�h
	this->textureID_.push_back(TextureID);

	// ����������
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);


}

void Sprite::Draw()
{
	// ��\����Ԃ�������`�揈�����s��Ȃ�
	if (isDisplay_ == false) return;

	// �p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
	PipelineManager::Ins()->SetPipline(piplineID_);

	// �萔�o�b�t�@B0�\���̂��}�b�v����
	MapConstDataB0(constBuffB0_, constBufferDataB0_);

	// ���W��ۑ����Ă���
	pos_ = Vec3(positionMat_.r[3].m128_f32[0], positionMat_.r[3].m128_f32[1], positionMat_.r[3].m128_f32[2]);

	// �萔�o�b�t�@�r���[�ݒ�R�}���h
	Engine::Ins()->mainGraphicsCmdList_->SetGraphicsRootConstantBufferView(0, constBuffB0_->GetGPUVirtualAddress());

	// �f�B�X�N���v�^�q�[�v�ݒ�R�}���h
	ID3D12DescriptorHeap* ppHeaps2[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	Engine::Ins()->mainGraphicsCmdList_->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

	// �V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
	for (int index = 0; index < textureID_.size(); ++index) {
		Engine::Ins()->mainGraphicsCmdList_->SetGraphicsRootDescriptorTable(index + 1, TextureManager::Ins()->GetSRV(textureID_[index]));
	}

	// ���_�o�b�t�@�r���[�ݒ�R�}���h
	Engine::Ins()->mainGraphicsCmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// �`��R�}���h
	Engine::Ins()->mainGraphicsCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
	Engine::Ins()->mainGraphicsCmdList_->DrawInstanced(static_cast<UINT>(vertex_.size()), 1, 0, 0);
}
