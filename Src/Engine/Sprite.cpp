#include "Sprite.h"
#include "TextureManager.h"
#include "DirectXBase.h"
#include "PiplineManager.h"

void Sprite::CommonGenerate(Vec3 CenterPos, DirectX::XMFLOAT2 Size, int ProjectionID, int PiplineID)
{
	//�p�C�v�����̖��O�̕ۑ�
	this->piplineID = PiplineID;

	//�ݒ�\����
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
	descHeapDesc.NumDescriptors = 2;										//CBV2��
	//�f�B�X�N���v�^�q�[�v�̐���
	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&constDescHeap));

	//���_�o�b�t�@�̐���
	Vertex vertexBuff;
	vertexBuff.pos = Vec3(-Size.x, Size.y, 10);		//����
	vertexBuff.uv = DirectX::XMFLOAT2(0, 1);
	vertex.push_back(vertexBuff);
	vertexBuff.pos = Vec3(-Size.x, -Size.y, 10);	//����
	vertexBuff.uv = DirectX::XMFLOAT2(0, 0);
	vertex.push_back(vertexBuff);
	vertexBuff.pos = Vec3(Size.x, Size.y, 10);		//�E��
	vertexBuff.uv = DirectX::XMFLOAT2(1, 1);
	vertex.push_back(vertexBuff);
	vertexBuff.pos = Vec3(Size.x, -Size.y, 10);		//�E��
	vertexBuff.uv = DirectX::XMFLOAT2(1, 0);
	vertex.push_back(vertexBuff);

	//���_�o�b�t�@�r���[�̐���
	CD3DX12_HEAP_PROPERTIES vtxHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vtxResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertex.size() * sizeof(Vertex));
	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
		&vtxHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&vtxResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	//���_�o�b�t�@�r���[�̐ݒ�
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = static_cast<UINT>(vertex.size()) * static_cast<UINT>(sizeof(Vertex));
	vbView.StrideInBytes = sizeof(Vertex);

	/*-----�萔�o�b�t�@�̐���-----*/
	CD3DX12_HEAP_PROPERTIES constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC constResDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);
	result = DirectXBase::Ins()->dev->CreateCommittedResource(
		&constHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);

	//�s���������
	projectionID = ProjectionID;
	rotationMat = DirectX::XMMatrixIdentity();
	scaleMat = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	positionMat = DirectX::XMMatrixTranslation(CenterPos.x, CenterPos.y, CenterPos.z);
	pos = CenterPos;

	//�}�b�v�������s��
	Vertex* vertMap = nullptr;
	vertBuff->Map(0, nullptr, (void**)&vertMap);
	// �S���_�ɑ΂���
	for (int i = 0; i < vertex.size(); ++i)
	{
		vertMap[i] = vertex.at(i);   // ���W���R�s�[
	}
	// �}�b�v������
	vertBuff->Unmap(0, nullptr);

	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuffB0->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB0->GetDesc().Width;
	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
}

void Sprite::GenerateForTexture(Vec3 CenterPos, DirectX::XMFLOAT2 Size, int ProjectionID, int PiplineID, LPCWSTR FileName)
{

	// �e�N�X�`�������[�h
	textureID.push_back(TextureManager::Ins()->LoadTexture(FileName));

	// ����������
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);

}

void Sprite::GenerateForColor(Vec3 CenterPos, DirectX::XMFLOAT2 Size, int ProjectionID, int PiplineID, DirectX::XMFLOAT4 Color)
{

	// �e�N�X�`�������[�h
	textureID.push_back(TextureManager::Ins()->CreateTexture(Color));

	// ����������
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);

}

void Sprite::GenerateSpecifyTextureID(Vec3 CenterPos, DirectX::XMFLOAT2 Size, int ProjectionID, int PiplineID, int TextureID)
{

	// �e�N�X�`�������[�h
	this->textureID.push_back(TextureID);

	// ����������
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);


}

void Sprite::Draw()
{
	// ��\����Ԃ�������`�揈�����s��Ȃ�
	if (isDisplay == false) return;

	// �p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
	PiplineManager::Ins()->SetPipline(piplineID);

	// �萔�o�b�t�@B0�\���̂��}�b�v����
	MapConstDataB0(constBuffB0, constBufferDataB0);

	// ���W��ۑ����Ă���
	pos = Vec3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);

	// �萔�o�b�t�@�r���[�ݒ�R�}���h
	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// �f�B�X�N���v�^�q�[�v�ݒ�R�}���h
	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

	// �V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
	for (int i = 0; i < textureID.size(); ++i) {
		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(textureID[i]));
	}

	// ���_�o�b�t�@�r���[�ݒ�R�}���h
	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	// �`��R�}���h
	DirectXBase::Ins()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
	DirectXBase::Ins()->cmdList->DrawInstanced(static_cast<UINT>(vertex.size()), 1, 0, 0);
}
