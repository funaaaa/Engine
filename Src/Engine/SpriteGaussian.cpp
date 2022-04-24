#include "SpriteGaussian.h"
#include "TextureManager.h"
#include "DirectXBase.h"

void SpriteGaussian::CommonGenerate(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID)
{
	//パイプランの名前の保存
	this->piplineID = piplineID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&constDescHeap));

	//頂点バッファの生成
	Vertex vertexBuff;
	vertexBuff.pos = XMFLOAT3(-size.x, size.y, 10);		//左下
	vertexBuff.uv = XMFLOAT2(0, 1);
	vertex.push_back(vertexBuff);
	vertexBuff.pos = XMFLOAT3(-size.x, -size.y, 10);	//左上
	vertexBuff.uv = XMFLOAT2(0, 0);
	vertex.push_back(vertexBuff);
	vertexBuff.pos = XMFLOAT3(size.x, size.y, 10);		//右下
	vertexBuff.uv = XMFLOAT2(1, 1);
	vertex.push_back(vertexBuff);
	vertexBuff.pos = XMFLOAT3(size.x, -size.y, 10);		//右上
	vertexBuff.uv = XMFLOAT2(1, 0);
	vertex.push_back(vertexBuff);

	//頂点バッファビューの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertex.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	//頂点バッファビューの設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertex.size() * sizeof(Vertex);
	vbView.StrideInBytes = sizeof(Vertex);

	/*-----定数バッファの生成-----*/
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataMultiPath) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1)
	);

	//行列を初期化
	projectionID = projectionID;
	rotationMat = XMMatrixIdentity();
	scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);

	//マップ処理を行う
	Vertex* vertMap = nullptr;
	vertBuff->Map(0, nullptr, (void**)&vertMap);
	// 全頂点に対して
	for (int i = 0; i < vertex.size(); ++i)
	{
		vertMap[i] = vertex.at(i);   // 座標をコピー
	}
	// マップを解除
	vertBuff->Unmap(0, nullptr);

	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuffB0->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB0->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
	//２つ目を生成
	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	cbvDesc.BufferLocation = constBuffB1->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB1->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
}

void SpriteGaussian::GenerateForTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, LPCWSTR fileName)
{
	// テクスチャをロード
	textureID.push_back(TextureManager::Instance()->LoadTexture(fileName));

	// 初期化処理
	CommonGenerate(centerPos, size, projectionID, piplineID);
}

void SpriteGaussian::GenerateForColor(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color)
{
	// テクスチャをロード
	textureID.push_back(TextureManager::Instance()->CreateTexture(color));

	// 初期化処理
	CommonGenerate(centerPos, size, projectionID, piplineID);
}

void SpriteGaussian::GenerateSpecifyTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID)
{
	// テクスチャをロード
	this->textureID.push_back(textureID);

	// 初期化処理
	CommonGenerate(centerPos, size, projectionID, piplineID);
}

void SpriteGaussian::Draw()
{
	// 非表示状態だったら描画処理を行わない
	if (isDisplay == false) return;

	// パイプラインとルートシグネチャの設定
	PiplineManager::Instance()->SetPipline(piplineID);

	// 定数バッファB0構造体をマップ処理
	MapConstDataB0(constBuffB0, constBufferDataB0);

	// 定数バッファ1へのデータ転送
	ConstBufferDataMultiPath* constMap2 = nullptr;
	HRESULT result = constBuffB1->Map(0, nullptr, (void**)&constMap2);
	//constBufferDataB1.gaussianWeight[0] = {};
	//constBufferDataB1.gaussianWeight[1] = {};
	constMap2->gaussianWeight[0] = constBufferDataB1.gaussianWeight[0];
	constMap2->gaussianWeight[1] = constBufferDataB1.gaussianWeight[1];
	constBuffB1->Unmap(0, nullptr);

	// 座標を保存しておく
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);

	// 定数バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	// ディスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Instance()->GetDescHeap().Get() };
	DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

	// シェーダーリソースビュー設定コマンド
	for (int i = 0; i < textureID.size(); ++i) {
		DirectXBase::Instance()->cmdList->SetGraphicsRootDescriptorTable(i + 2, TextureManager::Instance()->GetSRV(textureID[i]));
	}

	// 頂点バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	// 描画コマンド
	DirectXBase::Instance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
	DirectXBase::Instance()->cmdList->DrawInstanced(vertex.size(), 1, 0, 0);
}

void SpriteGaussian::ChangeConstBufferDataB1(vector<float> amount)
{
	constBufferDataB1.gaussianWeight[0] = { amount[0],amount[1],amount[2],amount[3] };
	constBufferDataB1.gaussianWeight[1] = { amount[4],amount[5],amount[6],amount[7] };
}
