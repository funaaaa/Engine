#include "ObjectManager.h"
#include "Enum.h"

//Line ObjectManager::GenerateLine(const XMFLOAT3& pos1, const XMFLOAT3& pos2, const XMFLOAT4& color, int projectionID, int piplineID)
//{
//	//仮の変数
//	Line lineBuffer;
//
//	//パイプランの名前の保存
//	lineBuffer.basicdata.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&lineBuffer.constDescHeap));
//
//	//テクスチャをロード
//	lineBuffer.basicdata.textureID.push_back(TextureManager::Ins()->CreateTexture(color));
//
//	//頂点バッファの生成
//	Vertex vertex;
//	vertex.pos = pos1;	//左下
//	vertex.uv = XMFLOAT2(0, 0);
//	lineBuffer.vertex.push_back(vertex);
//	vertex.pos = pos2;		//左上
//	vertex.uv = XMFLOAT2(0, 0);
//	lineBuffer.vertex.push_back(vertex);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(lineBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&lineBuffer.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	lineBuffer.vbView.BufferLocation = lineBuffer.vertBuff->GetGPUVirtualAddress();
//	lineBuffer.vbView.SizeInBytes = lineBuffer.vertex.size() * sizeof(Vertex);
//	lineBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----定数バッファの生成-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&lineBuffer.constBuffB0)
//	);
//
//	//行列を初期化
//	lineBuffer.basicdata.projectionID = projectionID;
//	lineBuffer.rotationMat = XMMatrixIdentity();
//	lineBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	lineBuffer.positionMat = XMMatrixIdentity();
//	lineBuffer.pos1 = pos1;
//	lineBuffer.pos2 = pos2;
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	lineBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < lineBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = lineBuffer.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	lineBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		lineBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = lineBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)lineBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	result = lineBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	lineBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return lineBuffer;
//}

//Sprite ObjectManager::GenerateSprite(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color, LPCWSTR fileName)
//{
//	//仮の変数
//	Sprite spriteBuffer;
//
//	//パイプランの名前の保存
//	spriteBuffer.basicdata.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName != 0) {
//		spriteBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(fileName));
//	}
//	else {
//		spriteBuffer.basicdata.textureID.push_back(TextureManager::Ins()->CreateTexture(color));
//	}
//
//	//頂点バッファの生成
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//左下
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//左上
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//右下
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//右上
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----定数バッファの生成-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB0)
//	);
//
//	//行列を初期化
//	spriteBuffer.basicdata.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite spriteBuffer;
//
//	//パイプランの名前の保存
//	spriteBuffer.basicdata.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//テクスチャをロード
//	spriteBuffer.basicdata.textureID.push_back(textureID);
//
//	//頂点バッファの生成
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//左下
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//左上
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//右下
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//右上
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----定数バッファの生成-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB0)
//	);
//
//	//行列を初期化
//	spriteBuffer.basicdata.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	SpriteMultiTexture spriteBuffer;
//
//	//パイプランの名前の保存
//	spriteBuffer.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName != nullptr) {
//		spriteBuffer.textureID.resize(fileName->size());
//		for (int i = 0; i < fileName->size(); ++i) {
//			spriteBuffer.textureID.at(i) = TextureManager::Ins()->LoadTexture(fileName->at(i));
//		}
//	}
//
//	//頂点バッファの生成
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//左下
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//左上
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//右下
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//右上
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----定数バッファの生成-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.constBuffB0)
//	);
//
//	//行列を初期化
//	spriteBuffer.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	SpriteMultiPath spriteBuffer;
//
//	//パイプランの名前の保存
//	spriteBuffer.basicdata.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteBuffer.constDescHeap));
//
//	//テクスチャをロード
//	spriteBuffer.basicdata.textureID.push_back(textureID);
//
//	//頂点バッファの生成
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);		//左下
//	vertex.uv = XMFLOAT2(0, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//左上
//	vertex.uv = XMFLOAT2(0, 0);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);		//右下
//	vertex.uv = XMFLOAT2(1, 1);
//	spriteBuffer.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);		//右上
//	vertex.uv = XMFLOAT2(1, 0);
//	spriteBuffer.vertex.push_back(vertex);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(spriteBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&spriteBuffer.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	spriteBuffer.vbView.BufferLocation = spriteBuffer.vertBuff->GetGPUVirtualAddress();
//	spriteBuffer.vbView.SizeInBytes = spriteBuffer.vertex.size() * sizeof(Vertex);
//	spriteBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	spriteBuffer.basicdata.projectionID = projectionID;
//	spriteBuffer.rotationMat = XMMatrixIdentity();
//	spriteBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	spriteBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	spriteBuffer.pos = XMFLOAT3(spriteBuffer.positionMat.r[3].m128_f32[0], spriteBuffer.positionMat.r[3].m128_f32[1], spriteBuffer.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	spriteBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < spriteBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = spriteBuffer.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	spriteBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//	//2つ目を生成
//	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		spriteBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	cbvDesc.BufferLocation = spriteBuffer.constBuffB1->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)spriteBuffer.constBuffB1->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	result = spriteBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	spriteBuffer.constBuffB0->Unmap(0, nullptr);
//	//定数バッファ1へのデータ転送
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
//	//仮の変数
//	Object3D objectBuffer;
//
//	//パイプランの名前の保存
//	objectBuffer.basicdata.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 3;										//CBV3つ
//	//ディスクリプタヒープの生成
//	HRESULT resultBuff = DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&objectBuffer.constDescHeap));
//
//	//テクスチャをロード
//	objectBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(textureFileName));
//
//	//objファイルをロード
//	ModelDataManager::LoadObj(directoryPath, modelFileName, objectBuffer, isSmoothing);
//
//	//頂点バッファの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.vertBuff)
//	);
//	//頂点バッファビューの設定
//	objectBuffer.vbView.BufferLocation = objectBuffer.vertBuff->GetGPUVirtualAddress();
//	objectBuffer.vbView.SizeInBytes = objectBuffer.vertex.size() * sizeof(Vertex);
//	objectBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	objectBuffer.ibView.BufferLocation = objectBuffer.indexBuff->GetGPUVirtualAddress();
//	objectBuffer.ibView.Format = DXGI_FORMAT_R16_UINT;
//	objectBuffer.ibView.SizeInBytes = objectBuffer.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	objectBuffer.basicdata.projectionID = projectionID;
//	objectBuffer.rotationMat = XMMatrixIdentity();
//	objectBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	objectBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	objectBuffer.pos = XMFLOAT3(objectBuffer.positionMat.r[3].m128_f32[0], objectBuffer.positionMat.r[3].m128_f32[1], objectBuffer.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	objectBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//全頂点に対して
//	for (int i = 0; i < objectBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = objectBuffer.vertex.at(i);   // 座標をコピー
//	}
//	//マップを解除
//	objectBuffer.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
//	if (objectBuffer.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = objectBuffer.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < objectBuffer.index.size(); ++i) {
//			indexMap[i] = objectBuffer.index.at(i);
//		}
//		objectBuffer.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
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
//	//定数バッファへのデータ転送(行列と視点座標関係)
//	ConstBufferDataB0* constMap = nullptr;
//	result = objectBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	objectBuffer.constBuffB0->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(マテリアル関係)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = objectBuffer.constBuffB1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = objectBuffer.constBuffDataB1.ambient;
//	constMap1->diffuse = objectBuffer.constBuffDataB1.diffuse;
//	constMap1->specular = objectBuffer.constBuffDataB1.specular;
//	constMap1->alpha = objectBuffer.constBuffDataB1.alpha;
//	objectBuffer.constBuffB1->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(ライト関係)
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
//	//仮の変数
//	Object3DShadow objectBuffer;
//
//	//パイプランの名前の保存
//	objectBuffer.basicdata.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 3;										//CBV3つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&objectBuffer.constDescHeap));
//
//	//テクスチャをロード
//	objectBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(textureFileName));
//
//	//objファイルをロード
//	ModelDataManager::LoadObj(directoryPath, modelFileName, objectBuffer, isSmoothing);
//
//	//頂点バッファの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.vertBuff)
//	);
//	//頂点バッファビューの設定
//	objectBuffer.vbView.BufferLocation = objectBuffer.vertBuff->GetGPUVirtualAddress();
//	objectBuffer.vbView.SizeInBytes = objectBuffer.vertex.size() * sizeof(Vertex);
//	objectBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	objectBuffer.ibView.BufferLocation = objectBuffer.indexBuff->GetGPUVirtualAddress();
//	objectBuffer.ibView.Format = DXGI_FORMAT_R16_UINT;
//	objectBuffer.ibView.SizeInBytes = objectBuffer.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	objectBuffer.basicdata.projectionID = projectionID;
//	objectBuffer.rotationMat = XMMatrixIdentity();
//	objectBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	objectBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	objectBuffer.pos = XMFLOAT3(objectBuffer.positionMat.r[3].m128_f32[0], objectBuffer.positionMat.r[3].m128_f32[1], objectBuffer.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	objectBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//全頂点に対して
//	for (int i = 0; i < objectBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = objectBuffer.vertex.at(i);   // 座標をコピー
//	}
//	//マップを解除
//	objectBuffer.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
//	if (objectBuffer.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = objectBuffer.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < objectBuffer.index.size(); ++i) {
//			indexMap[i] = objectBuffer.index.at(i);
//		}
//		objectBuffer.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
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
//	//定数バッファへのデータ転送(行列と視点座標関係)
//	ConstBufferDataB0* constMap = nullptr;
//	result = objectBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	objectBuffer.constBuffB0->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(マテリアル関係)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = objectBuffer.constBuffB1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = objectBuffer.constBuffDataB1.ambient;
//	constMap1->diffuse = objectBuffer.constBuffDataB1.diffuse;
//	constMap1->specular = objectBuffer.constBuffDataB1.specular;
//	constMap1->alpha = objectBuffer.constBuffDataB1.alpha;
//	objectBuffer.constBuffB1->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(ライト関係)
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
//	//仮の変数
//	Object3D objectBuffer;
//
//	//パイプランの名前の保存
//	objectBuffer.basicdata.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 3;										//CBV3つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&objectBuffer.constDescHeap));
//
//	//テクスチャをロード
//	objectBuffer.basicdata.textureID.push_back(TextureManager::Ins()->LoadTexture(textureFileName));
//
//	//objファイルをロード
//	ModelDataManager::LoadObj(directoryPath, modelFileName, objectBuffer, isSmoothing);
//
//	//頂点バッファの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.vertBuff)
//	);
//	//頂点バッファビューの設定
//	objectBuffer.vbView.BufferLocation = objectBuffer.vertBuff->GetGPUVirtualAddress();
//	objectBuffer.vbView.SizeInBytes = objectBuffer.vertex.size() * sizeof(Vertex);
//	objectBuffer.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(objectBuffer.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&objectBuffer.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	objectBuffer.ibView.BufferLocation = objectBuffer.indexBuff->GetGPUVirtualAddress();
//	objectBuffer.ibView.Format = DXGI_FORMAT_R16_UINT;
//	objectBuffer.ibView.SizeInBytes = objectBuffer.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	objectBuffer.basicdata.projectionID = projectionID;
//	objectBuffer.rotationMat = XMMatrixIdentity();
//	objectBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	objectBuffer.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	objectBuffer.pos = XMFLOAT3(objectBuffer.positionMat.r[3].m128_f32[0], objectBuffer.positionMat.r[3].m128_f32[1], objectBuffer.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	objectBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//全頂点に対して
//	for (int i = 0; i < objectBuffer.vertex.size(); ++i)
//	{
//		vertMap[i] = objectBuffer.vertex.at(i);   // 座標をコピー
//	}
//	//マップを解除
//	objectBuffer.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
//	if (objectBuffer.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = objectBuffer.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < objectBuffer.index.size(); ++i) {
//			indexMap[i] = objectBuffer.index.at(i);
//		}
//		objectBuffer.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
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
//	//定数バッファへのデータ転送(行列と視点座標関係)
//	ConstBufferDataB0* constMap = nullptr;
//	result = objectBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	objectBuffer.constBuffB0->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(マテリアル関係)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = objectBuffer.constBuffB1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = objectBuffer.constBuffDataB1.ambient;
//	constMap1->diffuse = objectBuffer.constBuffDataB1.diffuse;
//	constMap1->specular = objectBuffer.constBuffDataB1.specular;
//	constMap1->alpha = objectBuffer.constBuffDataB1.alpha;
//	objectBuffer.constBuffB1->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(ライト関係)
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
//	//パイプランの名前の保存
//	particleBuffer.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 1;										//CBV1つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&particleBuffer.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName == 0) {
//		particleBuffer.textureID = TextureManager::Ins()->CreateTexture(color);
//	}
//	else {
//		particleBuffer.textureID = TextureManager::Ins()->LoadTexture(fileName);
//	}
//
//	//頂点バッファの生成
//	particleBuffer.vertex = {};
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMFLOAT3)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	particleBuffer.vbView.BufferLocation = particleBuffer.vertBuff->GetGPUVirtualAddress();
//	particleBuffer.vbView.SizeInBytes = sizeof(XMFLOAT3);
//	particleBuffer.vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	/*-----定数バッファの生成-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.constBuffB0)
//	);
//
//	//行列を初期化
//	particleBuffer.rotationMat = XMMatrixIdentity();
//	particleBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	particleBuffer.positionMat = XMMatrixTranslation(pos.x, pos.y, pos.z);
//
//	//マップ処理を行う
//	XMFLOAT3* vertMap = nullptr;
//	particleBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	vertMap = &particleBuffer.vertex;		//座標をコピー
//	//マップを解除
//	particleBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		particleBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = particleBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)particleBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	result = particleBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= particleBuffer.scaleMat;
//	matWorld *= particleBuffer.rotationMat;
//	matWorld *= particleBuffer.positionMat;
//	constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
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
//	//パイプランの名前の保存
//	particleBuffer.piplineID = piplineID;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 1;										//CBV1つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&particleBuffer.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName == 0) {
//		particleBuffer.textureID = TextureManager::Ins()->CreateTexture(color);
//	}
//	else {
//		particleBuffer.textureID = TextureManager::Ins()->LoadTexture(fileName);
//	}
//
//	//頂点バッファの生成
//	particleBuffer.vertex = {};
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMFLOAT3)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	particleBuffer.vbView.BufferLocation = particleBuffer.vertBuff->GetGPUVirtualAddress();
//	particleBuffer.vbView.SizeInBytes = sizeof(XMFLOAT3);
//	particleBuffer.vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	/*-----定数バッファの生成-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(((sizeof(GSConstBufferDataB0) * INSTANCE_COUNT) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&particleBuffer.constBuffB0)
//	);
//
//	//行列を初期化
//	particleBuffer.rotationMat = XMMatrixIdentity();
//	particleBuffer.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	particleBuffer.positionMat = XMMatrixTranslation(pos.x, pos.y, pos.z);
//
//	//マップ処理を行う
//	XMFLOAT3* vertMap = nullptr;
//	particleBuffer.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	vertMap = &particleBuffer.vertex;		//座標をコピー
//	//マップを解除
//	particleBuffer.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		particleBuffer.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = particleBuffer.constBuffB0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)particleBuffer.constBuffB0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
//	GSConstBufferDataB0* constMap = nullptr;
//	result = particleBuffer.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= particleBuffer.scaleMat;
//	matWorld *= particleBuffer.rotationMat;
//	matWorld *= particleBuffer.positionMat;
//	constMap->world = matWorld * (Camera::matView * Camera::matPerspective);												//ワールド変換 * ビュー変換 * 透視投影変換
//	constMap->color = XMFLOAT4(1, 1, 1, 1);
//	particleBuffer.constBuffB0->Unmap(0, nullptr);
//
//	return particleBuffer;
//}

//void ObjectManager::DrawLine(Line& data)
//{
//	//非表示状態だったら描画処理を行わない
//	if (data.basicdata.isDisplay == false) return;
//
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//投影IDがbackGourndの場合は平行投影変換を行う
//	if (data.basicdata.projectionID == PROJECTIONID_UI) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//平行投影変換
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//投影IDがobjectの場合はいろいろな変換を行う
//	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//ビルボードの場合
//	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
//		//視点座標
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//注視点座標
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(仮の)上方向
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//カメラZ軸
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0ベクトルだと向きが定まらないので除外
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//ベクトルを正規化
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//カメラのX軸方向(右方向)
//		XMVECTOR cameraAxisX{};
//		//X軸は上方向->Z軸の外積で求まる
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//ベクトルを正規化
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//カメラのY軸(上方向)
//		XMVECTOR cameraAxisY{};
//		//Y軸はZ軸->X軸の外積で求める
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//ベクトルを正規化
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//カメラ回転行列
//		XMMATRIX matCameraRot;
//		//カメラ座標系->ワールド座標系の返還行列
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//転地により逆行列(逆回転)を計算
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//視点座標に-1をかけた座標
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
//		//一つのベクトルにまとめる
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//ビュー行列に平行移動分を設定
//		matView.r[3] = translation;
//
//		//ビルボード行列
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//ディスクリプタヒープ設定コマンド
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//シェーダーリソースビュー設定コマンド
//	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
//	}
//
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//描画コマンド
//	//DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}

//void ObjectManager::DrawSprite(Sprite& data)
//{
//	//非表示状態だったら描画処理を行わない
//	if (data.basicdata.isDisplay == false) return;
//
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//投影IDがbackGourndの場合は平行投影変換を行う
//	if (data.basicdata.projectionID == PROJECTIONID_UI) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//平行投影変換
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//投影IDがobjectの場合はいろいろな変換を行う
//	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//ビルボードの場合
//	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
//		//視点座標
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//注視点座標
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(仮の)上方向
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//カメラZ軸
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0ベクトルだと向きが定まらないので除外
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//ベクトルを正規化
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//カメラのX軸方向(右方向)
//		XMVECTOR cameraAxisX{};
//		//X軸は上方向->Z軸の外積で求まる
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//ベクトルを正規化
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//カメラのY軸(上方向)
//		XMVECTOR cameraAxisY{};
//		//Y軸はZ軸->X軸の外積で求める
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//ベクトルを正規化
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//カメラ回転行列
//		XMMATRIX matCameraRot;
//		//カメラ座標系->ワールド座標系の返還行列
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//転地により逆行列(逆回転)を計算
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//視点座標に-1をかけた座標
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
//		//一つのベクトルにまとめる
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//ビュー行列に平行移動分を設定
//		matView.r[3] = translation;
//
//		//ビルボード行列
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//座標を保存しておく
//	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//ディスクリプタヒープ設定コマンド
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//シェーダーリソースビュー設定コマンド
//	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
//	}
//
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//描画コマンド
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}

//void ObjectManager::DrawSpriteMultiPath(SpriteMultiPath& data)
//{
//	//非表示状態だったら描画処理を行わない
//	if (data.basicdata.isDisplay == false) return;
//
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//投影IDがbackGourndの場合は平行投影変換を行う
//	if (data.basicdata.projectionID == PROJECTIONID_UI) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//平行投影変換
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//投影IDがobjectの場合はいろいろな変換を行う
//	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//ビルボードの場合
//	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
//		//視点座標
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//注視点座標
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(仮の)上方向
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//カメラZ軸
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0ベクトルだと向きが定まらないので除外
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//ベクトルを正規化
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//カメラのX軸方向(右方向)
//		XMVECTOR cameraAxisX{};
//		//X軸は上方向->Z軸の外積で求まる
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//ベクトルを正規化
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//カメラのY軸(上方向)
//		XMVECTOR cameraAxisY{};
//		//Y軸はZ軸->X軸の外積で求める
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//ベクトルを正規化
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//カメラ回転行列
//		XMMATRIX matCameraRot;
//		//カメラ座標系->ワールド座標系の返還行列
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//転地により逆行列(逆回転)を計算
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//視点座標に-1をかけた座標
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
//		//一つのベクトルにまとめる
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//ビュー行列に平行移動分を設定
//		matView.r[3] = translation;
//
//		//ビルボード行列
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//定数バッファ2のデータ転送
//	ConstBufferDataMultiPath* constMap2 = nullptr;
//	result = data.constBuffB1->Map(0, nullptr, (void**)&constMap2);
//	constMap2->gaussianWeight[0] = data.constBufferDataB1.gaussianWeight[0];
//	constMap2->gaussianWeight[1] = data.constBufferDataB1.gaussianWeight[1];
//	data.constBuffB1->Unmap(0, nullptr);
//
//	//座標を保存しておく
//	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, data.constBuffB1->GetGPUVirtualAddress());
//
//	//ディスクリプタヒープ設定コマンド
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//シェーダーリソースビュー設定コマンド
//	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 2, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
//	}
//
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//描画コマンド
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}
//
//void ObjectManager::DrawSpriteMultiTexture(SpriteMultiTexture& data)
//{
//	//非表示状態だったら描画処理を行わない
//	if (data.isDisplay == false) return;
//
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(data.piplineID);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//投影IDがbackGourndの場合は平行投影変換を行う
//	if (data.projectionID == PROJECTIONID_UI) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//平行投影変換
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//投影IDがobjectの場合はいろいろな変換を行う
//	else if (data.projectionID == PROJECTIONID_OBJECT) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	//ビルボードの場合
//	else if (data.projectionID == PROJECTIONID_BILLBOARD) {
//		//視点座標
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//注視点座標
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(仮の)上方向
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//カメラZ軸
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0ベクトルだと向きが定まらないので除外
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//ベクトルを正規化
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//カメラのX軸方向(右方向)
//		XMVECTOR cameraAxisX{};
//		//X軸は上方向->Z軸の外積で求まる
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//ベクトルを正規化
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//カメラのY軸(上方向)
//		XMVECTOR cameraAxisY{};
//		//Y軸はZ軸->X軸の外積で求める
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//ベクトルを正規化
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//カメラ回転行列
//		XMMATRIX matCameraRot;
//		//カメラ座標系->ワールド座標系の返還行列
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//転地により逆行列(逆回転)を計算
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//視点座標に-1をかけた座標
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
//		//一つのベクトルにまとめる
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//ビュー行列に平行移動分を設定
//		matView.r[3] = translation;
//
//		//ビルボード行列
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= data.scaleMat;
//		matWorld *= data.rotationMat;
//		matWorld *= data.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//		constMap->color = data.constBufferDataB0.color;
//	}
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//座標を保存しておく
//	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//ディスクリプタヒープ設定コマンド
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//シェーダーリソースビュー設定コマンド
//	for (int i = 0; i < data.textureID.size(); ++i) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(data.textureID.at(i)));
//	}
//
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//描画コマンド
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(data.vertex.size(), 1, 0, 0);
//}
//
////void ObjectManager::DrawObject(Object3D& data)
////{
////	//非表示状態だったら描画処理を行わない
////	if (data.basicdata.isDisplay == false) return;
////
////	//パイプラインとルートシグネチャの設定
////	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
////
////	//定数バッファへのデータ転送
////	ConstBufferDataB0* constMap = nullptr;
////	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
////	//投影IDがbackGourndの場合は平行投影変換を行う
////	if (data.basicdata.projectionID == PROJECTIONID_UI) {
////		//ワールド行列の更新
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;
////		constMap->mat.viewproj = Camera::matProjection;								//平行投影変換
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//投影IDがobjectの場合はいろいろな変換を行う
////	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
////		//ワールド行列の更新
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
////		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//ビルボードの場合
////	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
////		//視点座標
////		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
////		//注視点座標
////		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
////		//(仮の)上方向
////		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
////		//カメラZ軸
////		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
////		//0ベクトルだと向きが定まらないので除外
////		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
////		assert(!XMVector3IsInfinite(cameraAxisZ));
////		assert(!XMVector3Equal(upVector, XMVectorZero()));
////		assert(!XMVector3IsInfinite(upVector));
////		//ベクトルを正規化
////		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
////		//カメラのX軸方向(右方向)
////		XMVECTOR cameraAxisX{};
////		//X軸は上方向->Z軸の外積で求まる
////		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
////		//ベクトルを正規化
////		cameraAxisX = XMVector3Normalize(cameraAxisX);
////		//カメラのY軸(上方向)
////		XMVECTOR cameraAxisY{};
////		//Y軸はZ軸->X軸の外積で求める
////		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
////		//ベクトルを正規化
////		cameraAxisY = XMVector3Normalize(cameraAxisY);
////		//カメラ回転行列
////		XMMATRIX matCameraRot;
////		//カメラ座標系->ワールド座標系の返還行列
////		matCameraRot.r[0] = cameraAxisX;
////		matCameraRot.r[1] = cameraAxisY;
////		matCameraRot.r[2] = cameraAxisZ;
////		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
////		//転地により逆行列(逆回転)を計算
////		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
////		//視点座標に-1をかけた座標
////		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
////		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
////		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
////		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
////		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
////		//一つのベクトルにまとめる
////		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
////		//ビュー行列に平行移動分を設定
////		matView.r[3] = translation;
////
////		//ビルボード行列
////		XMMATRIX billboardMat = XMMatrixIdentity();
////		billboardMat.r[0] = cameraAxisX;
////		billboardMat.r[1] = cameraAxisY;
////		billboardMat.r[2] = cameraAxisZ;
////		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
////		//ワールド行列の更新
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= billboardMat;
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
////		constMap->mat.viewproj = matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	data.constBuffB0->Unmap(0, nullptr);
////
////	//定数バッファの数が２つ以上(マテリアルを持っている)なら２つ目の定数バッファのマップ処理も行う
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
////	//座標を保存しておく
////	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
////
////	//定数バッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, data.constBuffB1->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, data.constBuffB2->GetGPUVirtualAddress());
////
////	//ディスクリプタヒープ設定コマンド
////	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
////	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
////	//シェーダーリソースビュー設定コマンド
////	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
////		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 3, TextureManager::Ins()->GetSRV(data.basicdata.textureID[i]));
////	}
////
////	//頂点バッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
////
////	//頂点インデックスバッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->IASetIndexBuffer(&data.ibView);
////
////	//描画コマンド
////	DirectXBase::Ins()->cmdList->DrawIndexedInstanced(data.index.size(), 1, 0, 0, 0);
////}
////
////void ObjectManager::DrawObject(Object3DShadow& data)
////{
////	//非表示状態だったら描画処理を行わない
////	if (data.basicdata.isDisplay == false) return;
////
////	//パイプラインとルートシグネチャの設定
////	PiplineManager::Ins()->SetPipline(data.basicdata.piplineID);
////
////	//定数バッファへのデータ転送
////	ConstBufferDataB0* constMap = nullptr;
////	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
////	//投影IDがbackGourndの場合は平行投影変換を行う
////	if (data.basicdata.projectionID == PROJECTIONID_UI) {
////		//ワールド行列の更新
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;
////		constMap->mat.viewproj = Camera::matProjection;								//平行投影変換
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//投影IDがobjectの場合はいろいろな変換を行う
////	else if (data.basicdata.projectionID == PROJECTIONID_OBJECT) {
////		//ワールド行列の更新
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
////		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	//ビルボードの場合
////	else if (data.basicdata.projectionID == PROJECTIONID_BILLBOARD) {
////		//視点座標
////		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
////		//注視点座標
////		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
////		//(仮の)上方向
////		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
////		//カメラZ軸
////		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
////		//0ベクトルだと向きが定まらないので除外
////		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
////		assert(!XMVector3IsInfinite(cameraAxisZ));
////		assert(!XMVector3Equal(upVector, XMVectorZero()));
////		assert(!XMVector3IsInfinite(upVector));
////		//ベクトルを正規化
////		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
////		//カメラのX軸方向(右方向)
////		XMVECTOR cameraAxisX{};
////		//X軸は上方向->Z軸の外積で求まる
////		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
////		//ベクトルを正規化
////		cameraAxisX = XMVector3Normalize(cameraAxisX);
////		//カメラのY軸(上方向)
////		XMVECTOR cameraAxisY{};
////		//Y軸はZ軸->X軸の外積で求める
////		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
////		//ベクトルを正規化
////		cameraAxisY = XMVector3Normalize(cameraAxisY);
////		//カメラ回転行列
////		XMMATRIX matCameraRot;
////		//カメラ座標系->ワールド座標系の返還行列
////		matCameraRot.r[0] = cameraAxisX;
////		matCameraRot.r[1] = cameraAxisY;
////		matCameraRot.r[2] = cameraAxisZ;
////		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
////		//転地により逆行列(逆回転)を計算
////		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
////		//視点座標に-1をかけた座標
////		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
////		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
////		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
////		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
////		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
////		//一つのベクトルにまとめる
////		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
////		//ビュー行列に平行移動分を設定
////		matView.r[3] = translation;
////
////		//ビルボード行列
////		XMMATRIX billboardMat = XMMatrixIdentity();
////		billboardMat.r[0] = cameraAxisX;
////		billboardMat.r[1] = cameraAxisY;
////		billboardMat.r[2] = cameraAxisZ;
////		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
////		//ワールド行列の更新
////		XMMATRIX matWorld = XMMatrixIdentity();
////		matWorld *= billboardMat;
////		matWorld *= data.scaleMat;
////		matWorld *= data.rotationMat;
////		matWorld *= data.positionMat;
////		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
////		constMap->mat.viewproj = matView * Camera::matPerspective;
////		constMap->eye = Camera::eye;
////		constMap->color = data.constBuffDataB0.color;
////	}
////	data.constBuffB0->Unmap(0, nullptr);
////
////	//定数バッファの数が２つ以上(マテリアルを持っている)なら２つ目の定数バッファのマップ処理も行う
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
////	//座標を保存しておく
////	data.pos = XMFLOAT3(data.positionMat.r[3].m128_f32[0], data.positionMat.r[3].m128_f32[1], data.positionMat.r[3].m128_f32[2]);
////
////	//定数バッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, data.constBuffB1->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, data.constBuffB2->GetGPUVirtualAddress());
////
////	//ディスクリプタヒープ設定コマンド
////	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
////	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
////	//シェーダーリソースビュー設定コマンド
////	for (int i = 0; i < data.basicdata.textureID.size(); ++i) {
////		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 3, TextureManager::Ins()->GetSRV(data.basicdata.textureID[0]));
////	}
////
////	//頂点バッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
////
////	//頂点インデックスバッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->IASetIndexBuffer(&data.ibView);
////
////	//描画コマンド
////	DirectXBase::Ins()->cmdList->DrawIndexedInstanced(data.index.size(), 1, 0, 0, 0);
////}
////
////void ObjectManager::DrawObjectInstance(const Object3D& instanceData, const vector<Object3D>& data, const int& piplineID)
////{
////	//インスタンス数を取得
////	int instanceCount = data.size();
////
////	//パイプラインとルートシグネチャの設定
////	PiplineManager::Ins()->SetPipline(piplineID);
////
////	//定数バッファへのデータ転送
////	ConstBufferDataB0* constMap = nullptr;
////	HRESULT result = instanceData.constBuffB0->Map(0, nullptr, (void**)&constMap);
////
////	for (int i = 0; i < instanceCount; ++i) {
////		//投影IDがbackGourndの場合は平行投影変換を行う
////		if (data[i].basicdata.projectionID == PROJECTIONID_UI) {
////			//ワールド行列の更新
////			XMMATRIX matWorld = XMMatrixIdentity();
////			matWorld *= data[i].scaleMat;
////			matWorld *= data[i].rotationMat;
////			matWorld *= data[i].positionMat;
////			constMap[i].mat.world = matWorld;
////			constMap[i].mat.viewproj = Camera::matProjection;								//平行投影変換
////			constMap[i].eye = Camera::eye;
////			constMap[i].color = data[i].constBuffDataB0.color;
////		}
////		//投影IDがobjectの場合はいろいろな変換を行う
////		else if (data[i].basicdata.projectionID == PROJECTIONID_OBJECT) {
////			//ワールド行列の更新
////			XMMATRIX matWorld = XMMatrixIdentity();
////			matWorld *= data[i].scaleMat;
////			matWorld *= data[i].rotationMat;
////			matWorld *= data[i].positionMat;
////			constMap[i].mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
////			constMap[i].mat.viewproj = Camera::matView * Camera::matPerspective;
////			constMap[i].eye = Camera::eye;
////			constMap[i].color = data[i].constBuffDataB0.color;
////		}
////		//ビルボードの場合
////		else if (data[i].basicdata.projectionID == PROJECTIONID_BILLBOARD) {
////			//視点座標
////			XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
////			//注視点座標
////			XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
////			//(仮の)上方向
////			XMVECTOR upVector = XMLoadFloat3(&Camera::up);
////			//カメラZ軸
////			XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
////			//0ベクトルだと向きが定まらないので除外
////			assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
////			assert(!XMVector3IsInfinite(cameraAxisZ));
////			assert(!XMVector3Equal(upVector, XMVectorZero()));
////			assert(!XMVector3IsInfinite(upVector));
////			//ベクトルを正規化
////			cameraAxisZ = XMVector3Normalize(cameraAxisZ);
////			//カメラのX軸方向(右方向)
////			XMVECTOR cameraAxisX{};
////			//X軸は上方向->Z軸の外積で求まる
////			cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
////			//ベクトルを正規化
////			cameraAxisX = XMVector3Normalize(cameraAxisX);
////			//カメラのY軸(上方向)
////			XMVECTOR cameraAxisY{};
////			//Y軸はZ軸->X軸の外積で求める
////			cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
////			//ベクトルを正規化
////			cameraAxisY = XMVector3Normalize(cameraAxisY);
////			//カメラ回転行列
////			XMMATRIX matCameraRot;
////			//カメラ座標系->ワールド座標系の返還行列
////			matCameraRot.r[0] = cameraAxisX;
////			matCameraRot.r[1] = cameraAxisY;
////			matCameraRot.r[2] = cameraAxisZ;
////			matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
////			//転地により逆行列(逆回転)を計算
////			XMMATRIX matView = XMMatrixTranspose(matCameraRot);
////			//視点座標に-1をかけた座標
////			XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
////			//カメラの位置からワールド原点へのベクトル(カメラ座標系)
////			XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
////			XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
////			XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
////			//一つのベクトルにまとめる
////			XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
////			//ビュー行列に平行移動分を設定
////			matView.r[3] = translation;
////
////			//ビルボード行列
////			XMMATRIX billboardMat = XMMatrixIdentity();
////			billboardMat.r[0] = cameraAxisX;
////			billboardMat.r[1] = cameraAxisY;
////			billboardMat.r[2] = cameraAxisZ;
////			billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
////			//ワールド行列の更新
////			XMMATRIX matWorld = XMMatrixIdentity();
////			matWorld *= billboardMat;
////			matWorld *= data[i].scaleMat;
////			matWorld *= data[i].rotationMat;
////			matWorld *= data[i].positionMat;
////			constMap[i].mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
////			constMap[i].mat.viewproj = matView * Camera::matPerspective;
////			constMap[i].eye = Camera::eye;
////			constMap[i].color = data[i].constBuffDataB0.color;
////		}
////	}
////
////	instanceData.constBuffB0->Unmap(0, nullptr);
////
////	//定数バッファの数が２つ以上(マテリアルを持っている)なら２つ目の定数バッファのマップ処理も行う
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
////	//定数バッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, instanceData.constBuffB0->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, instanceData.constBuffB1->GetGPUVirtualAddress());
////	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, instanceData.constBuffB2->GetGPUVirtualAddress());
////
////	//ディスクリプタヒープ設定コマンド
////	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
////	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
////	//シェーダーリソースビュー設定コマンド
////	for (int i = 0; i < instanceData.basicdata.textureID.size(); ++i) {
////		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 3, TextureManager::Ins()->GetSRV(instanceData.basicdata.textureID[0]));
////	}
////
////	//頂点バッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &instanceData.vbView);
////
////	//頂点インデックスバッファビュー設定コマンド
////	DirectXBase::Ins()->cmdList->IASetIndexBuffer(&instanceData.ibView);
////
////	//描画コマンド
////	DirectXBase::Ins()->cmdList->DrawIndexedInstanced(instanceData.index.size(), instanceCount, 0, 0, 0);
////}
//
//void ObjectManager::DrawGSParticle(GSParticleData& data)
//{
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(data.piplineID);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuffB0->Map(0, nullptr, (void**)&constMap);
//	//ワールド行列の更新
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= data.scaleMat;
//	matWorld *= data.rotationMat;
//	matWorld *= data.positionMat;
//	constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//	constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//	constMap->eye = Camera::eye;
//	constMap->color = data.constBuffDataB0.color;
//	data.constBuffB0->Unmap(0, nullptr);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuffB0->GetGPUVirtualAddress());
//
//	//ディスクリプタヒープ設定コマンド		スプライトがテクスチャのデータを持っていた場合のみ設定コマンドを実行する
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//シェーダーリソースビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Ins()->GetSRV(data.textureID));
//
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//描画コマンド
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(1, 1, 0, 0);
//}
//
//void ObjectManager::DrawGSParticleInstance(const GSParticleData& instanceData, vector<ParticleData*>& data, const int& piplinID) {
//
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(piplinID);
//
//	int instanceCount = data.size();
//
//	//定数バッファへのデータ転送
//	GSConstBufferDataB0* constMap = nullptr;
//	HRESULT result = instanceData.constBuffB0->Map(0, nullptr, (void**)&constMap);
//
//	for (int i = 0; i < instanceCount; ++i) {
//
//		if (data[i] == nullptr) continue;
//
//		if (data[i]->dirtyFlag) continue;
//
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= data[i]->matScale;
//		matWorld *= data[i]->matRot;
//		matWorld *= data[i]->matTrans;
//		data[i]->dirtyFlag = true;
//		constMap[i].world = matWorld * (Camera::matView * Camera::matPerspective);												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap[i].color = instanceData.constBuffDataB0.color;
//
//	}
//
//	instanceData.constBuffB0->Unmap(0, nullptr);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, instanceData.constBuffB0->GetGPUVirtualAddress());
//
//	//ディスクリプタヒープ設定コマンド		スプライトがテクスチャのデータを持っていた場合のみ設定コマンドを実行する
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//シェーダーリソースビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Ins()->GetSRV(instanceData.textureID));
//
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &instanceData.vbView);
//
//	//描画コマンド
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
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
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	line.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < line.vertex.size(); ++i)
//	{
//		vertMap[i] = line.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	line.vertBuff->Unmap(0, nullptr);
//
//	line.pos1 = pos1;
//	line.pos2 = pos2;
//
//}


#pragma region 元コード
//SpriteManager::SpriteManager() {
//}
//
//Sprite SpriteManager::GenerateLine(XMFLOAT3 startPos, XMFLOAT3 endPos, int projectionID, string pipleName, XMFLOAT4 color)
//{
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	proSprite.hasTexture = true;
//
//	//頂点バッファの生成
//	Vertex vertex;
//	vertex.pos = startPos;	//視点
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = endPos;	//終点
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//頂点インデックスの設定
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(1);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixIdentity();
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//spriteをsprite配列の最後に生成
//	pair<string, Sprite> pre;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	proSprite.hasTexture = true;
//
//	//頂点バッファの生成
//	for (int i = 0; i < lineCount; ++i) {
//		Vertex vertex;
//		vertex.pos = XMFLOAT3(startPos.x + (lineInterval * i * directions.x), startPos.y + (lineInterval * i * directions.y), startPos.z + (lineInterval * i * directions.z));	//視点
//		vertex.normal = XMFLOAT3(0, 0, 0);
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(endPos.x + (lineInterval * i * directions.x), endPos.y + (lineInterval * i * directions.y), endPos.z + (lineInterval * i * directions.z));	//終点
//		vertex.uv = XMFLOAT2(0, 1);
//		proSprite.vertex.push_back(vertex);
//	}
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//頂点インデックスの設定
//	for (int i = 0; i < lineCount * 2; ++i) {
//		proSprite.index.push_back(i);
//	}
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixIdentity();
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//spriteをsprite配列の最後に生成
//	pair<string, Sprite> pre;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	proSprite.hasTexture = true;
//
//	Vertex vertex;
//	vertex.normal = XMFLOAT3(0, 0, 0);
//
//	//頂点バッファの生成
//	for (int i = 0; i < lineCount + 1; ++i) {
//		/*下面の頂点*/
//		//縦
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//横
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*上面の頂点*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//縦
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//横
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*右面の頂点*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//縦
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//横(奥)
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*左面の頂点*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//縦
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * -1));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//横(奥)
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*奥面の頂点*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//縦
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), -size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//横
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), -size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	/*手前面の頂点*/
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//縦
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), -size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * -1), size.y + (lineInterval * i * 0), size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//	for (int i = 0; i < lineCount + 1; ++i) {
//		//横
//		vertex.pos = XMFLOAT3(-size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//始点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//		vertex.pos = XMFLOAT3(size.x + (lineInterval * i * 0), size.y + (lineInterval * i * -1), size.z + (lineInterval * i * 0));	//終点
//		vertex.uv = XMFLOAT2(0, 0);
//		proSprite.vertex.push_back(vertex);
//	}
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//頂点インデックスの設定
//	for (int i = 0; i < proSprite.vertex.size(); ++i) {
//		proSprite.index.push_back(i);
//	}
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixIdentity();
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//spriteをsprite配列の最後に生成
//	pair<string, Sprite> pre;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	return proSprite;
//}
//
//Sprite SpriteManager::Generate2DTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, string pipleName, LPCWSTR fileName, UVs* uv) {
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = false;
//	}
//
//	//頂点バッファの生成
//	Vertex vertex;
//	vertex.pos = XMFLOAT3(-size.x, -size.y, 10);	//左下
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(-size.x, size.y, 10);	//左上
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, -size.y, 10);	//右下
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);
//	vertex.pos = XMFLOAT3(size.x, size.y, 10);	//右上
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);
//
//	//引数のuvがnullptrじゃなかったら代入
//	if (uv != nullptr) {
//		UVs uvBuff = *uv;
//		proSprite.vertex.at(0).uv = uvBuff.leftUp;
//		proSprite.vertex.at(1).uv = uvBuff.leftDown;
//		proSprite.vertex.at(2).uv = uvBuff.rightUp;
//		proSprite.vertex.at(3).uv = uvBuff.rightDown;
//	}
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//法線ベクトルの計算
//	if (projectionID != backGround) {
//		for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//			//三角形ひとつひとつ計算していく
//			//三角形のインデックスを取り出して、一時的な変数に入れる
//			unsigned short index0 = proSprite.index.at(i * 3 + 0);
//			unsigned short index1 = proSprite.index.at(i * 3 + 1);
//			unsigned short index2 = proSprite.index.at(i * 3 + 2);
//			//三角形を構成する頂点座標をベクトルに代入
//			XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//			XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//			XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//			//p0->p1ベクトル,p0->p2ベクトルを計算
//			XMVECTOR v1 = XMVectorSubtract(p1, p0);
//			XMVECTOR v2 = XMVectorSubtract(p2, p0);
//			//外戚は両方から垂直なベクトル
//			XMVECTOR normal = XMVector3Cross(v1, v2);
//			//正規化
//			normal = XMVector3Normalize(normal);
//			//求めた法線を頂点データに代入
//			XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//			XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//			XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//		}
//	}
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//頂点バッファの生成
//#pragma region 頂点初期化
//	Vertex vertex;
//	//前
//	vertex.pos = XMFLOAT3(-size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//左下
//	vertex.pos = XMFLOAT3(-size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//左上
//	vertex.pos = XMFLOAT3(size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//右下
//	vertex.pos = XMFLOAT3(size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//右上
//	//後ろ
//	vertex.pos = XMFLOAT3(size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//左下
//	vertex.pos = XMFLOAT3(size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//左上
//	vertex.pos = XMFLOAT3(-size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//右下
//	vertex.pos = XMFLOAT3(-size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//右上
//	//右
//	vertex.pos = XMFLOAT3(size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//左下
//	vertex.pos = XMFLOAT3(size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//左上
//	vertex.pos = XMFLOAT3(size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//右下
//	vertex.pos = XMFLOAT3(size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//右上
//	//左
//	vertex.pos = XMFLOAT3(-size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//左下
//	vertex.pos = XMFLOAT3(-size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//左上
//	vertex.pos = XMFLOAT3(-size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//右下
//	vertex.pos = XMFLOAT3(-size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//右上
//	//下
//	vertex.pos = XMFLOAT3(-size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//左下
//	vertex.pos = XMFLOAT3(-size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//左上
//	vertex.pos = XMFLOAT3(size.x, -size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//右下
//	vertex.pos = XMFLOAT3(size.x, -size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//右上
//	//上
//	vertex.pos = XMFLOAT3(-size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 0);
//	proSprite.vertex.push_back(vertex);	//左下
//	vertex.pos = XMFLOAT3(-size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(0, 1);
//	proSprite.vertex.push_back(vertex);	//左上
//	vertex.pos = XMFLOAT3(size.x, size.y, -size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 0);
//	proSprite.vertex.push_back(vertex);	//右下
//	vertex.pos = XMFLOAT3(size.x, size.y, size.z);
//	vertex.normal = XMFLOAT3(0, 0, 0);
//	vertex.uv = XMFLOAT2(1, 1);
//	proSprite.vertex.push_back(vertex);	//右上
//#pragma endregion
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	//頂点インデックスの設定
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
//	//法線ベクトルの計算
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//三角形ひとつひとつ計算していく
//		//三角形のインデックスを取り出して、一時的な変数に入れる
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//三角形を構成する頂点座標をベクトルに代入
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1ベクトル,p0->p2ベクトルを計算
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//外戚は両方から垂直なベクトル
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//正規化
//		normal = XMVector3Normalize(normal);
//		//求めた法線を頂点データに代入
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//spriteをsprite配列の最後に生成
//	pair<string, Sprite> pre;
//	pre.first = spriteName;
//	pre.second = proSprite;
//	//sprite.push_back(pre);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//図形をロード
//	ShapeManager::Ins()->LoadShape(ShapeDataCone, proSprite);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(radius, radius, height);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//図形をロード
//	ShapeManager::Ins()->LoadShape(ShapeDataCylinder, proSprite);
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size()),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(radius, radius, height);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	if (fileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//	}
//	else {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->CreateTexture(color);
//	}
//
//	//図形をロード
//	ShapeManager::Ins()->LoadShape(ShapeDataSphere, proSprite);
//
//	//半径を保存
//	proSprite.radius = radius;
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(radius, radius, radius);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	// 全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	// マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		proSprite.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = proSprite.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)proSprite.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 2;										//CBV2つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	if (textureFileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(textureFileName);
//	}
//	else {
//		proSprite.hasTexture = false;
//	}
//
//	//fbxファイルをロード
//	ModelDataManager::LoadFbx(modelFileName, proSprite);
//
//	//頂点バッファの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	//マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
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
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
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
//	//定数バッファへのデータ転送
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
//	//仮の変数
//	Sprite proSprite;
//
//	//パイプランの名前の保存
//	proSprite.piplineName = pipleName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 3;										//CBV3つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&proSprite.constDescHeap));
//
//	//テクスチャをロード
//	if (textureFileName != 0) {
//		proSprite.hasTexture = true;
//		proSprite.textureIDNum = TextureManager::Ins()->LoadTexture(textureFileName);
//	}
//	else {
//		proSprite.hasTexture = false;
//	}
//
//	//fbxファイルをロード
//	ModelDataManager::LoadObj(modelFileName, proSprite, isSmoothing);
//
//	//頂点バッファの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.vertex.size() * sizeof(Vertex)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.vertBuff)
//	);
//	//頂点バッファビューの設定
//	proSprite.vbView.BufferLocation = proSprite.vertBuff->GetGPUVirtualAddress();
//	proSprite.vbView.SizeInBytes = proSprite.vertex.size() * sizeof(Vertex);
//	proSprite.vbView.StrideInBytes = sizeof(Vertex);
//
//	/*-----頂点インデックスバッファの設定-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(proSprite.index.size() * sizeof(unsigned short)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&proSprite.indexBuff)
//	);
//
//	//頂点インデックスバッファビューの設定
//	proSprite.ibView.BufferLocation = proSprite.indexBuff->GetGPUVirtualAddress();
//	proSprite.ibView.Format = DXGI_FORMAT_R16_UINT;
//	proSprite.ibView.SizeInBytes = proSprite.index.size() * sizeof(unsigned short);
//
//	/*-----定数バッファの生成-----*/
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
//	//行列を初期化
//	proSprite.projectionID = projectionID;
//	proSprite.rotationMat = XMMatrixIdentity();
//	proSprite.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	proSprite.positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
//	proSprite.pos = XMFLOAT3(proSprite.positionMat.r[3].m128_f32[0], proSprite.positionMat.r[3].m128_f32[1], proSprite.positionMat.r[3].m128_f32[2]);
//
//	//マップ処理を行う
//	Vertex* vertMap = nullptr;
//	proSprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	//全頂点に対して
//	for (int i = 0; i < proSprite.vertex.size(); ++i)
//	{
//		vertMap[i] = proSprite.vertex.at(i);   // 座標をコピー
//	}
//	//マップを解除
//	proSprite.vertBuff->Unmap(0, nullptr);
//
//	//3Dで頂点インデックスがあるならバッファのデータ転送を行う
//	if (proSprite.index.size() > 0) {
//		unsigned short* indexMap = nullptr;
//		HRESULT result = proSprite.indexBuff->Map(0, nullptr, (void**)&indexMap);
//		for (int i = 0; i < proSprite.index.size(); ++i) {
//			indexMap[i] = proSprite.index.at(i);
//		}
//		proSprite.indexBuff->Unmap(0, nullptr);
//	}
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
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
//	//丸影の生成
//	proSprite.circleShadowNum = -1;
//	if (isCircleShadow == true) {
//		proSprite.circleShadowNum = LightManager::Ins()->GenerateCircleShadow(XMFLOAT3(0, 1, 0), proSprite.pos, 100.0f, XMFLOAT3(0.5f, 0.6f, 0.0f), XMFLOAT2(10.0f, 20.0f), 1);
//	}
//
//	//定数バッファへのデータ転送(行列と視点座標関係)
//	ConstBufferDataB0* constMap = nullptr;
//	result = proSprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	constMap->eye = Camera::eye;
//	proSprite.constBuff0->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(マテリアル関係)
//	ConstBufferDataB1* constMap1 = nullptr;
//	result = proSprite.constBuff1->Map(0, nullptr, (void**)&constMap1);
//	constMap1->ambient = proSprite.constBuffDataB1.ambient;
//	constMap1->diffuse = proSprite.constBuffDataB1.diffuse;
//	constMap1->specular = proSprite.constBuffDataB1.specular;
//	constMap1->alpha = proSprite.constBuffDataB1.alpha;
//	proSprite.constBuff1->Unmap(0, nullptr);
//
//	//定数バッファへのデータ転送(ライト関係)
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
//			//影のキャストを更新
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
//	//非表示状態だったら描画処理を行わない
//	if (sprite.isDisplay == false) return;
//
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(sprite.piplineName);
//
//	//定数バッファへのデータ転送	値が変更されていた時のみデータ転送を行う
//	//if (sprite.isDirty == true) {
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = sprite.constBuff0->Map(0, nullptr, (void**)&constMap);
//	//投影IDがbackGourndの場合は平行投影変換を行う
//	if (sprite.projectionID == backGround) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= sprite.scaleMat;
//		matWorld *= sprite.rotationMat;
//		matWorld *= sprite.positionMat;
//		constMap->mat.world = matWorld;
//		constMap->mat.viewproj = Camera::matProjection;								//平行投影変換
//		constMap->eye = Camera::eye;
//	}
//	//投影IDがobjectの場合はいろいろな変換を行う
//	else if (sprite.projectionID == object) {
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= sprite.scaleMat;
//		matWorld *= sprite.rotationMat;
//		matWorld *= sprite.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//	}
//	//ビルボードの場合
//	else if (sprite.projectionID == billBoard) {
//		//視点座標
//		XMVECTOR eyePosition = XMLoadFloat3(&Camera::eye);
//		//注視点座標
//		XMVECTOR targetPosition = XMLoadFloat3(&Camera::target);
//		//(仮の)上方向
//		XMVECTOR upVector = XMLoadFloat3(&Camera::up);
//		//カメラZ軸
//		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
//		//0ベクトルだと向きが定まらないので除外
//		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
//		assert(!XMVector3IsInfinite(cameraAxisZ));
//		assert(!XMVector3Equal(upVector, XMVectorZero()));
//		assert(!XMVector3IsInfinite(upVector));
//		//ベクトルを正規化
//		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
//		//カメラのX軸方向(右方向)
//		XMVECTOR cameraAxisX{};
//		//X軸は上方向->Z軸の外積で求まる
//		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
//		//ベクトルを正規化
//		cameraAxisX = XMVector3Normalize(cameraAxisX);
//		//カメラのY軸(上方向)
//		XMVECTOR cameraAxisY{};
//		//Y軸はZ軸->X軸の外積で求める
//		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
//		//ベクトルを正規化
//		cameraAxisY = XMVector3Normalize(cameraAxisY);
//		//カメラ回転行列
//		XMMATRIX matCameraRot;
//		//カメラ座標系->ワールド座標系の返還行列
//		matCameraRot.r[0] = cameraAxisX;
//		matCameraRot.r[1] = cameraAxisY;
//		matCameraRot.r[2] = cameraAxisZ;
//		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
//		//転地により逆行列(逆回転)を計算
//		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
//		//視点座標に-1をかけた座標
//		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
//		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
//		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
//		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
//		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
//		//一つのベクトルにまとめる
//		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
//		//ビュー行列に平行移動分を設定
//		matView.r[3] = translation;
//
//		//ビルボード行列
//		XMMATRIX billboardMat = XMMatrixIdentity();
//		billboardMat.r[0] = cameraAxisX;
//		billboardMat.r[1] = cameraAxisY;
//		billboardMat.r[2] = cameraAxisZ;
//		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
//		//ワールド行列の更新
//		XMMATRIX matWorld = XMMatrixIdentity();
//		matWorld *= billboardMat;
//		matWorld *= sprite.scaleMat;
//		matWorld *= sprite.rotationMat;
//		matWorld *= sprite.positionMat;
//		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//		constMap->mat.viewproj = matView * Camera::matPerspective;
//		constMap->eye = Camera::eye;
//	}
//	sprite.constBuff0->Unmap(0, nullptr);
//
//	//定数バッファの数が２つ以上(マテリアルを持っている)なら２つ目の定数バッファのマップ処理も行う
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
//				//影のキャストを更新
//				LightManager::Ins()->circleShadows[sprite.circleShadowNum].centerpos = XMFLOAT3(sprite.pos.x, sprite.pos.y + 280, sprite.pos.z);
//			}
//			constMap2->circleShadowData[i] = LightManager::Ins()->circleShadows[i];
//		}
//		sprite.constBuff2->Unmap(0, nullptr);
//	}
//
//	//ダーティフラグをfalseにする
//	//sprite.isDirty = false;
//	//}
//
//	//座標を保存しておく
//	sprite.pos = XMFLOAT3(sprite.positionMat.r[3].m128_f32[0], sprite.positionMat.r[3].m128_f32[1], sprite.positionMat.r[3].m128_f32[2]);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, sprite.constBuff0->GetGPUVirtualAddress());
//	if (sprite.constBuffCount >= 2) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(1, sprite.constBuff1->GetGPUVirtualAddress());
//	}
//	if (sprite.constBuffCount >= 3) {
//		DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(2, sprite.constBuff2->GetGPUVirtualAddress());
//	}
//
//	//ディスクリプタヒープ設定コマンド		スプライトがテクスチャのデータを持っていた場合のみ設定コマンドを実行する
//	if (sprite.hasTexture == true) {
//		ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//		//シェーダーリソースビュー設定コマンド
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
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &sprite.vbView);
//
//	//3Dで頂点インデックスがあるなら頂点インデックスバッファビュー設定コマンドを実行する
//	if (sprite.index.size() > 0) {
//		DirectXBase::Ins()->cmdList->IASetIndexBuffer(&sprite.ibView);
//	}
//
//	//描画コマンド
//	if (sprite.index.size() > 0) {
//		DirectXBase::Ins()->cmdList->DrawIndexedInstanced(sprite.index.size(), 1, 0, 0, 0);
//	}
//	else {
//		DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
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
//	//パイプランの名前の保存
//	data.piplineName = piplineName;
//
//	//設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
//	descHeapDesc.NumDescriptors = 1;										//CBV1つ
//	//ディスクリプタヒープの生成
//	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&data.constDescHeap));
//
//	//テクスチャをロード
//	data.textureIDNum = TextureManager::Ins()->LoadTexture(fileName);
//
//	//頂点バッファの生成
//	data.vertex = {};
//
//	//頂点バッファビューの生成
//	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMFLOAT3)),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&data.vertBuff)
//	);
//
//	//頂点バッファビューの設定
//	data.vbView.BufferLocation = data.vertBuff->GetGPUVirtualAddress();
//	data.vbView.SizeInBytes = sizeof(XMFLOAT3);
//	data.vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	/*-----定数バッファの生成-----*/
//	result = DirectXBase::Ins()->dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&data.constBuff0)
//	);
//
//	//行列を初期化
//	data.rotationMat = XMMatrixIdentity();
//	data.scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
//	data.positionMat = XMMatrixTranslation(pos.x, pos.y, pos.z);
//
//	//マップ処理を行う
//	XMFLOAT3* vertMap = nullptr;
//	data.vertBuff->Map(0, nullptr, (void**)&vertMap);
//	vertMap = &data.vertex;		//座標をコピー
//	//マップを解除
//	data.vertBuff->Unmap(0, nullptr);
//
//	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
//	//CBVディスクリプタヒープの先頭アドレスを取得
//	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
//		data.constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//	cbvDesc.BufferLocation = data.constBuff0->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)data.constBuff0->GetDesc().Width;
//	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	result = data.constBuff0->Map(0, nullptr, (void**)&constMap);
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= data.scaleMat;
//	matWorld *= data.rotationMat;
//	matWorld *= data.positionMat;
//	constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//	constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//	constMap->eye = Camera::eye;
//	data.constBuff0->Unmap(0, nullptr);
//
//	return data;
//}
//
//void SpriteManager::DrawGSParticle(GSParticleData data)
//{
//	//パイプラインとルートシグネチャの設定
//	PiplineManager::Ins()->SetPipline(data.piplineName);
//
//	//定数バッファへのデータ転送
//	ConstBufferDataB0* constMap = nullptr;
//	HRESULT result = data.constBuff0->Map(0, nullptr, (void**)&constMap);
//	//ワールド行列の更新
//	XMMATRIX matWorld = XMMatrixIdentity();
//	matWorld *= data.scaleMat;
//	matWorld *= data.rotationMat;
//	matWorld *= data.positionMat;
//	constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
//	constMap->mat.viewproj = Camera::matView * Camera::matPerspective;
//	constMap->eye = Camera::eye;
//	data.constBuff0->Unmap(0, nullptr);
//
//	//定数バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, data.constBuff0->GetGPUVirtualAddress());
//
//	//ディスクリプタヒープ設定コマンド		スプライトがテクスチャのデータを持っていた場合のみ設定コマンドを実行する
//	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
//	//シェーダーリソースビュー設定コマンド
//	DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Ins()->GetSRV(data.textureIDNum));
//
//	//頂点バッファビュー設定コマンド
//	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &data.vbView);
//
//	//描画コマンド
//	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
//	DirectXBase::Ins()->cmdList->DrawInstanced(1, 1, 0, 0);
//}

#pragma endregion
