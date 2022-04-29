#include "ObjectManager.h"
#include "Enum.h"

//Line ObjectManager::GenerateLine(const XMFLOAT3& pos1, const XMFLOAT3& pos2, const XMFLOAT4& color, int projectionID, int piplineID)
//{
//	//���̕ϐ�
//	Line lineBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	lineBuffer.basicdata.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&lineBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	lineBuffer.basicdata.textureID.push_back(TextureManager::Ins()->CreateTexture(color));
//
//	//���_�o�b�t�@�̐���
//	Vertex vertex;
//	vertex.pos = pos1;	//����
//	vertex.uv = XMFLOAT2(0, 0);
//	lineBuffer.vertex.push_back(vertex);
//	vertex.pos = pos2;		//����
//	vertex.uv = XMFLOAT2(0, 0);
//	lineBuffer.vertex.push_back(vertex);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(lineBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&lineBuffer.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	lineBuffer.vbView.BufferLocation = lineBuffer.vertBuff->GetGPUVirtualAddress();
//	lineBuffer.vbView.SizeInBytes = lineBuffer.vertex.size() * sizeof(Vertex);
//	lineBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&lineBuffer.constBuffB0)
//	);
//
//	//�s���������
//	lineBuffer.basicdata.projectionID = projectionID;
//	lineBuffer.rotationMat = XMMatrixIdentity();
//	lineBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	lineBuffer.positionMat = XMMatrixIdentity();
//	lineBuffer.pos1 = pos1;
//	lineBuffer.pos2 = pos2;
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	lineBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < lineBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = lineBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	lineBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		lineBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = lineBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)lineBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = lineBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	lineBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return lineBuffer;
//}

//Sprite ObjectManager::GenerateSprite(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color, LPCWSTR fileName)
//{
//	//���̕ϐ�
//	Sprite spriteBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	spriteBuffer.basicdata.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName != 0) {
//		spriteBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(fileName));
//	}
//	else {
//		spriteBuffer.basicdata.textureID.push_back(TextureManager::Ins()->CreateTexture(color));
//	}
//
//	//���_�o�b�t�@�̐���
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//����
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//����
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB0)
//	);
//
//	//�s���������
//	spriteBuffer.basicdata.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = spriteBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	spriteBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return spriteBuffer;
//}
//
//Sprite ObjectManager::GenerateSpriteTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID)
//{
//	//���̕ϐ�
//	Sprite spriteBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	spriteBuffer.basicdata.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	spriteBuffer.basicdata.textureID.push_back(textureID);
//
//	//���_�o�b�t�@�̐���
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//����
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//����
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB0)
//	);
//
//	//�s���������
//	spriteBuffer.basicdata.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = spriteBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//
//	spriteBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return spriteBuffer;
//}

//SpriteMultiTexture ObjectManager::GenerateSpriteMultiTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, vector<LPCWSTR>* fileName)
//{
//	//���̕ϐ�
//	SpriteMultiTexture spriteBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	spriteBuffer.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName != nullptr) {
//		spriteBuffer.textureID.resize(fileName->size());
//		for (int i = 0; i < fileName->size(); ++i) {
//			spriteBuffer.textureID.at(i) = TextureManager::Ins()->LoadTexture(fileName->at(i));
//		}
//	}
//
//	//���_�o�b�t�@�̐���
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//����
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//����
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB0)
//	);
//
//	//�s���������
//	spriteBuffer.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = spriteBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//
//	spriteBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return spriteBuffer;
//}

//SpriteMultiPath ObjectManager::GenerateSPriteMultiPath(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID)
//{
//	//���̕ϐ�
//	SpriteMultiPath spriteBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	spriteBuffer.basicdata.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	spriteBuffer.basicdata.textureID.push_back(textureID);
//
//	//���_�o�b�t�@�̐���
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//����
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//����
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//�E��
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB0)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataMultiPath) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB1)
//	);
//
//	//�s���������
//	spriteBuffer.basicdata.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//	//2�ڂ𐶐�
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB1->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB1->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = spriteBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	spriteBuffer.constBuffB0->Unmap(0, nullptr);
//	//�萔�o�b�t�@1�ւ̃f�[�^�]��
//	ConstBufferDataMultiPath* constMap2 = nullptr;
//	result = spriteBuffer.constBuffB1->Map(0, nullptr, (void**)&constMap2);
//	spriteBuffer.constBufferDataB1.gaussianWeight[0] = {};
//	spriteBuffer.constBufferDataB1.gaussianWeight[1] = {};
//	constMap2->gaussianWeight[0] = spriteBuffer.constBufferDataB1.gaussianWeight[0];
//	constMap2->gaussianWeight[1] = spriteBuffer.constBufferDataB1.gaussianWeight[1];
//	spriteBuffer.constBuffB1->Unmap(0, nullptr);
//
//	return spriteBuffer;
//}

//Object3D ObjectManager::GenerateModelObj(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string modelFileName, LPCWSTR textureFileName, bool isSmoothing)
//{
//	//���̕ϐ�
//	Object3D objectBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	objectBuffer.basicdata.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 3;										//CBV3��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	HRESULT resultBuff = DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&objectBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	objectBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(textureFileName));
//
//	//obj�t�@�C�������[�h
//	ModelDataManager::LoadObj(directoryPath, modelFileName, objectBuffer, isSmoothing);
//
//	//���_�o�b�t�@�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	objectBuffer.vbView.BufferLocation = objectBuffer.vertBuff->GetGPUVirtualAddress();
//	objectBuffer.vbView.SizeInBytes = objectBuffer.vertex.size() * sizeof(Vertex);
//	objectBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	objectBuffer.ibView.BufferLocation = objectBuffer.indexBuff->GetGPUVirtualAddress();
//	objectBuffer.ibView.Format = DXGI_FORMAT_R16_UINT;
//	objectBuffer.ibView.SizeInBytes = objectBuffer.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB0)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB1)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB2) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB2)
//	);
//
//	//�s���������
//	objectBuffer.basicdata.projectionID = projectionID;
//	objectBuffer.rotationMat = XMMatrixIdentity();
//	objectBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	objectBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	objectBuffer.pos = XMFLOAT3(objectBuffer.positionMat.r[3].m128_f32[0], objectBuffer.positionMat.r[3].m128_f32[1], objectBuffer.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	objectBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//�S���_�ɑ΂���
//	for (int i = 0; i < objectBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = objectBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	//�}�b�v������
//	objectBuffer.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (objectBuffer.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = objectBuffer.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < objectBuffer.index.size(); ++i) {
//			indexMap[i] = objectBuffer.index.at(i);
//		}
//		objectBuffer.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = objectBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = objectBuffer.constBuffB1->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB1->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 2, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = objectBuffer.constBuffB2->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB2->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�s��Ǝ��_���W�֌W)
//	ConstBufferDataB0* constMap = nullptr;
//	result = objectBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	objectBuffer.constBuffB0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�}�e���A���֌W)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = objectBuffer.constBuffB1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = objectBuffer.constBuffDataB1.ambient;
//	constMap1->diffuse = objectBuffer.constBuffDataB1.diffuse;
//	constMap1->specular = objectBuffer.constBuffDataB1.specular;
//	constMap1->alpha = objectBuffer.constBuffDataB1.alpha;
//	objectBuffer.constBuffB1->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(���C�g�֌W)
//	ConstBufferDataB2* constMap2 = nullptr;
//	result = objectBuffer.constBuffB2->Map(0, nullptr, (void**)&constMap2);
//	for (int i = 0; i < DIRLIGHT_NUM; ++i) {
//		constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
//	}
//	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
//		constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
//	}
//	for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
//		constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
//	}
//	objectBuffer.constBuffB2->Unmap(0, nullptr);
//
//	return objectBuffer;
//}
//
//Object3DShadow ObjectManager::GenerateModelObjShadow(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string modelFileName, LPCWSTR textureFileName, bool isSmoothing)
//{
//	//���̕ϐ�
//	Object3DShadow objectBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	objectBuffer.basicdata.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 3;										//CBV3��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&objectBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	objectBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(textureFileName));
//
//	//obj�t�@�C�������[�h
//	ModelDataManager::LoadObj(directoryPath, modelFileName, objectBuffer, isSmoothing);
//
//	//���_�o�b�t�@�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	objectBuffer.vbView.BufferLocation = objectBuffer.vertBuff->GetGPUVirtualAddress();
//	objectBuffer.vbView.SizeInBytes = objectBuffer.vertex.size() * sizeof(Vertex);
//	objectBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	objectBuffer.ibView.BufferLocation = objectBuffer.indexBuff->GetGPUVirtualAddress();
//	objectBuffer.ibView.Format = DXGI_FORMAT_R16_UINT;
//	objectBuffer.ibView.SizeInBytes = objectBuffer.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB0)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB1)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB2Shadow) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB2)
//	);
//
//	//�s���������
//	objectBuffer.basicdata.projectionID = projectionID;
//	objectBuffer.rotationMat = XMMatrixIdentity();
//	objectBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	objectBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	objectBuffer.pos = XMFLOAT3(objectBuffer.positionMat.r[3].m128_f32[0], objectBuffer.positionMat.r[3].m128_f32[1], objectBuffer.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	objectBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//�S���_�ɑ΂���
//	for (int i = 0; i < objectBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = objectBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	//�}�b�v������
//	objectBuffer.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (objectBuffer.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = objectBuffer.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < objectBuffer.index.size(); ++i) {
//			indexMap[i] = objectBuffer.index.at(i);
//		}
//		objectBuffer.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = objectBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = objectBuffer.constBuffB1->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB1->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 2, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = objectBuffer.constBuffB2->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB2->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�s��Ǝ��_���W�֌W)
//	ConstBufferDataB0* constMap = nullptr;
//	result = objectBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	objectBuffer.constBuffB0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�}�e���A���֌W)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = objectBuffer.constBuffB1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = objectBuffer.constBuffDataB1.ambient;
//	constMap1->diffuse = objectBuffer.constBuffDataB1.diffuse;
//	constMap1->specular = objectBuffer.constBuffDataB1.specular;
//	constMap1->alpha = objectBuffer.constBuffDataB1.alpha;
//	objectBuffer.constBuffB1->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(���C�g�֌W)
//	ConstBufferDataB2Shadow* constMap2 = nullptr;
//	result = objectBuffer.constBuffB2->Map(0, nullptr, (void**)&constMap2);
//	for (int i = 0; i < DIRLIGHT_NUM; ++i) {
//		constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
//	}
//	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
//		constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
//	}
//	for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
//		constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
//	}
//	for (int i = 0; i < CIRCLESHADOW_NUM; ++i) {
//		constMap2->circleShadowData[i] = LightManager::Ins()->circleShadows[i];
//	}
//	objectBuffer.constBuffB2->Unmap(0, nullptr);
//
//	return objectBuffer;
//}
//
//Object3D ObjectManager::GenerateModelObjInstance(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string modelFileName, LPCWSTR textureFileName, bool isSmoothing)
//{
//	//���̕ϐ�
//	Object3D objectBuffer;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	objectBuffer.basicdata.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 3;										//CBV3��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&objectBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	objectBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(textureFileName));
//
//	//obj�t�@�C�������[�h
//	ModelDataManager::LoadObj(directoryPath, modelFileName, objectBuffer, isSmoothing);
//
//	//���_�o�b�t�@�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	objectBuffer.vbView.BufferLocation = objectBuffer.vertBuff->GetGPUVirtualAddress();
//	objectBuffer.vbView.SizeInBytes = objectBuffer.vertex.size() * sizeof(Vertex);
//	objectBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	objectBuffer.ibView.BufferLocation = objectBuffer.indexBuff->GetGPUVirtualAddress();
//	objectBuffer.ibView.Format = DXGI_FORMAT_R16_UINT;
//	objectBuffer.ibView.SizeInBytes = objectBuffer.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(((sizeof(ConstBufferDataB0) * INSTANCE_COUNT) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB0)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB1)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB2) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.constBuffB2)
//	);
//
//	//�s���������
//	objectBuffer.basicdata.projectionID = projectionID;
//	objectBuffer.rotationMat = XMMatrixIdentity();
//	objectBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	objectBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	objectBuffer.pos = XMFLOAT3(objectBuffer.positionMat.r[3].m128_f32[0], objectBuffer.positionMat.r[3].m128_f32[1], objectBuffer.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	objectBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//�S���_�ɑ΂���
//	for (int i = 0; i < objectBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = objectBuffer.vertex.at(i);   // ���W���R�s�[
//	}
//	//�}�b�v������
//	objectBuffer.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (objectBuffer.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = objectBuffer.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < objectBuffer.index.size(); ++i) {
//			indexMap[i] = objectBuffer.index.at(i);
//		}
//		objectBuffer.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = objectBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = objectBuffer.constBuffB1->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB1->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		objectBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 2, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = objectBuffer.constBuffB2->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)objectBuffer.constBuffB2->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�s��Ǝ��_���W�֌W)
//	ConstBufferDataB0* constMap = nullptr;
//	result = objectBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	objectBuffer.constBuffB0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�}�e���A���֌W)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = objectBuffer.constBuffB1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = objectBuffer.constBuffDataB1.ambient;
//	constMap1->diffuse = objectBuffer.constBuffDataB1.diffuse;
//	constMap1->specular = objectBuffer.constBuffDataB1.specular;
//	constMap1->alpha = objectBuffer.constBuffDataB1.alpha;
//	objectBuffer.constBuffB1->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(���C�g�֌W)
//	ConstBufferDataB2* constMap2 = nullptr;
//	result = objectBuffer.constBuffB2->Map(0, nullptr, (void**)&constMap2);
//	for (int i = 0; i < DIRLIGHT_NUM; ++i) {
//		constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
//	}
//	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
//		constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
//	}
//	for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
//		//constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
//	}
//	objectBuffer.constBuffB2->Unmap(0, nullptr);
//
//	return objectBuffer;
//}

//GSParticleData ObjectManager::GenerateGSParticle(XMFLOAT3 pos, int piplineID, XMFLOAT4 color, LPCWSTR fileName)
//{
//	GSParticleData particleBuffer{};
//
//	//�p�C�v�����̖��O�̕ۑ�
//	particleBuffer.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 1;										//CBV1��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&particleBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName == 0) {
//		particleBuffer.textureID = TextureManager::Ins()->CreateTexture(color);
//	}
//	else {
//		particleBuffer.textureID = TextureManager::Ins()->LoadTexture(fileName);
//	}
//
//	//���_�o�b�t�@�̐���
//	particleBuffer.vertex = {};
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMFLOAT3)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	particleBuffer.vbView.BufferLocation = particleBuffer.vertBuff->GetGPUVirtualAddress();
//	particleBuffer.vbView.SizeInBytes = sizeof(XMFLOAT3);
//	particleBuffer.vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.constBuffB0)
//	);
//
//	//�s���������
//	particleBuffer.rotationMat = XMMatrixIdentity();
//	particleBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	particleBuffer.positionMat = XMMatrixTranslation(pos.x, pos.y, pos.z);
//
//	//�}�b�v�������s��
//	XMFLOAT3* vertMap = nullptr;
//	particleBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	vertMap = &particleBuffer.vertex;		//���W���R�s�[
//	//�}�b�v������
//	particleBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		particleBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = particleBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)particleBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = particleBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= particleBuffer.scaleMat;
//	matWorld *= particleBuffer.rotationMat;
//	matWorld *= particleBuffer.positionMat;
//	constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//	constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//	constMap->eye = Camera::eye;
//	particleBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return particleBuffer;
//}
//
//GSParticleData ObjectManager::GenerateGSParticleInstance(XMFLOAT3 pos, int piplineID, XMFLOAT4 color, LPCWSTR fileName)
//{
//	GSParticleData particleBuffer{};
//
//	//�p�C�v�����̖��O�̕ۑ�
//	particleBuffer.piplineID = piplineID;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 1;										//CBV1��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&particleBuffer.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName == 0) {
//		particleBuffer.textureID = TextureManager::Ins()->CreateTexture(color);
//	}
//	else {
//		particleBuffer.textureID = TextureManager::Ins()->LoadTexture(fileName);
//	}
//
//	//���_�o�b�t�@�̐���
//	particleBuffer.vertex = {};
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMFLOAT3)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	particleBuffer.vbView.BufferLocation = particleBuffer.vertBuff->GetGPUVirtualAddress();
//	particleBuffer.vbView.SizeInBytes = sizeof(XMFLOAT3);
//	particleBuffer.vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(((sizeof(GSConstBufferDataB0) * INSTANCE_COUNT) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.constBuffB0)
//	);
//
//	//�s���������
//	particleBuffer.rotationMat = XMMatrixIdentity();
//	particleBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	particleBuffer.positionMat = XMMatrixTranslation(pos.x, pos.y, pos.z);
//
//	//�}�b�v�������s��
//	XMFLOAT3* vertMap = nullptr;
//	particleBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	vertMap = &particleBuffer.vertex;		//���W���R�s�[
//	//�}�b�v������
//	particleBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		particleBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = particleBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)particleBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	GSConstBufferDataB0* constMap = nullptr;
//	result = particleBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= particleBuffer.scaleMat;
//	matWorld *= particleBuffer.rotationMat;
//	matWorld *= particleBuffer.positionMat;
//	constMap->world = matWorld * (Camera::matView * Camera::matPerspective);												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//	constMap->color = XMFLOAT4(1, 1, 1, 1);
//	particleBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return particleBuffer;
//}

//void ObjectManager::DrawLine(Line& data)
//{
//	//��\����Ԃ�������`�揈�����s��Ȃ�
//	if (data.basicdata.isDisplay == false) return;
//
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
//	if (data.basicdata.projectionID == PROJECTIONID_UI) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
//	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//�r���{�[�h�̏ꍇ
//	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
//		//���_���W
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//�����_���W
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(����)�����
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//�J����Z��
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//�x�N�g���𐳋K��
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//�J������X������(�E����)
//		XMVECTOR cameraAxisX{};
//		//X���͏����->Z���̊O�ςŋ��܂�
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//�x�N�g���𐳋K��
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//�J������Y��(�����)
//		XMVECTOR cameraAxisY{};
//		//Y����Z��->X���̊O�ςŋ��߂�
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//�x�N�g���𐳋K��
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//�J������]�s��
//		XMMATRIX matCameraRot;
//		//�J�������W�n->���[���h���W�n�̕Ԋҍs��
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//�]�n�ɂ��t�s��(�t��])���v�Z
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//���_���W��-1�����������W
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
//		//��̃x�N�g���ɂ܂Ƃ߂�
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//�r���[�s��ɕ��s�ړ�����ݒ�
//		matView.r[3] = translation;
//
//		//�r���{�[�h�s��
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
//	}
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//�`��R�}���h
//	//DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}

//void ObjectManager::DrawSprite(Sprite& data)
//{
//	//��\����Ԃ�������`�揈�����s��Ȃ�
//	if (data.basicdata.isDisplay == false) return;
//
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
//	if (data.basicdata.projectionID == PROJECTIONID_UI) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
//	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//�r���{�[�h�̏ꍇ
//	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
//		//���_���W
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//�����_���W
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(����)�����
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//�J����Z��
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//�x�N�g���𐳋K��
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//�J������X������(�E����)
//		XMVECTOR cameraAxisX{};
//		//X���͏����->Z���̊O�ςŋ��܂�
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//�x�N�g���𐳋K��
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//�J������Y��(�����)
//		XMVECTOR cameraAxisY{};
//		//Y����Z��->X���̊O�ςŋ��߂�
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//�x�N�g���𐳋K��
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//�J������]�s��
//		XMMATRIX matCameraRot;
//		//�J�������W�n->���[���h���W�n�̕Ԋҍs��
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//�]�n�ɂ��t�s��(�t��])���v�Z
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//���_���W��-1�����������W
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
//		//��̃x�N�g���ɂ܂Ƃ߂�
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//�r���[�s��ɕ��s�ړ�����ݒ�
//		matView.r[3] = translation;
//
//		//�r���{�[�h�s��
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//���W��ۑ����Ă���
//	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
//	}
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//�`��R�}���h
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}

//void ObjectManager::DrawSpriteMultiPath(SpriteMultiPath& data)
//{
//	//��\����Ԃ�������`�揈�����s��Ȃ�
//	if (data.basicdata.isDisplay == false) return;
//
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
//	if (data.basicdata.projectionID == PROJECTIONID_UI) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
//	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//�r���{�[�h�̏ꍇ
//	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
//		//���_���W
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//�����_���W
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(����)�����
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//�J����Z��
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//�x�N�g���𐳋K��
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//�J������X������(�E����)
//		XMVECTOR cameraAxisX{};
//		//X���͏����->Z���̊O�ςŋ��܂�
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//�x�N�g���𐳋K��
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//�J������Y��(�����)
//		XMVECTOR cameraAxisY{};
//		//Y����Z��->X���̊O�ςŋ��߂�
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//�x�N�g���𐳋K��
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//�J������]�s��
//		XMMATRIX matCameraRot;
//		//�J�������W�n->���[���h���W�n�̕Ԋҍs��
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//�]�n�ɂ��t�s��(�t��])���v�Z
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//���_���W��-1�����������W
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
//		//��̃x�N�g���ɂ܂Ƃ߂�
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//�r���[�s��ɕ��s�ړ�����ݒ�
//		matView.r[3] = translation;
//
//		//�r���{�[�h�s��
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@2�̃f�[�^�]��
//	ConstBufferDataMultiPath* constMap2 = nullptr;
//	result = data.constBuffB1->Map(0, nullptr, (void**)&constMap2);
//	constMap2->gaussianWeight[0] = data.constBufferDataB1.gaussianWeight[0];
//	constMap2->gaussianWeight[1] = data.constBufferDataB1.gaussianWeight[1];
//	data.constBuffB1->Unmap(0, nullptr);
//
//	//���W��ۑ����Ă���
//	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, data.constBuffB1->GetGPUVirtualAddress());
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 2, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
//	}
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//�`��R�}���h
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}
//
//void ObjectManager::DrawSpriteMultiTexture(SpriteMultiTexture& data)
//{
//	//��\����Ԃ�������`�揈�����s��Ȃ�
//	if (data.isDisplay == false) return;
//
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(data.piplineID);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
//	if (data.projectionID == PROJECTIONID_UI) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
//	else if (data.projectionID == PROJECTIONID_OBJECT) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//�r���{�[�h�̏ꍇ
//	else if (data.projectionID == PROJECTIONID_BILLBOARD) {
//		//���_���W
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//�����_���W
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(����)�����
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//�J����Z��
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//�x�N�g���𐳋K��
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//�J������X������(�E����)
//		XMVECTOR cameraAxisX{};
//		//X���͏����->Z���̊O�ςŋ��܂�
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//�x�N�g���𐳋K��
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//�J������Y��(�����)
//		XMVECTOR cameraAxisY{};
//		//Y����Z��->X���̊O�ςŋ��߂�
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//�x�N�g���𐳋K��
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//�J������]�s��
//		XMMATRIX matCameraRot;
//		//�J�������W�n->���[���h���W�n�̕Ԋҍs��
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//�]�n�ɂ��t�s��(�t��])���v�Z
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//���_���W��-1�����������W
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
//		//��̃x�N�g���ɂ܂Ƃ߂�
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//�r���[�s��ɕ��s�ړ�����ݒ�
//		matView.r[3] = translation;
//
//		//�r���{�[�h�s��
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//���W��ۑ����Ă���
//	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//	for (int i = 0; i < data.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(data.textureID.at(i)));
//	}
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//�`��R�}���h
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}
//
////void ObjectManager::DrawObject(Object3D& data)
////{
////	//��\����Ԃ�������`�揈�����s��Ȃ�
////	if (data.basicdata.isDisplay == false) return;
////
////	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
////	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
////
////	//�萔�o�b�t�@�ւ̃f�[�^�]��
////	ConstBufferDataB0* constMap = nullptr;
////	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
////	//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
////	if (data.basicdata.projectionID == PROJECTIONID_UI) {
////		//���[���h�s��̍X�V
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;
////		constMap->mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
////	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
////		//���[���h�s��̍X�V
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
////		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//�r���{�[�h�̏ꍇ
////	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
////		//���_���W
////		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
////		//�����_���W
////		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
////		//(����)�����
////		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
////		//�J����Z��
////		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
////		//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
////		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
////		assert(!XMVector3IsInfinite(cameraAxisZ));
////		assert(!XMVector3Equal(upVector, XMVectorZero()));
////		assert(!XMVector3IsInfinite(upVector));
////		//�x�N�g���𐳋K��
////		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
////		//�J������X������(�E����)
////		XMVECTOR cameraAxisX{};
////		//X���͏����->Z���̊O�ςŋ��܂�
////		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
////		//�x�N�g���𐳋K��
////		cameraAxisX = XMVector3Normalize(cameraAxisX);
////		//�J������Y��(�����)
////		XMVECTOR cameraAxisY{};
////		//Y����Z��->X���̊O�ςŋ��߂�
////		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
////		//�x�N�g���𐳋K��
////		cameraAxisY = XMVector3Normalize(cameraAxisY);
////		//�J������]�s��
////		XMMATRIX matCameraRot;
////		//�J�������W�n->���[���h���W�n�̕Ԋҍs��
////		matCameraRot.r[0] = cameraAxisX;
////		matCameraRot.r[1] = cameraAxisY;
////		matCameraRot.r[2] = cameraAxisZ;
////		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
////		//�]�n�ɂ��t�s��(�t��])���v�Z
////		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
////		//���_���W��-1�����������W
////		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
////		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
////		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
////		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
////		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
////		//��̃x�N�g���ɂ܂Ƃ߂�
////		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
////		//�r���[�s��ɕ��s�ړ�����ݒ�
////		matView.r[3] = translation;
////
////		//�r���{�[�h�s��
////		XMMATRIX billboardMat = XMMatrixIdentity();
////		billboardMat.r[0] = cameraAxisX;
////		billboardMat.r[1] = cameraAxisY;
////		billboardMat.r[2] = cameraAxisZ;
////		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
////		//���[���h�s��̍X�V
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= billboardMat;
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
////		constMap->mat.viewproj = matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	data.constBuffB0->Unmap(0, nullptr);
////
////	//�萔�o�b�t�@�̐����Q�ȏ�(�}�e���A���������Ă���)�Ȃ�Q�ڂ̒萔�o�b�t�@�̃}�b�v�������s��
////	ConstBufferDataB1* constMap1 = nullptr;
////	result = data.constBuffB1->Map(0, nullptr, (void**)&constMap1);
////	constMap1->ambient = data.constBuffDataB1.ambient;
////	constMap1->diffuse = data.constBuffDataB1.diffuse;
////	constMap1->specular = data.constBuffDataB1.specular;
////	constMap1->alpha = data.constBuffDataB1.alpha;
////	data.constBuffB1->Unmap(0, nullptr);
////
////	ConstBufferDataB2* constMap2 = nullptr;
////	result = data.constBuffB2->Map(0, nullptr, (void**)&constMap2);
////	for (int i = 0; i < DIRLIGHT_NUM; ++i) {
////		constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
////	}
////	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
////		constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
////	}
////	for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
////		constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
////	}
////
////	data.constBuffB2->Unmap(0, nullptr);
////
////	//���W��ۑ����Ă���
////	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
////
////	//�萔�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, data.constBuffB1->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, data.constBuffB2->GetGPUVirtualAddress());
////
////	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h
////	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
////	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
////	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
////	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
////		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 3, TextureManager::Ins()->GetSRV(data.basicdata.textureID[i]));
////	}
////
////	//���_�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
////
////	//���_�C���f�b�N�X�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->IASetIndexBuffer(&data.ibView);
////
////	//�`��R�}���h
////	DirectXBase::Ins()->cmdList->DrawIndexedInstanced(data.index.size(), 1, 0, 0, 0);
////}
////
////void ObjectManager::DrawObject(Object3DShadow& data)
////{
////	//��\����Ԃ�������`�揈�����s��Ȃ�
////	if (data.basicdata.isDisplay == false) return;
////
////	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
////	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
////
////	//�萔�o�b�t�@�ւ̃f�[�^�]��
////	ConstBufferDataB0* constMap = nullptr;
////	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
////	//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
////	if (data.basicdata.projectionID == PROJECTIONID_UI) {
////		//���[���h�s��̍X�V
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;
////		constMap->mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
////	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
////		//���[���h�s��̍X�V
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
////		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//�r���{�[�h�̏ꍇ
////	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
////		//���_���W
////		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
////		//�����_���W
////		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
////		//(����)�����
////		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
////		//�J����Z��
////		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
////		//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
////		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
////		assert(!XMVector3IsInfinite(cameraAxisZ));
////		assert(!XMVector3Equal(upVector, XMVectorZero()));
////		assert(!XMVector3IsInfinite(upVector));
////		//�x�N�g���𐳋K��
////		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
////		//�J������X������(�E����)
////		XMVECTOR cameraAxisX{};
////		//X���͏����->Z���̊O�ςŋ��܂�
////		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
////		//�x�N�g���𐳋K��
////		cameraAxisX = XMVector3Normalize(cameraAxisX);
////		//�J������Y��(�����)
////		XMVECTOR cameraAxisY{};
////		//Y����Z��->X���̊O�ςŋ��߂�
////		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
////		//�x�N�g���𐳋K��
////		cameraAxisY = XMVector3Normalize(cameraAxisY);
////		//�J������]�s��
////		XMMATRIX matCameraRot;
////		//�J�������W�n->���[���h���W�n�̕Ԋҍs��
////		matCameraRot.r[0] = cameraAxisX;
////		matCameraRot.r[1] = cameraAxisY;
////		matCameraRot.r[2] = cameraAxisZ;
////		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
////		//�]�n�ɂ��t�s��(�t��])���v�Z
////		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
////		//���_���W��-1�����������W
////		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
////		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
////		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
////		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
////		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
////		//��̃x�N�g���ɂ܂Ƃ߂�
////		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
////		//�r���[�s��ɕ��s�ړ�����ݒ�
////		matView.r[3] = translation;
////
////		//�r���{�[�h�s��
////		XMMATRIX billboardMat = XMMatrixIdentity();
////		billboardMat.r[0] = cameraAxisX;
////		billboardMat.r[1] = cameraAxisY;
////		billboardMat.r[2] = cameraAxisZ;
////		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
////		//���[���h�s��̍X�V
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= billboardMat;
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
////		constMap->mat.viewproj = matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	data.constBuffB0->Unmap(0, nullptr);
////
////	//�萔�o�b�t�@�̐����Q�ȏ�(�}�e���A���������Ă���)�Ȃ�Q�ڂ̒萔�o�b�t�@�̃}�b�v�������s��
////	ConstBufferDataB1* constMap1 = nullptr;
////	result = data.constBuffB1->Map(0, nullptr, (void**)&constMap1);
////	constMap1->ambient = data.constBuffDataB1.ambient;
////	constMap1->diffuse = data.constBuffDataB1.diffuse;
////	constMap1->specular = data.constBuffDataB1.specular;
////	constMap1->alpha = data.constBuffDataB1.alpha;
////	data.constBuffB1->Unmap(0, nullptr);
////
////	ConstBufferDataB2Shadow* constMap2 = nullptr;
////	result = data.constBuffB2->Map(0, nullptr, (void**)&constMap2);
////	for (int i = 0; i < DIRLIGHT_NUM; ++i) {
////		constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
////	}
////	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
////		constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
////	}
////	for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
////		constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
////	}
////	for (int i = 0; i < CIRCLESHADOW_NUM; ++i) {
////		constMap2->circleShadowData[i] = LightManager::Ins()->circleShadows[i];
////	}
////
////	data.constBuffB2->Unmap(0, nullptr);
////
////	//���W��ۑ����Ă���
////	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
////
////	//�萔�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, data.constBuffB1->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, data.constBuffB2->GetGPUVirtualAddress());
////
////	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h
////	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
////	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
////	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
////	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
////		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 3, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
////	}
////
////	//���_�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
////
////	//���_�C���f�b�N�X�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->IASetIndexBuffer(&data.ibView);
////
////	//�`��R�}���h
////	DirectXBase::Ins()->cmdList->DrawIndexedInstanced(data.index.size(), 1, 0, 0, 0);
////}
////
////void ObjectManager::DrawObjectInstance(const Object3D& instanceData, const vector<Object3D>& data, const int& piplineID)
////{
////	//�C���X�^���X�����擾
////	int instanceCount = data.size();
////
////	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
////	PiplineManager::Ins()->SetPipline(piplineID);
////
////	//�萔�o�b�t�@�ւ̃f�[�^�]��
////	ConstBufferDataB0* constMap = nullptr;
////	HRESULT result = instanceData.constBuffB0->Map(0, nullptr, (void**)&constMap);
////
////	for (int i = 0; i < instanceCount; ++i) {
////		//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
////		if (data[i].basicdata.projectionID == PROJECTIONID_UI) {
////			//���[���h�s��̍X�V
////			XMMATRIX matWorld = XMMatrixIdentity();
////			matWorld *= data[i].scaleMat;
////			matWorld *= data[i].rotationMat;
////			matWorld *= data[i].positionMat;
////			constMap[i].mat.world = matWorld;
////			constMap[i].mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
////			constMap[i].eye = Camera::eye;
////			constMap[i].color = data[i].constBuffDataB0.color;
////		}
////		//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
////		else if (data[i].basicdata.projectionID == PROJECTIONID_OBJECT) {
////			//���[���h�s��̍X�V
////			XMMATRIX matWorld = XMMatrixIdentity();
////			matWorld *= data[i].scaleMat;
////			matWorld *= data[i].rotationMat;
////			matWorld *= data[i].positionMat;
////			constMap[i].mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
////			constMap[i].mat.viewproj = Camera::matView * Camera::matPerspective;
////			constMap[i].eye = Camera::eye;
////			constMap[i].color = data[i].constBuffDataB0.color;
////		}
////		//�r���{�[�h�̏ꍇ
////		else if (data[i].basicdata.projectionID == PROJECTIONID_BILLBOARD) {
////			//���_���W
////			XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
////			//�����_���W
////			XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
////			//(����)�����
////			XMVECTOR upVector = XMLoadFloat3(&Camera::up);
////			//�J����Z��
////			XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
////			//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
////			assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
////			assert(!XMVector3IsInfinite(cameraAxisZ));
////			assert(!XMVector3Equal(upVector, XMVectorZero()));
////			assert(!XMVector3IsInfinite(upVector));
////			//�x�N�g���𐳋K��
////			cameraAxisZ = XMVector3Normalize(cameraAxisZ);
////			//�J������X������(�E����)
////			XMVECTOR cameraAxisX{};
////			//X���͏����->Z���̊O�ςŋ��܂�
////			cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
////			//�x�N�g���𐳋K��
////			cameraAxisX = XMVector3Normalize(cameraAxisX);
////			//�J������Y��(�����)
////			XMVECTOR cameraAxisY{};
////			//Y����Z��->X���̊O�ςŋ��߂�
////			cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
////			//�x�N�g���𐳋K��
////			cameraAxisY = XMVector3Normalize(cameraAxisY);
////			//�J������]�s��
////			XMMATRIX matCameraRot;
////			//�J�������W�n->���[���h���W�n�̕Ԋҍs��
////			matCameraRot.r[0] = cameraAxisX;
////			matCameraRot.r[1] = cameraAxisY;
////			matCameraRot.r[2] = cameraAxisZ;
////			matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
////			//�]�n�ɂ��t�s��(�t��])���v�Z
////			XMMATRIX matView = XMMatrixTranspose(matCameraRot);
////			//���_���W��-1�����������W
////			XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
////			//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
////			XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
////			XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
////			XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
////			//��̃x�N�g���ɂ܂Ƃ߂�
////			XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
////			//�r���[�s��ɕ��s�ړ�����ݒ�
////			matView.r[3] = translation;
////
////			//�r���{�[�h�s��
////			XMMATRIX billboardMat = XMMatrixIdentity();
////			billboardMat.r[0] = cameraAxisX;
////			billboardMat.r[1] = cameraAxisY;
////			billboardMat.r[2] = cameraAxisZ;
////			billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
////			//���[���h�s��̍X�V
////			XMMATRIX matWorld = XMMatrixIdentity();
////			matWorld *= billboardMat;
////			matWorld *= data[i].scaleMat;
////			matWorld *= data[i].rotationMat;
////			matWorld *= data[i].positionMat;
////			constMap[i].mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
////			constMap[i].mat.viewproj = matView * Camera::matPerspective;
////			constMap[i].eye = Camera::eye;
////			constMap[i].color = data[i].constBuffDataB0.color;
////		}
////	}
////
////	instanceData.constBuffB0->Unmap(0, nullptr);
////
////	//�萔�o�b�t�@�̐����Q�ȏ�(�}�e���A���������Ă���)�Ȃ�Q�ڂ̒萔�o�b�t�@�̃}�b�v�������s��
////	ConstBufferDataB1* constMap1 = nullptr;
////	result = instanceData.constBuffB1->Map(0, nullptr, (void**)&constMap1);
////	constMap1->ambient = instanceData.constBuffDataB1.ambient;
////	constMap1->diffuse = instanceData.constBuffDataB1.diffuse;
////	constMap1->specular = instanceData.constBuffDataB1.specular;
////	constMap1->alpha = instanceData.constBuffDataB1.alpha;
////	instanceData.constBuffB1->Unmap(0, nullptr);
////
////	ConstBufferDataB2Shadow* constMap2 = nullptr;
////	result = instanceData.constBuffB2->Map(0, nullptr, (void**)&constMap2);
////	for (int i = 0; i < DIRLIGHT_NUM; ++i) {
////		constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
////	}
////	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
////		constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
////	}
////	for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
////		constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
////	}
////	/*for (int i = 0; i < CIRCLESHADOW_NUM; ++i) {
////		constMap2->circleShadowData[i] = LightManager::Ins()->circleShadows[i];
////	}*/
////
////	instanceData.constBuffB2->Unmap(0, nullptr);
////
////	//�萔�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, instanceData.constBuffB0->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, instanceData.constBuffB1->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, instanceData.constBuffB2->GetGPUVirtualAddress());
////
////	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h
////	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
////	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
////	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
////	for (int i = 0; i < instanceData.basicdata.textureID.size(); ++i) {
////		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 3, TextureManager::Ins()->GetSRV(instanceData.basicdata.textureID[0]));
////	}
////
////	//���_�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &instanceData.vbView);
////
////	//���_�C���f�b�N�X�o�b�t�@�r���[�ݒ�R�}���h
////	DirectXBase::Ins()->cmdList->IASetIndexBuffer(&instanceData.ibView);
////
////	//�`��R�}���h
////	DirectXBase::Ins()->cmdList->DrawIndexedInstanced(instanceData.index.size(), instanceCount, 0, 0, 0);
////}
//
//void ObjectManager::DrawGSParticle(GSParticleData& data)
//{
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(data.piplineID);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//���[���h�s��̍X�V
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= data.scaleMat;
//	matWorld *= data.rotationMat;
//	matWorld *= data.positionMat;
//	constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//	constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//	constMap->eye = Camera::eye;
//	constMap->color = data.constBuffDataB0.color;
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h		�X�v���C�g���e�N�X�`���̃f�[�^�������Ă����ꍇ�̂ݐݒ�R�}���h�����s����
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Ins()->GetSRV(data.textureID));
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//�`��R�}���h
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(1, 1, 0, 0);
//}
//
//void ObjectManager::DrawGSParticleInstance(const GSParticleData& instanceData, vector<ParticleData*>& data, const int& piplinID) {
//
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(piplinID);
//
//	int instanceCount = data.size();
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	GSConstBufferDataB0* constMap = nullptr;
//	HRESULT result = instanceData.constBuffB0->Map(0, nullptr, (void**)&constMap);
//
//	for (int i = 0; i < instanceCount; ++i) {
//
//		if (data[i] == nullptr) continue;
//
//		if (data[i]->dirtyFlag) continue;
//
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data[i]->matScale;
//		matWorld *= data[i]->matRot;
//		matWorld *= data[i]->matTrans;
//		data[i]->dirtyFlag = true;
//		constMap[i].world = matWorld * (Camera::matView * Camera::matPerspective);												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap[i].color = instanceData.constBuffDataB0.color;
//
//	}
//
//	instanceData.constBuffB0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, instanceData.constBuffB0->GetGPUVirtualAddress());
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h		�X�v���C�g���e�N�X�`���̃f�[�^�������Ă����ꍇ�̂ݐݒ�R�}���h�����s����
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Ins()->GetSRV(instanceData.textureID));
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &instanceData.vbView);
//
//	//�`��R�}���h
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(1, instanceCount, 0, 0);
//}
//
//void ObjectManager::ChangeScale(XMMATRIX& scaleMat, XMFLOAT3 amount)
//{
//	scaleMat = XMMatrixScaling(amount.x, amount.y, amount.z);
//}
//
//void ObjectManager::ChangeRotation(XMMATRIX& rotationMat, XMFLOAT3 amount)
//{
//	XMMATRIX buff = XMMatrixIdentity();
//	buff *= XMMatrixRotationZ(amount.z);
//	buff *= XMMatrixRotationX(amount.x);
//	buff *= XMMatrixRotationY(amount.y);
//	rotationMat = buff * rotationMat;
//}
//
//void ObjectManager::ChangeRotationInit(XMMATRIX& rotationMat)
//{
//	rotationMat = XMMatrixIdentity();
//}
//
//void ObjectManager::ChangePosition(XMMATRIX& positionMat, XMFLOAT3& pos, XMFLOAT3 amount)
//{
//	positionMat = XMMatrixTranslation(amount.x, amount.y, amount.z);
//	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
//}
//
//void ObjectManager::ChangePositionAdd(XMMATRIX& positionMat, XMFLOAT3& pos, XMFLOAT3 amount)
//{
//	positionMat *= XMMatrixTranslation(amount.x, amount.y, amount.z);
//	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
//}

//void ObjectManager::ChangeLinePosition(const XMFLOAT3& pos1, const XMFLOAT3& pos2, Line& line)
//{
//	line.vertex[0].pos = pos1;
//	line.vertex[1].pos = pos2;
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	line.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < line.vertex.size(); ++i)
//	{
//		vertMap[i] = line.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	line.vertBuff->Unmap(0, nullptr);
//
//	line.pos1 = pos1;
//	line.pos2 = pos2;
//
//}


#pragma region ���R�[�h
//SpriteManager::SpriteManager() {
//}
//
//Sprite SpriteManager::GenerateLine(XMFLOAT3 startPos, XMFLOAT3 endPos, int projectionID, string pipleName, XMFLOAT4 color)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	proSprite.hasTexture = true;
//
//	//���_�o�b�t�@�̐���
//	Vertex vertex;
//	vertex.pos = startPos;	//���_
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = endPos;	//�I�_
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//���_�C���f�b�N�X�̐ݒ�
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(1);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixIdentity();
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//sprite��sprite�z��̍Ō�ɐ���
//	pair<string, Sprite> pre;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateLines(XMFLOAT3 startPos, XMFLOAT3 endPos, XMFLOAT3 directions, float lineInterval, int lineCount, int projectionID, string pipleName, XMFLOAT4 color)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	proSprite.hasTexture = true;
//
//	//���_�o�b�t�@�̐���
//	for (int i = 0; i < lineCount; ++i) {
//		Vertex vertex;
//		vertex.pos = XMFLOAT3(startPos.x + (lineInterval * i * directions.x), startPos.y + (lineInterval * i * directions.y), startPos.z + (lineInterval * i * directions.z));	//���_
//		vertex.normal = XMFLOAT3(0, 0, 0);
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(endPos.x + (lineInterval * i * directions.x), endPos.y + (lineInterval * i * directions.y), endPos.z + (lineInterval * i * directions.z));	//�I�_
//		vertex.uv = XMFLOAT2(0, 1);
//		proSprite.vertex.push_back(vertex);
//	}
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//���_�C���f�b�N�X�̐ݒ�
//	for (int i = 0; i < lineCount * 2; ++i) {
//		proSprite.index.push_back(i);
//	}
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixIdentity();
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//sprite��sprite�z��̍Ō�ɐ���
//	pair<string, Sprite> pre;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateLineBox(XMFLOAT3 centerPos, XMFLOAT3 size, float lineInterval, int lineCount, int projectionID, string pipleName, XMFLOAT4 color)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	proSprite.hasTexture = true;
//
//	Vertex vertex;
//	vertex.normal = XMFLOAT3(0, 0, 0);
//
//	//���_�o�b�t�@�̐���
//	for (int i = 0; i < lineCount + 1; ++i) {
//		/*���ʂ̒��_*/
//		//�c
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//��
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*��ʂ̒��_*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//�c
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//��
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*�E�ʂ̒��_*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//�c
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//��(��)
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*���ʂ̒��_*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//�c
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//��(��)
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*���ʂ̒��_*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//�c
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//��
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*��O�ʂ̒��_*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//�c
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//��
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//�n�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//�I�_
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//���_�C���f�b�N�X�̐ݒ�
//	for (int i = 0; i < proSprite.vertex.size(); ++i) {
//		proSprite.index.push_back(i);
//	}
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixIdentity();
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//sprite��sprite�z��̍Ō�ɐ���
//	pair<string, Sprite> pre;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::Generate2DTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, string pipleName, LPCWSTR fileName, UVs* uv) {
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = false;
//	}
//
//	//���_�o�b�t�@�̐���
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//����
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);	//����
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);	//�E��
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);	//�E��
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);
//
//	//������uv��nullptr����Ȃ���������
//	if (uv != nullptr) {
//		UVs uvBuff = *uv;
//		proSprite.vertex.at(0).uv = uvBuff.leftUp;
//		proSprite.vertex.at(1).uv = uvBuff.leftDown;
//		proSprite.vertex.at(2).uv = uvBuff.rightUp;
//		proSprite.vertex.at(3).uv = uvBuff.rightDown;
//	}
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//�@���x�N�g���̌v�Z
//	if (projectionID != backGround) {
//		for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//			//�O�p�`�ЂƂЂƂv�Z���Ă���
//			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
//			unsigned short index0 = proSprite.index.at(i * 3 + 0);
//			unsigned short index1 = proSprite.index.at(i * 3 + 1);
//			unsigned short index2 = proSprite.index.at(i * 3 + 2);
//			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
//			XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//			XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//			XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//			//p0->p1�x�N�g��,p0->p2�x�N�g�����v�Z
//			XMVECTOR v1 = XMVectorSubtract(p1, p0);
//			XMVECTOR v2 = XMVectorSubtract(p2, p0);
//			//�O�ʂ͗������琂���ȃx�N�g��
//			XMVECTOR normal = XMVector3Cross(v1, v2);
//			//���K��
//			normal = XMVector3Normalize(normal);
//			//���߂��@���𒸓_�f�[�^�ɑ��
//			XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//			XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//			XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//		}
//	}
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateBox(XMFLOAT3 centerPos, XMFLOAT3 size, string spriteName, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//���_�o�b�t�@�̐���
//#pragma region ���_������
//	Vertex vertex;
//	//�O
//	vertex.pos = XMFLOAT3(-size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(-size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//�E��
//	vertex.pos = XMFLOAT3(size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//�E��
//	//���
//	vertex.pos = XMFLOAT3(size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(-size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//�E��
//	vertex.pos = XMFLOAT3(-size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//�E��
//	//�E
//	vertex.pos = XMFLOAT3(size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//�E��
//	vertex.pos = XMFLOAT3(size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//�E��
//	//��
//	vertex.pos = XMFLOAT3(-size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(-size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(-size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//�E��
//	vertex.pos = XMFLOAT3(-size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//�E��
//	//��
//	vertex.pos = XMFLOAT3(-size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(-size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//�E��
//	vertex.pos = XMFLOAT3(size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//�E��
//	//��
//	vertex.pos = XMFLOAT3(-size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(-size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//����
//	vertex.pos = XMFLOAT3(size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//�E��
//	vertex.pos = XMFLOAT3(size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//�E��
//#pragma endregion
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//���_�C���f�b�N�X�̐ݒ�
//	int buff = 0;
//	for (int i = 0; i < 6; ++i) {
//		proSprite.index.push_back(0 + buff);
//		proSprite.index.push_back(1 + buff);
//		proSprite.index.push_back(2 + buff);
//		proSprite.index.push_back(2 + buff);
//		proSprite.index.push_back(1 + buff);
//		proSprite.index.push_back(3 + buff);
//		buff += 4;
//	}
//
//	//�@���x�N�g���̌v�Z
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//�O�p�`�ЂƂЂƂv�Z���Ă���
//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1�x�N�g��,p0->p2�x�N�g�����v�Z
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//�O�ʂ͗������琂���ȃx�N�g��
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//���K��
//		normal = XMVector3Normalize(normal);
//		//���߂��@���𒸓_�f�[�^�ɑ��
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//sprite��sprite�z��̍Ō�ɐ���
//	pair<string, Sprite> pre;
//	pre.first = spriteName;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateCone(XMFLOAT3 centerPos, float radius, float height, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//�}�`�����[�h
//	ShapeManager::Ins()->LoadShape(ShapeDataCone, proSprite);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(radius, radius, height);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateCylinder(XMFLOAT3 centerPos, float radius, float height, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//�}�`�����[�h
//	ShapeManager::Ins()->LoadShape(ShapeDataCylinder, proSprite);
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(radius, radius, height);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateSphere(XMFLOAT3 centerPos, float radius, int projectionID, string pipleName, XMFLOAT4 color, LPCWSTR fileName)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//�}�`�����[�h
//	ShapeManager::Ins()->LoadShape(ShapeDataSphere, proSprite);
//
//	//���a��ۑ�
//	proSprite.radius = radius;
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(radius, radius, radius);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// �S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	// �}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateModelFbx(XMFLOAT3 centerPos, string spriteName, int projectionID, string pipleName, const char* modelFileName, LPCWSTR textureFileName)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 2;										//CBV2��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (textureFileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(textureFileName);
//	}
//	else {
//		proSprite.hasTexture = false;
//	}
//
//	//fbx�t�@�C�������[�h
//	ModelDataManager::LoadFbx(modelFileName, proSprite);
//
//	//���_�o�b�t�@�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 1;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//�S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	//�}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	CD3DX12_GPU_DESCRIPTOR_HANDLE basicHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetGPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::GenerateModelObj(XMFLOAT3 centerPos, string spriteName, int projectionID, string pipleName, const char* modelFileName, LPCWSTR textureFileName, bool isSmoothing, bool isCircleShadow)
//{
//	//���̕ϐ�
//	Sprite proSprite;
//
//	//�p�C�v�����̖��O�̕ۑ�
//	proSprite.piplineName = pipleName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 3;										//CBV3��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	if (textureFileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(textureFileName);
//	}
//	else {
//		proSprite.hasTexture = false;
//	}
//
//	//fbx�t�@�C�������[�h
//	ModelDataManager::LoadObj(modelFileName, proSprite, isSmoothing);
//
//	//���_�o�b�t�@�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//���_�o�b�t�@�r���[�̐ݒ�
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----���_�C���f�b�N�X�o�b�t�@�̐ݒ�-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//���_�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	proSprite.constBuffCount = 3;
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff0)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff1)
//	);
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB2) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.constBuff2)
//	);
//
//	//�s���������
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//�}�b�v�������s��
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//�S���_�ɑ΂���
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // ���W���R�s�[
//	}
//	//�}�b�v������
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ�o�b�t�@�̃f�[�^�]�����s��
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = proSprite.constBuff1->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff1->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 2, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = proSprite.constBuff2->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff2->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�ۉe�̐���
//	proSprite.circleShadowNum = -1;
//	if (isCircleShadow == true) {
//		proSprite.circleShadowNum = LightManager::Ins()->GenerateCircleShadow(XMFLOAT3(0, 1, 0), proSprite.pos, 100.0f, XMFLOAT3(0.5f, 0.6f, 0.0f), XMFLOAT2(10.0f, 20.0f), 1);
//	}
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�s��Ǝ��_���W�֌W)
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(�}�e���A���֌W)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = proSprite.constBuff1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = proSprite.constBuffDataB1.ambient;
//	constMap1->diffuse = proSprite.constBuffDataB1.diffuse;
//	constMap1->specular = proSprite.constBuffDataB1.specular;
//	constMap1->alpha = proSprite.constBuffDataB1.alpha;
//	proSprite.constBuff1->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��(���C�g�֌W)
//	ConstBufferDataB2* constMap2 = nullptr;
//	result = proSprite.constBuff2->Map(0, nullptr, (void**)&constMap2);
//	for (int i = 0; i < DIRLIGHT_NUM; ++i) {
//		constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
//	}
//	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
//		constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
//	}
//	for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
//		constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
//	}
//	for (int i = 0; i < CIRCLESHADOW_NUM; ++i) {
//		if (proSprite.circleShadowNum != -1) {
//			//�e�̃L���X�g���X�V
//			LightManager::Ins()->circleShadows[proSprite.circleShadowNum].centerpos = XMFLOAT3(proSprite.pos.x, proSprite.pos.y + 280, proSprite.pos.z);
//		}
//		constMap2->circleShadowData[i] = LightManager::Ins()->circleShadows[i];
//	}
//	proSprite.constBuff2->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//void SpriteManager::Draw(Sprite& sprite)
//{
//	//��\����Ԃ�������`�揈�����s��Ȃ�
//	if (sprite.isDisplay == false) return;
//
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(sprite.piplineName);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��	�l���ύX����Ă������̂݃f�[�^�]�����s��
//	//if (sprite.isDirty == true) {
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = sprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	//���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
//	if (sprite.projectionID == backGround) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= sprite.scaleMat;
//		matWorld *= sprite.rotationMat;
//		matWorld *= sprite.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//���s���e�ϊ�
//		constMap->eye = Camera::eye;
//	}
//	//���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
//	else if (sprite.projectionID == object) {
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= sprite.scaleMat;
//		matWorld *= sprite.rotationMat;
//		matWorld *= sprite.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//	}
//	//�r���{�[�h�̏ꍇ
//	else if (sprite.projectionID == billBoard) {
//		//���_���W
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//�����_���W
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(����)�����
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//�J����Z��
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//�x�N�g���𐳋K��
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//�J������X������(�E����)
//		XMVECTOR cameraAxisX{};
//		//X���͏����->Z���̊O�ςŋ��܂�
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//�x�N�g���𐳋K��
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//�J������Y��(�����)
//		XMVECTOR cameraAxisY{};
//		//Y����Z��->X���̊O�ςŋ��߂�
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//�x�N�g���𐳋K��
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//�J������]�s��
//		XMMATRIX matCameraRot;
//		//�J�������W�n->���[���h���W�n�̕Ԋҍs��
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//�]�n�ɂ��t�s��(�t��])���v�Z
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//���_���W��-1�����������W
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X����
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y����
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z����
//		//��̃x�N�g���ɂ܂Ƃ߂�
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//�r���[�s��ɕ��s�ړ�����ݒ�
//		matView.r[3] = translation;
//
//		//�r���{�[�h�s��
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//���[���h�s��̍X�V
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= sprite.scaleMat;
//		matWorld *= sprite.rotationMat;
//		matWorld *= sprite.positionMat;
//		constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//	}
//	sprite.constBuff0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�̐����Q�ȏ�(�}�e���A���������Ă���)�Ȃ�Q�ڂ̒萔�o�b�t�@�̃}�b�v�������s��
//	if (sprite.constBuffCount >= 2) {
//		ConstBufferDataB1* constMap1 = nullptr;
//		result = sprite.constBuff1->Map(0, nullptr, (void**)&constMap1);
//		constMap1->ambient = sprite.constBuffDataB1.ambient;
//		constMap1->diffuse = sprite.constBuffDataB1.diffuse;
//		constMap1->specular = sprite.constBuffDataB1.specular;
//		constMap1->alpha = sprite.constBuffDataB1.alpha;
//		sprite.constBuff1->Unmap(0, nullptr);
//	}
//	if (sprite.constBuffCount >= 3) {
//		ConstBufferDataB2* constMap2 = nullptr;
//		result = sprite.constBuff2->Map(0, nullptr, (void**)&constMap2);
//		for (int i = 0; i < DIRLIGHT_NUM; ++i) {
//			constMap2->dirlights[i] = LightManager::Ins()->dirlights[i];
//		}
//		for (int i = 0; i < POINTLIGHT_NUM; ++i) {
//			constMap2->pointlights[i] = LightManager::Ins()->pointlights[i];
//		}
//		for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
//			constMap2->spotlights[i] = LightManager::Ins()->spotlights[i];
//		}
//		for (int i = 0; i < CIRCLESHADOW_NUM; ++i) {
//			if (sprite.circleShadowNum != -1) {
//				//�e�̃L���X�g���X�V
//				LightManager::Ins()->circleShadows[sprite.circleShadowNum].centerpos = XMFLOAT3(sprite.pos.x, sprite.pos.y + 280, sprite.pos.z);
//			}
//			constMap2->circleShadowData[i] = LightManager::Ins()->circleShadows[i];
//		}
//		sprite.constBuff2->Unmap(0, nullptr);
//	}
//
//	//�_�[�e�B�t���O��false�ɂ���
//	//sprite.isDirty = false;
//	//}
//
//	//���W��ۑ����Ă���
//	sprite.pos = XMFLOAT3(sprite.positionMat.r[3].m128_f32[0], sprite.positionMat.r[3].m128_f32[1], sprite.positionMat.r[3].m128_f32[2]);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, sprite.constBuff0->GetGPUVirtualAddress());
//	if (sprite.constBuffCount >= 2) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, sprite.constBuff1->GetGPUVirtualAddress());
//	}
//	if (sprite.constBuffCount >= 3) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, sprite.constBuff2->GetGPUVirtualAddress());
//	}
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h		�X�v���C�g���e�N�X�`���̃f�[�^�������Ă����ꍇ�̂ݐݒ�R�}���h�����s����
//	if (sprite.hasTexture == true) {
//		ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//		//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//		if (sprite.constBuffCount >= 3) {
//			DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(3, TextureManager::Ins()->GetSRV(sprite.textureIDNum));
//		}
//		else if (sprite.constBuffCount >= 2) {
//			DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(2, TextureManager::Ins()->GetSRV(sprite.textureIDNum));
//		}
//		else if (sprite.constBuffCount == 1) {
//			DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Ins()->GetSRV(sprite.textureIDNum));
//		}
//	}
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &sprite.vbView);
//
//	//3D�Œ��_�C���f�b�N�X������Ȃ璸�_�C���f�b�N�X�o�b�t�@�r���[�ݒ�R�}���h�����s����
//	if (sprite.index.size() > 0) {
//		DirectXBase::Ins()->cmdList->IASetIndexBuffer(&sprite.ibView);
//	}
//
//	//�`��R�}���h
//	if (sprite.index.size() > 0) {
//		DirectXBase::Ins()->cmdList->DrawIndexedInstanced(sprite.index.size(), 1, 0, 0, 0);
//	}
//	else {
//		DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//		DirectXBase::Ins()->cmdList->DrawInstanced(sprite.vertex.size(), 1, 0, 0);
//	}
//}
//
//void SpriteManager::ChangeScale(Sprite& sprite, XMFLOAT3 scale)
//{
//	sprite.scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
//	sprite.pos = XMFLOAT3(sprite.positionMat.r[3].m128_f32[0], sprite.positionMat.r[3].m128_f32[1], sprite.positionMat.r[3].m128_f32[2]);
//	sprite.isDirty = true;
//}
//
//void SpriteManager::ChangeRotation(Sprite& sprite, XMFLOAT3 rotation)
//{
//	XMMATRIX kari = XMMatrixIdentity();
//	kari *= XMMatrixRotationZ(rotation.z);
//	kari *= XMMatrixRotationX(rotation.x);
//	kari *= XMMatrixRotationY(rotation.y);
//	sprite.rotationMat = kari * sprite.rotationMat;
//	sprite.pos = XMFLOAT3(sprite.positionMat.r[3].m128_f32[0], sprite.positionMat.r[3].m128_f32[1], sprite.positionMat.r[3].m128_f32[2]);
//	sprite.isDirty = true;
//}
//
//void SpriteManager::ChangePosition(Sprite& sprite, XMFLOAT3 position)
//{
//	sprite.positionMat = XMMatrixTranslation(position.x, position.y, position.z);
//	sprite.pos = XMFLOAT3(sprite.positionMat.r[3].m128_f32[0], sprite.positionMat.r[3].m128_f32[1], sprite.positionMat.r[3].m128_f32[2]);
//	sprite.isDirty = true;
//}
//
//void SpriteManager::ChangePositionAdd(Sprite& sprite, XMFLOAT3 position)
//{
//	sprite.positionMat *= XMMatrixTranslation(position.x, position.y, position.z);
//	sprite.pos = XMFLOAT3(sprite.positionMat.r[3].m128_f32[0], sprite.positionMat.r[3].m128_f32[1], sprite.positionMat.r[3].m128_f32[2]);
//	sprite.isDirty = true;
//}
//
//GSParticleData SpriteManager::GenerateGSParticle(XMFLOAT3 pos, string piplineName, LPCWSTR fileName)
//{
//	GSParticleData data{};
//
//	//�p�C�v�����̖��O�̕ۑ�
//	data.piplineName = piplineName;
//
//	//�ݒ�\����
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//�V�F�[�_�[���猩����
//	descHeapDesc.NumDescriptors = 1;										//CBV1��
//	//�f�B�X�N���v�^�q�[�v�̐���
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&data.constDescHeap));
//
//	//�e�N�X�`�������[�h
//	data.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//
//	//���_�o�b�t�@�̐���
//	data.vertex = {};
//
//	//���_�o�b�t�@�r���[�̐���
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMFLOAT3)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&data.vertBuff)
//	);
//
//	//���_�o�b�t�@�r���[�̐ݒ�
//	data.vbView.BufferLocation = data.vertBuff->GetGPUVirtualAddress();
//	data.vbView.SizeInBytes = sizeof(XMFLOAT3);
//	data.vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	/*-----�萔�o�b�t�@�̐���-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&data.constBuff0)
//	);
//
//	//�s���������
//	data.rotationMat = XMMatrixIdentity();
//	data.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	data.positionMat = XMMatrixTranslation(pos.x, pos.y, pos.z);
//
//	//�}�b�v�������s��
//	XMFLOAT3* vertMap = nullptr;
//	data.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	vertMap = &data.vertex;		//���W���R�s�[
//	//�}�b�v������
//	data.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBV�f�B�X�N���v�^�q�[�v�̐��� �萔�o�b�t�@�̏���GPU�ɓ`���邽�߂̒萔�o�b�t�@�r���[�p-----*/
//	//CBV�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		data.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = data.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)data.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	result = data.constBuff0->Map(0, nullptr, (void**)&constMap);
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= data.scaleMat;
//	matWorld *= data.rotationMat;
//	matWorld *= data.positionMat;
//	constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//	constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//	constMap->eye = Camera::eye;
//	data.constBuff0->Unmap(0, nullptr);
//
//	return data;
//}
//
//void SpriteManager::DrawGSParticle(GSParticleData data)
//{
//	//�p�C�v���C���ƃ��[�g�V�O�l�`���̐ݒ�
//	PiplineManager::Ins()->SetPipline(data.piplineName);
//
//	//�萔�o�b�t�@�ւ̃f�[�^�]��
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuff0->Map(0, nullptr, (void**)&constMap);
//	//���[���h�s��̍X�V
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= data.scaleMat;
//	matWorld *= data.rotationMat;
//	matWorld *= data.positionMat;
//	constMap->mat.world = matWorld;												//���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
//	constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//	constMap->eye = Camera::eye;
//	data.constBuff0->Unmap(0, nullptr);
//
//	//�萔�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuff0->GetGPUVirtualAddress());
//
//	//�f�B�X�N���v�^�q�[�v�ݒ�R�}���h		�X�v���C�g���e�N�X�`���̃f�[�^�������Ă����ꍇ�̂ݐݒ�R�}���h�����s����
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//�V�F�[�_�[���\�[�X�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Ins()->GetSRV(data.textureIDNum));
//
//	//���_�o�b�t�@�r���[�ݒ�R�}���h
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//�`��R�}���h
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//�����̈�����ς��邱�ƂŒ��_�𗘗p���Ăǂ�Ȑ}�`��`������ݒ�ł��� ����3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(1, 1, 0, 0);
//}

#pragma endregion
