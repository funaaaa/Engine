#include "GSParticle.h"
#include "Camera.h"
#include "TextureManager.h"
#include "DirectXBase.h"

void GSParticle::CommonGenerate(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int instanceCount)
{

	//パイプランの名前の保存
	this->piplineID = piplineID;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			//シェーダーから見える
	descHeapDesc.NumDescriptors = 1;										//CBV1つ
	//ディスクリプタヒープの生成
	DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&constDescHeap));

	//頂点バッファの生成
	vertex = {};

	//頂点バッファビューの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMFLOAT3)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	//頂点バッファビューの設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(XMFLOAT3);
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	/*-----定数バッファの生成-----*/
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(GSConstBufferDataB0) * instanceCount + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);

	//行列を初期化
	rotationMat = XMMatrixIdentity();
	scaleMat = XMMatrixScaling(size.x, size.y, 1.0f);
	positionMat = XMMatrixTranslation(pos.x, pos.y, pos.z);

	//マップ処理を行う
	XMFLOAT3* vertMap = nullptr;
	vertBuff->Map(0, nullptr, (void**)&vertMap);
	vertMap = &vertex;		//座標をコピー
	//マップを解除
	vertBuff->Unmap(0, nullptr);

	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
	//CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuffB0->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB0->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	//定数バッファへのデータ転送
	GSConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	XMMATRIX matWorld = XMMatrixIdentity();
	matWorld *= scaleMat;
	matWorld *= rotationMat;
	matWorld *= positionMat;
	constMap->mat = matWorld * Camera::matView * Camera::matPerspective;	//ワールド変換 * ビュー変換 * 透視投影変換
	constBuffB0->Unmap(0, nullptr);

}

void GSParticle::GenerateForTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, LPCWSTR fileName, int instanceCount)
{

	// テクスチャをロード
	textureID.push_back(TextureManager::Instance()->LoadTexture(fileName));

	// 共通の生成処理を行う
	CommonGenerate(centerPos, size, projectionID, piplineID, instanceCount);
}

void GSParticle::GenerateForColor(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color, int instanceCount)
{
	// テクスチャをロード
	textureID.push_back(TextureManager::Instance()->CreateTexture(color));

	// 共通の生成処理を行う
	CommonGenerate(centerPos, size, projectionID, piplineID, instanceCount);
}

void GSParticle::GenerateSpecifyTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID, int instanceCount)
{
	// テクスチャをロード
	this->textureID.push_back(textureID);

	// 共通の生成処理を行う
	CommonGenerate(centerPos, size, projectionID, piplineID, instanceCount);
}

void GSParticle::Draw()
{
	//パイプラインとルートシグネチャの設定
	PiplineManager::Instance()->SetPipline(piplineID);

	//定数バッファへのデータ転送
	GSConstBufferDataB0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	//ワールド行列の更新
	XMMATRIX matWorld = XMMatrixIdentity();
	matWorld *= scaleMat;
	matWorld *= rotationMat;
	matWorld *= positionMat;
	constMap->mat = matWorld * Camera::matView * Camera::matPerspective;		//ワールド変換 * ビュー変換 * 透視投影変換
	constMap->color = constBufferDataB0.color;
	constBuffB0->Unmap(0, nullptr);

	//定数バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//ディスクリプタヒープ設定コマンド		スプライトがテクスチャのデータを持っていた場合のみ設定コマンドを実行する
	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Instance()->GetDescHeap().Get() };
	DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
	//シェーダーリソースビュー設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Instance()->GetSRV(textureID[0]));

	//頂点バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	//描画コマンド
	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
	DirectXBase::Instance()->cmdList->DrawInstanced(1, 1, 0, 0);

}

void GSParticle::DrawInstance(vector<GSConstBufferDataB0> instanceData)
{
	int instanceCount = instanceData.size();

	//パイプラインとルートシグネチャの設定
	PiplineManager::Instance()->SetPipline(piplineID);

	//定数バッファへのデータ転送
	GSConstBufferDataB0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);

	for (int index = 0; index < instanceCount; ++index) {

		//ワールド行列の更新
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap[index].mat = instanceData[index].mat * Camera::matView * Camera::matPerspective;		//ワールド変換 * ビュー変換 * 透視投影変換
		constMap[index].color = instanceData[index].color;

	}

	constBuffB0->Unmap(0, nullptr);

	//定数バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//ディスクリプタヒープ設定コマンド		スプライトがテクスチャのデータを持っていた場合のみ設定コマンドを実行する
	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Instance()->GetDescHeap().Get() };
	DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
	//シェーダーリソースビュー設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootDescriptorTable(1, TextureManager::Instance()->GetSRV(textureID[0]));

	//頂点バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	//描画コマンド
	DirectXBase::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
	DirectXBase::Instance()->cmdList->DrawInstanced(1, instanceCount, 0, 0);

}
