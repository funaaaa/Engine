#include "TextureManager.h"
#include "Engine.h"
#include "DescriptorHeapMgr.h"
#include <array>
#include <DirectXTex/DirectXTexDDS.cpp>

TextureManager::TextureManager() {
}

int TextureManager::LoadTexture(LPCWSTR FileName) {

	// ファイルがロード済みかをチェック
	if (0 < texture_.size()) {

		int counter = 0;
		for (auto& index_ : texture_) {

			// ロードしてあったら識別番号を返す
			if (index_.filePath_ == FileName) {

				return descriptorHeadMgrIndex_[counter];

			}
			++counter;
		}
	}

	// テクスチャデータを保存
	Texture proTexture{};
	proTexture.filePath_ = FileName;

	//// ロードしていなかったらロードする
	//DirectX::TexMetadata metadata_;
	//DirectX::ScratchImage scratchImg_;
	//HRESULT result = LoadFromWICFile(
	//	FileName,
	//	DirectX::WIC_FLAGS_NONE,
	//	&metadata_, scratchImg_
	//);

	DirectX::TexMetadata metadata_;
	DirectX::ScratchImage scratchImg_;
	HRESULT result = LoadFromDDSFile(FileName, DDS_FLAGS_NONE, &metadata_, scratchImg_);
	if (FAILED(result)) {
		result = LoadFromWICFile(FileName, WIC_FLAGS_NONE/*WIC_FLAGS_FORCE_RGB*/, &metadata_, scratchImg_);
	}
	if (FAILED(result)) {
		assert(0);
	}

	const DirectX::Image* img = scratchImg_.GetImage(0, 0, 0);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata_.format,
		static_cast<UINT>(metadata_.width),
		static_cast<UINT>(metadata_.height),
		static_cast<UINT16>(metadata_.arraySize),
		static_cast<UINT16>(metadata_.mipLevels));

	// テクスチャバッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&texbuff));


	// ここから追記。

	// Footprint(コピー可能なリソースのレイアウト)を取得。
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT64 totalBytes = 0;
	Engine::Ins()->device_.dev_->GetCopyableFootprints(&texresDesc, 0, 1, 0, &footprint, nullptr, nullptr, &totalBytes);

	// Upload用のバッファを作成する。
	D3D12_RESOURCE_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = totalBytes;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES heap;
	memset(&heap, 0, sizeof(heap));
	heap.Type = D3D12_HEAP_TYPE_UPLOAD;

	ID3D12Resource* iUploadBuffer = nullptr;
	Engine::Ins()->device_.dev_->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&iUploadBuffer));

	// UploadBufferへの書き込み。
	void* ptr = nullptr;
	iUploadBuffer->Map(0, nullptr, &ptr);
	memcpy(reinterpret_cast<unsigned char*>(ptr) + footprint.Offset, img->pixels, img->slicePitch);

	// Copyコマンドの作成
	D3D12_TEXTURE_COPY_LOCATION dest;
	memset(&dest, 0, sizeof(dest));
	dest.pResource = texbuff.Get();
	dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dest.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION  src;
	memset(&src, 0, sizeof(src));
	src.pResource = iUploadBuffer;
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = footprint;

	Engine::Ins()->mainGraphicsCmdList_->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);

	// リソースバリア。
	D3D12_RESOURCE_BARRIER  barrier;
	memset(&barrier, 0, sizeof(barrier));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = texbuff.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// コピーコマンドの実行。
	Engine::Ins()->mainGraphicsCmdList_->Close();
	ID3D12CommandList* commandLists[] = { Engine::Ins()->mainGraphicsCmdList_.Get() };
	Engine::Ins()->graphicsCmdQueue_->ExecuteCommandLists(1, commandLists);
	Engine::Ins()->graphicsCmdQueue_->Signal(Engine::Ins()->asFence_.Get(), ++Engine::Ins()->asfenceValue_);

	// グラフィックコマンドリストの完了待ち
	UINT64 gpuFence = Engine::Ins()->asFence_->GetCompletedValue();
	if (gpuFence != Engine::Ins()->asfenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		Engine::Ins()->asFence_->SetEventOnCompletion(Engine::Ins()->asfenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// コマンドアロケータのリセット
	Engine::Ins()->mainGraphicsCmdAllocator_->Reset();	//キューをクリア

	// コマンドリストのリセット
	Engine::Ins()->mainGraphicsCmdList_->Reset(Engine::Ins()->mainGraphicsCmdAllocator_.Get(), nullptr);//再びコマンドリストを貯める準備


	// ここまで追記。

	// テクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	proTexture.metadata_ = metadata_;
	proTexture.scratchImg_ = &scratchImg_;
	proTexture.texBuff_ = texbuff;
	texture_.emplace_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata_.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT_MAX;
	// ヒープにシェーダーリソースビュー生成
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

int TextureManager::LoadTexture(std::array<wchar_t, 128> FileName)
{
	// ファイルがロード済みかをチェック
	if (0 < texture_.size()) {

		int counter = 0;
		for (auto& index_ : texture_) {

			// ロードしてあったら識別番号を返す
			if (index_.filePath_ == FileName.data()) {

				return descriptorHeadMgrIndex_[counter];

			}
			++counter;
		}
	}

	// テクスチャデータを保存
	Texture proTexture{};
	proTexture.filePathP_ = FileName;
	proTexture.filePath_ = proTexture.filePathP_.data();

	// ロードしていなかったらロードする
	DirectX::TexMetadata metadata_;
	DirectX::ScratchImage scratchImg_;
	HRESULT result = LoadFromDDSFile(proTexture.filePath_, DDS_FLAGS_NONE, &metadata_, scratchImg_);
	if (FAILED(result)) {
		result = LoadFromWICFile(proTexture.filePath_, WIC_FLAGS_NONE/*WIC_FLAGS_FORCE_RGB*/, &metadata_, scratchImg_);
	}
	if (FAILED(result)) {
		assert(0);
	}
	const DirectX::Image* img = scratchImg_.GetImage(0, 0, 0);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata_.format,
		static_cast<UINT>(metadata_.width),
		static_cast<UINT>(metadata_.height),
		static_cast<UINT16>(metadata_.arraySize),
		static_cast<UINT16>(metadata_.mipLevels));

	// テクスチャバッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&texbuff));


	// ここから追記。

	// Footprint(コピー可能なリソースのレイアウト)を取得。
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT64 totalBytes = 0;
	Engine::Ins()->device_.dev_->GetCopyableFootprints(&texresDesc, 0, 1, 0, &footprint, nullptr, nullptr, &totalBytes);

	// Upload用のバッファを作成する。
	D3D12_RESOURCE_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = totalBytes;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES heap;
	memset(&heap, 0, sizeof(heap));
	heap.Type = D3D12_HEAP_TYPE_UPLOAD;

	ID3D12Resource* iUploadBuffer = nullptr;
	Engine::Ins()->device_.dev_->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&iUploadBuffer));

	// UploadBufferへの書き込み。
	void* ptr = nullptr;
	iUploadBuffer->Map(0, nullptr, &ptr);
	memcpy(reinterpret_cast<unsigned char*>(ptr) + footprint.Offset, img->pixels, img->slicePitch);

	// Copyコマンドの作成
	D3D12_TEXTURE_COPY_LOCATION dest;
	memset(&dest, 0, sizeof(dest));
	dest.pResource = texbuff.Get();
	dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dest.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION  src;
	memset(&src, 0, sizeof(src));
	src.pResource = iUploadBuffer;
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = footprint;

	Engine::Ins()->mainGraphicsCmdList_->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);

	// リソースバリア。
	D3D12_RESOURCE_BARRIER  barrier;
	memset(&barrier, 0, sizeof(barrier));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = texbuff.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// コピーコマンドの実行。
	Engine::Ins()->mainGraphicsCmdList_->Close();
	ID3D12CommandList* commandLists[] = { Engine::Ins()->mainGraphicsCmdList_.Get() };
	Engine::Ins()->graphicsCmdQueue_->ExecuteCommandLists(1, commandLists);
	Engine::Ins()->graphicsCmdQueue_->Signal(Engine::Ins()->asFence_.Get(), ++Engine::Ins()->asfenceValue_);

	// グラフィックコマンドリストの完了待ち
	UINT64 gpuFence = Engine::Ins()->asFence_->GetCompletedValue();
	if (gpuFence != Engine::Ins()->asfenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		Engine::Ins()->asFence_->SetEventOnCompletion(Engine::Ins()->asfenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// コマンドアロケータのリセット
	Engine::Ins()->mainGraphicsCmdAllocator_->Reset();	//キューをクリア

	// コマンドリストのリセット
	Engine::Ins()->mainGraphicsCmdList_->Reset(Engine::Ins()->mainGraphicsCmdAllocator_.Get(), nullptr);//再びコマンドリストを貯める準備


	// ここまで追記。

	// テクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	proTexture.metadata_ = metadata_;
	proTexture.scratchImg_ = &scratchImg_;
	proTexture.texBuff_ = texbuff;
	texture_.emplace_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata_.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT_MAX;
	// ヒープにシェーダーリソースビュー生成
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

int TextureManager::LoadTexture(std::wstring FileName, const void* Src, const UINT64& Size)
{

	// データがnullかどうかをチェック。
	if (Src == nullptr) {

		assert(0);

	}

	// ファイルがロード済みかをチェック
	if (0 < texture_.size()) {

		int counter = 0;
		for (auto& index_ : texture_) {

			// ロードしてあったら識別番号を返す
			if (index_.filePath_ == FileName.data()) {

				return descriptorHeadMgrIndex_[counter];

			}
			++counter;
		}
	}

	// テクスチャデータを保存
	Texture proTexture{};
	proTexture.filePath_ = FileName.data();

	// ロードしていなかったらロードする
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;

	HRESULT hr = E_FAIL;
	hr = LoadFromDDSMemory(Src, Size, DDS_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(hr)) {
		hr = LoadFromWICMemory(Src, Size, WIC_FLAGS_NONE/*WIC_FLAGS_FORCE_RGB*/, &metadata, scratchImg);
	}
	if (FAILED(hr)) {
		assert(0);
	}

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		static_cast<UINT>(metadata.width),
		static_cast<UINT>(metadata.height),
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels));

	// テクスチャバッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	HRESULT result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// データ転送
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);
	result = texbuff->WriteToSubresource(
		0,
		nullptr,							// 全領域コピー
		img->pixels,						// 元データの先頭アドレス
		static_cast<UINT>(img->rowPitch),	// 一ラインのサイズ
		static_cast<UINT>(img->slicePitch)	// いちまいのサイズ
	);

	// テクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	proTexture.metadata_ = metadata;
	proTexture.scratchImg_ = &scratchImg;
	proTexture.texBuff_ = texbuff;
	texture_.emplace_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// ヒープにシェーダーリソースビュー生成
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}


int TextureManager::CreateTexture(DirectX::XMFLOAT4 Color)
{
	// 同じ色のテクスチャがすでに生成済みかをチェックする
	LPCWSTR selfTex = L"SelfTexture";

	int counter = 0;
	for (auto& index_ : texture_) {

		if (index_.filePath_ == selfTex && index_.colorData_.x == Color.x && index_.colorData_.y == Color.y &&
			index_.colorData_.z == Color.z && index_.colorData_.w == Color.w) {

			// すでに生成してあるテクスチャなのでSRVヒープの番号を返す
			return counter;

		}
		++counter;

	}

	// 画像データを作成する
	static const int texWidth = 256;
	const int imageDataCount = texWidth * texWidth;
	// 画像イメージデータ配列
	std::array<DirectX::XMFLOAT4, texWidth> imageData;

	// 全ピクセルの色を初期化
	for (auto& index_ : imageData) {
		index_.x = Color.x;
		index_.y = Color.y;
		index_.z = Color.z;
		index_.w = Color.w;
	}

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		texWidth,
		static_cast<UINT>(texWidth),
		static_cast<UINT16>(1),
		static_cast<UINT16>(1));

	// テクスチャバッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapPropBuff = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	HRESULT result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapPropBuff,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// データ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,									//全領域コピー
		imageData.data(),							//元データの先頭アドレス
		sizeof(DirectX::XMFLOAT4) * texWidth,		//一ラインのサイズ
		sizeof(DirectX::XMFLOAT4) * imageDataCount	//いちまいのサイズ
	);

	// テクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	Texture proTexture{};
	proTexture.filePath_ = L"SelfTexture";
	proTexture.metadata_ = {};
	proTexture.scratchImg_ = {};
	proTexture.texBuff_ = texbuff;
	proTexture.colorData_ = Color;
	texture_.push_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// ヒープにシェーダーリソースビュー生成
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRV(int IDNum) {

	D3D12_GPU_DESCRIPTOR_HANDLE basicHeapHandle = DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	// 消費した分だけアドレスをずらす。
	for (int i = 0; i < IDNum; ++i) {

		basicHeapHandle.ptr += Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	return basicHeapHandle;
}

Texture TextureManager::GetTexture(int ID)
{
	return texture_[ID];
}
