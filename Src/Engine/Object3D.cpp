#include "Object3D.h"
#include "TextureManager.h"
#include "ModelDataManager.h"
#include "Enum.h"
#include "Camera.h"
#include "LightManager.h"
#include "LightCamera.h"
#include "DirectXBase.h"

void Object3D::Generate(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string modelFileName, LPCWSTR textureFileName, bool isSmoothing)
{
	// 各変数を初期化
	isAlreadyMapB1 = false;

	// パイプランの名前の保存
	this->piplineID = piplineID;

	// パイプラインのIDによってライティングのマップ処理を行うかどうかのフラグを更新する。
	if (piplineID == PIPLINE_OBJECT_LIGHT_ALPHA ||
		piplineID == PIPLINE_OBJECT_NOLIGHT_ALPHA ||
		piplineID == PIPLINE_OBJECT_LIGHT_FOG_ALPHA ||
		piplineID == PIPLINE_OBJECT_LIGHT_FOG_BUMP_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA_F ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_MRT_D_ALPHA ||
		piplineID == PIPLINE_OBJECT_BUMP_ALPHA ||
		piplineID == PIPLINE_OBJECT_LIGHT_SHADOWMAP_ALPHA) {

		// ライティングをさせる。定数バッファb1b2を送る。
		isLighting = true;

	}
	else {

		// ライティングをさせない。
		isLighting = false;

	}

	// 設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// シェーダーから見える
	descHeapDesc.NumDescriptors = 32;										// CBV3つ
	// ディスクリプタヒープの生成
	HRESULT resultBuff = DirectXBase::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&constDescHeap));

	// テクスチャをロード
	textureID.push_back(TextureManager::Instance()->LoadTexture(textureFileName));

	// objファイルをロード
	Object3DDeliveryData objectDataBuffer;
	ModelDataManager::LoadObj(directoryPath, modelFileName, objectDataBuffer, isSmoothing);

	// ロードした頂点データを代入
	int vertexSize = objectDataBuffer.vertex.size();
	BaseDrawData::Vertex vertDeliveryDataBuffer;
	for (int i = 0; i < vertexSize; ++i) {
		vertDeliveryDataBuffer.normal = objectDataBuffer.vertex[i].normal;
		vertDeliveryDataBuffer.pos = objectDataBuffer.vertex[i].pos;
		vertDeliveryDataBuffer.uv = objectDataBuffer.vertex[i].uv;
		vertex.push_back(vertDeliveryDataBuffer);
	}

	// ロードした頂点インデックスデータを代入
	int indexSize = objectDataBuffer.index.size();
	for (int i = 0; i < indexSize; ++i) {
		index.push_back(objectDataBuffer.index[i]);
	}

	// ロードしたマテリアルを代入
	constBufferDataB1 = objectDataBuffer.constBufferDataB1;

	// 頂点バッファの生成
	HRESULT result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertex.size() * sizeof(Vertex)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	// 頂点バッファビューの設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertex.size() * sizeof(Vertex);
	vbView.StrideInBytes = sizeof(Vertex);

	/*-----頂点インデックスバッファの設定-----*/
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(index.size() * sizeof(unsigned short)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	// 頂点インデックスバッファビューの設定
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = index.size() * sizeof(unsigned short);

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
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1)
	);
	result = DirectXBase::Instance()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB2) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB2)
	);

	// 行列を初期化
	this->projectionID = projectionID;
	rotationMat = XMMatrixIdentity();
	scaleMat = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	positionMat = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);

	// マップ処理を行う
	Vertex* vertMap = nullptr;
	vertBuff->Map(0, nullptr, (void**)&vertMap);
	// 全頂点に対して
	for (int i = 0; i < vertex.size(); ++i)
	{
		vertMap[i] = vertex.at(i);   //  座標をコピー
	}
	// マップを解除
	vertBuff->Unmap(0, nullptr);

	// 3Dで頂点インデックスがあるならバッファのデータ転送を行う
	if (index.size() > 0) {
		unsigned short* indexMap = nullptr;
		HRESULT result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		for (int i = 0; i < index.size(); ++i) {
			indexMap[i] = index.at(i);
		}
		indexBuff->Unmap(0, nullptr);
	}

	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
	// CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 0, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuffB0->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB0->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 1, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	cbvDesc.BufferLocation = constBuffB1->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB1->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), 2, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	cbvDesc.BufferLocation = constBuffB2->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB2->GetDesc().Width;
	DirectXBase::Instance()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// 定数バッファへのデータ転送(行列と視点座標関係)
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	constMap->eye = Camera::Instance()->eye;
	constBuffB0->Unmap(0, nullptr);

	// ライティングをしていない状態だったらライティングを行わない
	if (isLighting) {

		// B1がまだマップ処理していなかったらマップ処理を行う
		if (!isAlreadyMapB1) {

			// 定数バッファへのデータ転送(マテリアル関係)
			ConstBufferDataB1* constMap1 = nullptr;
			result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
			// 転送処理を行う。
			memcpy(constMap1, &constBufferDataB1, sizeof(ConstBufferDataB1));

			// B1をマップ処理したことを記録する
			isAlreadyMapB1 = true;

		}

		// 定数バッファへのデータ転送(ライト関係)
		ConstBufferDataB2* constMap2 = nullptr;
		result = constBuffB2->Map(0, nullptr, (void**)&constMap2);
		for (int i = 0; i < DIRLIGHT_NUM; ++i) {
			constMap2->dirlights[i] = LightManager::Instance()->dirlights[i];
		}
		for (int i = 0; i < POINTLIGHT_NUM; ++i) {
			constMap2->pointlights[i] = LightManager::Instance()->pointlights[i];
		}
		for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
			constMap2->spotlights[i] = LightManager::Instance()->spotlights[i];
		}
		constBuffB2->Unmap(0, nullptr);

	}

}

void Object3D::Draw()
{
	// 非表示状態だったら描画処理を行わない
	if (isDisplay == false) return;

	// パイプラインとルートシグネチャの設定
	PiplineManager::Instance()->SetPipline(piplineID);

	// 定数バッファb0のマップ処理を行う
	MapConstDataB0(constBuffB0, constBufferDataB0);

	// ライティングを行う場合のみマップ処理を行う
	if (isLighting) {

		// B1がまだマップ処理していなかったらマップ処理を行う
		if (!isAlreadyMapB1) {

			// 定数バッファb1のマップ処理を行う
			ConstBufferDataB1* constMap1 = nullptr;
			HRESULT result = constBuffB1->Map(0, nullptr, (void**)&constMap1);

			// 転送処理を行う。
			memcpy(constMap1, &constBufferDataB1, sizeof(ConstBufferDataB1));

			constBuffB1->Unmap(0, nullptr);

			// B1をマップ処理したことを記録する
			isAlreadyMapB1 = true;

		}

		// 定数バッファb2のマップ処理を行う
		ConstBufferDataB2* constMap2 = nullptr;
		HRESULT result = constBuffB2->Map(0, nullptr, (void**)&constMap2);

		for (int i = 0; i < DIRLIGHT_NUM; ++i) {
			constMap2->dirlights[i] = LightManager::Instance()->dirlights[i];
		}
		for (int i = 0; i < POINTLIGHT_NUM; ++i) {
			constMap2->pointlights[i] = LightManager::Instance()->pointlights[i];
		}
		for (int i = 0; i < SPOTLIGHT_NUM; ++i) {
			constMap2->spotlights[i] = LightManager::Instance()->spotlights[i];
		}

		constBuffB2->Unmap(0, nullptr);

	}

	// 座標を保存しておく
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);

	// 定数バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// SRVに足すオフセットの値を計算
	int offsetSRV = 0;

	// ライティングを行わない場合は定数バッファビューを設定しない
	if (isLighting) {

		DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());
		DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(2, constBuffB2->GetGPUVirtualAddress());

		offsetSRV = 3;

	}
	else {

		offsetSRV = 1;

	}

	// 追加された定数バッファの数
	const int ADD_CONSTBUFFER_AMOUNT = constBufferData.size();

	// 追加された定数バッファをセットする。
	for (int index = 0; index < ADD_CONSTBUFFER_AMOUNT; ++index) {

		DirectXBase::Instance()->cmdList->SetGraphicsRootConstantBufferView(offsetSRV + index, constBufferData[index]->GetGPUVirtualAddress());

	}

	// 定数バッファが追加されている分オフセットをずらす。
	offsetSRV += ADD_CONSTBUFFER_AMOUNT;

	// ディスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps2[] = { TextureManager::Instance()->GetDescHeap().Get() };
	DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);
	// シェーダーリソースビュー設定コマンド
	for (int i = 0; i < textureID.size(); ++i) {
		DirectXBase::Instance()->cmdList->SetGraphicsRootDescriptorTable(i + offsetSRV, TextureManager::Instance()->GetSRV(textureID[i]));
	}

	// 頂点バッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->IASetVertexBuffers(0, 1, &vbView);

	// 頂点インデックスバッファビュー設定コマンド
	DirectXBase::Instance()->cmdList->IASetIndexBuffer(&ibView);

	// 描画コマンド
	DirectXBase::Instance()->cmdList->DrawIndexedInstanced(index.size(), 1, 0, 0, 0);
}

void Object3D::UpdatePiplineID(int piplineID)
{
	// パイプラインのIDによってライティングのマップ処理を行うかどうかのフラグを更新する。
	if (piplineID == PIPLINE_OBJECT_LIGHT_ALPHA ||
		piplineID == PIPLINE_OBJECT_NOLIGHT_ALPHA ||
		piplineID == PIPLINE_OBJECT_LIGHT_FOG_ALPHA ||
		piplineID == PIPLINE_OBJECT_LIGHT_FOG_BUMP_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA_F ||
		piplineID == PIPLINE_OBJECT_TOONSHADER_SHADOW_MRT_D_ALPHA ||
		piplineID == PIPLINE_OBJECT_BUMP_ALPHA ||
		piplineID == PIPLINE_OBJECT_LIGHT_SHADOWMAP_ALPHA) {

		// ライティングをさせる。定数バッファb1b2を送る。
		isLighting = true;

	}
	else {

		// ライティングをさせない。
		isLighting = false;

	}

	// パイプラインIDを更新
	this->piplineID = piplineID;

}
