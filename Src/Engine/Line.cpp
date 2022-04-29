#include "Line.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include "Camera.h"

void Line::Generate(const XMFLOAT3& pos1, const XMFLOAT3& pos2, const XMFLOAT4& color, int projectionID, int piplineID)
{
	//パイプランの名前の保存
	this->piplineID = piplineID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										//CBV2つ
	//ディスクリプタヒープの生成
	DirectXBase::Ins()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&constDescHeap));

	//テクスチャをロード
	textureID.push_back(TextureManager::Ins()->CreateTexture(color));

	//頂点バッファの生成
	Vertex vertexBuff;
	vertexBuff.pos = pos1;	//左下
	vertexBuff.uv = XMFLOAT2(0, 0);
	vertex.push_back(vertexBuff);
	vertexBuff.pos = pos2;		//左上
	vertexBuff.uv = XMFLOAT2(0, 0);
	vertex.push_back(vertexBuff);

	//頂点バッファビューの生成
	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
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
	result = DirectXBase::Ins()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);

	//行列を初期化
	this->projectionID = projectionID;
	rotationMat = XMMatrixIdentity();
	scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	positionMat = XMMatrixIdentity();
	this->pos1 = pos1;
	this->pos2 = pos2;

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
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuffB0->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB0->GetDesc().Width;
	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

}

void Line::Draw()
{
	//非表示状態だったら描画処理を行わない
	if (isDisplay == false) return;

	//パイプラインとルートシグネチャの設定
	PiplineManager::Ins()->SetPipline(piplineID);

	// 定数バッファB0構造体をマップ処理
	MapConstDataB0(constBuffB0, constBufferDataB0);

	//定数バッファビュー設定コマンド
	DirectXBase::Ins()->cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//ディスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Ins()->GetDescHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
	//シェーダーリソースビュー設定コマンド
	for (int i = 0; i < textureID.size(); ++i) {
		DirectXBase::Ins()->cmdList->SetGraphicsRootDescriptorTable(i + 1, TextureManager::Ins()->GetSRV(textureID[0]));
	}

	//頂点バッファビュー設定コマンド
	DirectXBase::Ins()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	//描画コマンド
	//DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
	DirectXBase::Ins()->cmdList->DrawInstanced(vertex.size(), 1, 0, 0);
}

void Line::ChangeVertexPos(const XMFLOAT3& pos1, const XMFLOAT3& pos2)
{
	vertex[0].pos = pos1;
	vertex[1].pos = pos2;

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

	this->pos1 = pos1;
	this->pos2 = pos2;
}
