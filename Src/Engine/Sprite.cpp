#include "Sprite.h"
#include "TextureManager.h"
#include "Engine.h"
#include "PipelineManager.h"
#include "DescriptorHeapMgr.h"

void Sprite::CommonGenerate(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID)
{
	// パイプランの名前の保存
	this->piplineID_ = PiplineID;

	// 設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// シェーダーから見える
	descHeapDesc.NumDescriptors = 2;										// CBV2つ
	// ディスクリプタヒープの生成
	Engine::Ins()->dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&constDescHeap_));

	// 頂点バッファの生成
	Vertex vertexBuff;
	vertexBuff.pos_ = Vec3(-1.0f, 1.0f, 10);		// 左下
	vertexBuff.uv_ = Vec2(0, 1);
	vertex_.push_back(vertexBuff);
	vertexBuff.pos_ = Vec3(-1.0f, -1.0f, 10);	// 左上
	vertexBuff.uv_ = Vec2(0, 0);
	vertex_.push_back(vertexBuff);
	vertexBuff.pos_ = Vec3(1.0f, 1.0f, 10);		// 右下
	vertexBuff.uv_ = Vec2(1, 1);
	vertex_.push_back(vertexBuff);
	vertexBuff.pos_ = Vec3(1.0f, -1.0f, 10);		// 右上
	vertexBuff.uv_ = Vec2(1, 0);
	vertex_.push_back(vertexBuff);

	// 頂点バッファビューの生成
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

	// 頂点バッファビューの設定
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = static_cast<UINT>(vertex_.size()) * static_cast<UINT>(sizeof(Vertex));
	vbView_.StrideInBytes = sizeof(Vertex);

	/*-----定数バッファの生成-----*/
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

	// 行列を初期化
	projectionID_ = ProjectionID;
	rotationMat_ = DirectX::XMMatrixIdentity();
	scaleMat_ = DirectX::XMMatrixScaling(Size.x_, Size.y_, 1.0f);
	positionMat_ = DirectX::XMMatrixTranslation(CenterPos.x_, CenterPos.y_, CenterPos.z_);
	pos_ = CenterPos;

	// マップ処理を行う
	Vertex* vertMap = nullptr;
	vertBuff_->Map(0, nullptr, (void**)&vertMap);
	// 全頂点に対して
	for (int i = 0; i < vertex_.size(); ++i)
	{
		vertMap[i] = vertex_.at(i);   // 座標をコピー
	}
	// マップを解除
	vertBuff_->Unmap(0, nullptr);

	/*-----CBVディスクリプタヒープの生成 定数バッファの情報をGPUに伝えるための定数バッファビュー用-----*/
	// CBVディスクリプタヒープの先頭アドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap_->GetCPUDescriptorHandleForHeapStart(), 0, Engine::Ins()->dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuffB0_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB0_->GetDesc().Width;
	Engine::Ins()->dev_->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
}

void Sprite::GenerateForTexture(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, LPCWSTR FileName)
{

	// テクスチャをロード
	textureID_.push_back(TextureManager::Ins()->LoadTexture(FileName));

	// 初期化処理
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);

}

void Sprite::GenerateForColor(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, DirectX::XMFLOAT4 Color)
{

	// テクスチャをロード
	textureID_.push_back(TextureManager::Ins()->CreateTexture(Color));

	// 初期化処理
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);

}

void Sprite::GenerateSpecifyTextureID(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, int TextureID)
{

	// テクスチャをロード
	this->textureID_.push_back(TextureID);

	// 初期化処理
	CommonGenerate(CenterPos, Size, ProjectionID, PiplineID);


}

void Sprite::Draw()
{
	// 非表示状態だったら描画処理を行わない
	if (isDisplay_ == false) return;

	// パイプラインとルートシグネチャの設定
	PipelineManager::Ins()->SetPipline(piplineID_);

	// 定数バッファB0構造体をマップ処理
	MapConstDataB0(constBuffB0_, constBufferDataB0_);

	// 座標を保存しておく
	pos_ = Vec3(positionMat_.r[3].m128_f32[0], positionMat_.r[3].m128_f32[1], positionMat_.r[3].m128_f32[2]);

	// 定数バッファビュー設定コマンド
	Engine::Ins()->mainGraphicsCmdList_->SetGraphicsRootConstantBufferView(0, constBuffB0_->GetGPUVirtualAddress());

	// ディスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps2[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	Engine::Ins()->mainGraphicsCmdList_->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

	// シェーダーリソースビュー設定コマンド
	for (int index = 0; index < textureID_.size(); ++index) {
		Engine::Ins()->mainGraphicsCmdList_->SetGraphicsRootDescriptorTable(index + 1, TextureManager::Ins()->GetSRV(textureID_[index]));
	}

	// 頂点バッファビュー設定コマンド
	Engine::Ins()->mainGraphicsCmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// 描画コマンド
	Engine::Ins()->mainGraphicsCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//ここの引数を変えることで頂点を利用してどんな図形を描くかを設定できる 資料3_3
	Engine::Ins()->mainGraphicsCmdList_->DrawInstanced(static_cast<UINT>(vertex_.size()), 1, 0, 0);
}
