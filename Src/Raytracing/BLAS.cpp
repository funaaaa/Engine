#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "RWStructuredBuffer.h"
#include "StructuredBuffer.h"
#include "TextureManager.h"

void BLAS::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing, const bool& IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// パスを保存。
	texturePath = TexturePath;
	modelPath = DirectryPath + ModelName;

	// テクスチャを読み込む。
	const int TEXTURE_PATH_COUNT = static_cast<int>(TexturePath.size());
	for (auto& index : TexturePath) {

		textureHandle.emplace_back(TextureManager::Ins()->LoadTexture(index));

	}


	/*-- 形状データを読み込む --*/

	// 読み込んだデータを一時保存する用のバッファ。
	ModelDataManager::ObjectData dataBuff;

	// モデルをロード。
	ModelDataManager::Ins()->LoadObj(DirectryPath, ModelName, dataBuff, IsSmoothing);

	// マテリアル情報を保存。
	material = dataBuff.material;

	// マテリアル用定数バッファを生成。
	materialBuffer = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::Material)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したバッファにマテリアルデータを書き込む。
	WriteToMemory(materialBuffer, &material, static_cast<size_t>(sizeof(ModelDataManager::Material)));

	// マテリアルデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor.CreateStructuredSRV(materialBuffer, 1, 0, sizeof(ModelDataManager::Material), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// 頂点数を求める。
	vertexCount = static_cast<UINT>(dataBuff.vertex.size());

	// 頂点インデックス数を求める。
	indexCount = static_cast<UINT>(dataBuff.index.size());

	// 頂点データを変換。
	for (int index = 0; index < static_cast<int>(vertexCount); ++index) {

		RayVertex buff{};
		buff.normal = dataBuff.vertex[index].normal;
		buff.position = dataBuff.vertex[index].pos;
		buff.uv = dataBuff.vertex[index].uv;

		// データを保存。
		vertex.push_back(buff);

	}

	// 頂点インデックスデータを保存。
	vertIndex = dataBuff.index;

	// 頂点サイズを求める。
	vertexStride = sizeof(RayVertex);

	// 頂点インデックスサイズを求める。
	indexStride = sizeof(UINT);

	// 頂点バッファを生成する。
	vertexBuffer = CreateBuffer(
		static_cast<size_t>(vertexStride * vertexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// 頂点インデックスバッファを生成する。
	indexBuffer = CreateBuffer(
		static_cast<size_t>(indexStride * indexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したインデックスバッファに頂点インデックスデータを書き込む。
	WriteToMemory(indexBuffer, vertIndex.data(), static_cast<size_t>(indexStride * indexCount));

	// 頂点インデックスデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// 頂点データでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLASバッファを生成する --*/

	// 形状を設定する用の構造体を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque);
	isOpaque = IsOpaque;

	// BLASバッファを設定、構築する。
	SettingAccelerationStructure(geomDesc);


	// ヒットグループ名を保存する。
	this->hitGroupName = HitGroupName;

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexPos.resize(static_cast<unsigned __int64>(vertex.size()));
	vertexNormal.resize(static_cast<unsigned __int64>(vertex.size()));
	int counter = 0;
	for (auto& index : vertex) {
		vertexPos[counter] = index.position;
		vertexNormal[counter] = index.normal;
		++counter;
	}

}

void BLAS::GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath)
{

	/*===== BLASを生成する処理 =====*/

	// テクスチャを読み込む。
	const int TEXTURE_PATH_COUNT = static_cast<int>(TexturePath.size());
	for (int index = 0; index < TEXTURE_PATH_COUNT; ++index) {

		textureHandle.emplace_back(TextureManager::Ins()->LoadTexture(TexturePath[index]));

	}

	/*-- 形状データを読み込む --*/

	// 読み込んだデータを一時保存する用のバッファ。
	std::vector<FbxLoader::Vertex> modelVertexData;
	std::vector<UINT> modelIndexData;

	// モデルをロード。
	modelIndex = FbxLoader::Ins()->LoadModelFromFile(DirectryPath, ModelName);

	FbxLoader::Ins()->GetFbxData(modelIndex, modelVertexData, modelIndexData);

	// マテリアル用定数バッファを生成。
	materialBuffer = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::Material)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したバッファにマテリアルデータを書き込む。
	WriteToMemory(materialBuffer, &material, static_cast<size_t>(sizeof(ModelDataManager::Material)));

	// マテリアルデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor.CreateStructuredSRV(materialBuffer, 1, 0, sizeof(ModelDataManager::Material), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// 頂点数を求める。
	vertexCount = static_cast<UINT>(modelVertexData.size());

	// 頂点インデックス数を求める。
	indexCount = static_cast<UINT>(modelIndexData.size());

	// 頂点データを変換。
	for (int index = 0; index < static_cast<int>(vertexCount); ++index) {

		RayVertex buff{};
		buff.normal = modelVertexData[index].normal;
		buff.position = modelVertexData[index].pos;
		buff.uv = modelVertexData[index].uv;

		// データを保存。
		vertex.push_back(buff);

	}

	// 頂点インデックスデータを保存。
	vertIndex = modelIndexData;

	// 頂点サイズを求める。
	vertexStride = sizeof(RayVertex);

	// 頂点インデックスサイズを求める。
	indexStride = sizeof(UINT);

	// 頂点バッファを生成する。
	vertexBuffer = CreateBuffer(
		static_cast<size_t>(vertexStride * vertexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// 頂点インデックスバッファを生成する。
	indexBuffer = CreateBuffer(
		static_cast<size_t>(indexStride * indexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したインデックスバッファに頂点インデックスデータを書き込む。
	WriteToMemory(indexBuffer, vertIndex.data(), static_cast<size_t>(indexStride * indexCount));

	// 頂点インデックスデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// 頂点データでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLASバッファを生成する --*/

	// 形状を設定する用の構造体を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(true);
	isOpaque = true;

	// BLASバッファを設定、構築する。
	SettingAccelerationStructure(geomDesc);

	// ヒットグループ名を保存する。
	this->hitGroupName = HitGroupName;

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		// 入力用構造体をリサイズ。
		skinComputeInput.resize(vertex.size());

		// スキニングアニメーションコンピュートシェーダーで使用する入力用構造体をセット。
		FbxLoader::Ins()->GetSkinComputeInput(modelIndex, skinComputeInput);

		// スキニングアニメーションで使用するコンピュートシェーダーをロードしておく。
		skinComput.Init(L"resource/ShaderFiles/RayTracing/ComputeSkin.hlsl", sizeof(FbxLoader::SkinComputeInput), static_cast<int>(skinComputeInput.size()), skinComputeInput.data(), sizeof(RayVertex), static_cast<int>(vertex.size()), vertex.data());

	}

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexPos.resize(static_cast<unsigned __int64>(vertex.size()));
	vertexNormal.resize(static_cast<unsigned __int64>(vertex.size()));
	int counter = 0;
	for (auto& index : vertex) {
		vertexPos[counter] = index.position;
		vertexNormal[counter] = index.normal;
		++counter;
	}

}

void BLAS::GenerateBLASData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// テクスチャを読み込む。
	textureHandle = TextureHandle;

	/*-- 形状データを読み込む --*/

	// マテリアル用定数バッファを生成。
	materialBuffer = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::Material)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したバッファにマテリアルデータを書き込む。
	WriteToMemory(materialBuffer, &material, static_cast<size_t>(sizeof(ModelDataManager::Material)));

	// マテリアルデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor.CreateStructuredSRV(materialBuffer, 1, 0, sizeof(ModelDataManager::Material), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// 頂点数を求める。
	vertexCount = static_cast<UINT>(Data.vertex.size());

	// 頂点インデックス数を求める。
	indexCount = static_cast<UINT>(Data.index.size());

	// 頂点データを変換。
	for (int index = 0; index < static_cast<int>(vertexCount); ++index) {

		RayVertex buff{};
		buff.normal = Data.vertex[index].normal;
		buff.position = Data.vertex[index].pos;
		buff.uv = Data.vertex[index].uv;

		// データを保存。
		vertex.push_back(buff);

	}

	// 頂点インデックスデータを保存。
	vertIndex = Data.index;

	// 頂点サイズを求める。
	vertexStride = sizeof(RayVertex);

	// 頂点インデックスサイズを求める。
	indexStride = sizeof(UINT);

	// 頂点バッファを生成する。
	vertexBuffer = CreateBuffer(
		static_cast<size_t>(vertexStride * vertexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// 頂点インデックスバッファを生成する。
	indexBuffer = CreateBuffer(
		static_cast<size_t>(indexStride * indexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したインデックスバッファに頂点インデックスデータを書き込む。
	WriteToMemory(indexBuffer, vertIndex.data(), static_cast<size_t>(indexStride * indexCount));

	// 頂点インデックスデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// 頂点データでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLASバッファを生成する --*/

	// 形状を設定する用の構造体を設定。
	isOpaque = IsOpaque;
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(isOpaque);

	// BLASバッファを設定、構築する。
	SettingAccelerationStructure(geomDesc);


	// ヒットグループ名を保存する。
	this->hitGroupName = HitGroupName;

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexPos.resize(static_cast<unsigned __int64>(vertex.size()));
	vertexNormal.resize(static_cast<unsigned __int64>(vertex.size()));
	int counter = 0;
	for (auto& index : vertex) {
		vertexPos[counter] = index.position;
		vertexNormal[counter] = index.normal;
		++counter;
	}

}

void BLAS::Update()
{

	/*===== BLASの更新 =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		// アニメーションの更新処理
		auto& model = FbxLoader::Ins()->GetFbxModel(modelIndex);
		if (model.isPlay) {

			model.currentTime += model.frameTime;

			// 最後まで再生したら先頭に戻す。
			if (model.endTime < model.currentTime) {

				model.currentTime = model.startTime;

			}

			// スキニングアニメーションコンピュートシェーダーで使用する入力用構造体をセット。
			FbxLoader::Ins()->GetSkinComputeInput(modelIndex, skinComputeInput);

		}

	};

	// 頂点を書き込む。 今のところは頂点しか書き換える予定はないが、後々他のやつも書き込む。ダーティフラグみたいなのを用意したい。
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// 更新のための値を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(isOpaque);
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &geomDesc;
	// BLAS の更新処理を行うためのフラグを設定する。
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;


	// インプレース更新を実行する。
	asDesc.SourceAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	// 更新用の作業バッファを設定する。
	asDesc.ScratchAccelerationStructureData = updateBuffer->GetGPUVirtualAddress();

	// コマンドリストに積む。
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure();

}

void BLAS::ComputeSkin()
{

	/*===== 頂点データをスキニング行列を元に書き換える処理 =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		// 入力構造体を更新。
		skinComput.UpdateInputSB(skinComputeInput.data());

		// 計算開始。
		skinComput.Dispatch(1, 1, 1);

		// 結果を代入。
		memcpy(vertex.data(), skinComput.outputSB->buffersOnCPU, sizeof(RayVertex) * vertex.size());

	}

}

void BLAS::InitAnimation()
{
	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Ins()->GetFbxModel(modelIndex).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== アニメーションさせる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Ins()->GetFbxModel(modelIndex).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== アニメーションを停止させる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Ins()->GetFbxModel(modelIndex).StopAnimation();

	}

}
#include "HitGroupMgr.h"
#include <assert.h>
uint8_t* BLAS::WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== シェーダーレコードを書き込む =====*/

	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps;
	StateObject.As(&rtsoProps);
	auto entryBegin = Dst;
	auto shader = GetHitGroupName();

	// 保存されているヒットグループ名と違っていたら書き込まない。
	//if (HitGroupName == shader) {

	auto id = rtsoProps->GetShaderIdentifier(shader.c_str());
	if (id == nullptr) {
		throw std::logic_error("Not found ShaderIdentifier");
	}

	// シェーダー識別子を書き込む。
	memcpy(Dst, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	// 今回のプログラムでは以下の順序でディスクリプタを記録。
	// [0] : インデックスバッファ
	// [1] : 頂点バッファ
	// ※ ローカルルートシグネチャの順序に合わせる必要がある。
	Dst += WriteGPUDescriptor(Dst, &indexDescriptor.GetGPUHandle());
	Dst += WriteGPUDescriptor(Dst, &vertexDescriptor.GetGPUHandle());

	// マテリアル用のバッファをセット。
	Dst += WriteGPUDescriptor(Dst, &materialDescriptor.GetGPUHandle());

	// ヒットグループ名からヒットグループ名IDを取得する。
	int hitGroupID = HitGroupMgr::Ins()->GetHitGroupID(HitGroupName);

	// 頂点、インデックス、マテリアルのオフセット
	const int OFFSET_VERTEX_INDEX_MATERIAL = 3;

	// ヒットグループIDからSRVの数を取得。
	int srvCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID) + HitGroupMgr::Ins()->GetHitGroupSRVCount(hitGroupID) - OFFSET_VERTEX_INDEX_MATERIAL;

	// ここはテクスチャのサイズではなく、パイプラインにセットされたSRVの数を持ってきてそれを使う。
	// この時点でSRVの数とテクスチャの数が合っていなかったらassertを出す。
	for (int index = 0; index < srvCount; ++index) {

		// このインデックスのテクスチャが存在していなかったら
		if (textureHandle.size() <= index) {

			// メモリ上にズレが生じてしまうので先頭のテクスチャを書き込む。
			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[0]);
			WriteGPUDescriptor(Dst, &texDescHandle);

		}
		else {

			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[index]);
			Dst += WriteGPUDescriptor(Dst, &texDescHandle);

		}
	}

	Dst = entryBegin + recordSize;
	return Dst;

	//}


	// ヒットグループが複数あったときの処理。今は一つしか無いので消す。
	//else {

	//	// シェーダー識別子を書き込む。
	//	auto idBuff = rtsoProps->GetShaderIdentifier(HitGroupName);
	//	memcpy(Dst, idBuff, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	//	Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	//	// 今回のプログラムでは以下の順序でディスクリプタを記録。
	//	// [0] : インデックスバッファ
	//	// [1] : 頂点バッファ
	//	// ※ ローカルルートシグネチャの順序に合わせる必要がある。
	//	Dst += WriteGPUDescriptor(Dst, &indexDescriptor.GetGPUHandle());
	//	Dst += WriteGPUDescriptor(Dst, &vertexDescriptor.GetGPUHandle());

	//	// マテリアル用のバッファをセット。
	//	Dst += WriteGPUDescriptor(Dst, &materialDescriptor.GetGPUHandle());

	//	// ヒットグループ名からヒットグループ名IDを取得する。
	//	int hitGroupID = HitGroupMgr::Ins()->GetHitGroupID(HitGroupName);

	//	// 頂点、インデックス、マテリアルのオフセット
	//	const int OFFSET_VERTEX_INDEX_MATERIAL = 3;

	//	// ヒットグループIDからSRVの数を取得。
	//	int srvCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID) + HitGroupMgr::Ins()->GetHitGroupSRVCount(hitGroupID) - OFFSET_VERTEX_INDEX_MATERIAL;

	//	// ここはテクスチャのサイズではなく、パイプラインにセットされたSRVの数を持ってきてそれを使う。
	//	// この時点でSRVの数とテクスチャの数が合っていなかったらassertを出す。
	//	for (int index = 0; index < srvCount; ++index) {

	//		// このインデックスのテクスチャが存在していなかったら
	//		if (textureHandle.size() <= index) {

	//			// メモリ上にズレが生じてしまうので先頭のテクスチャを書き込む。
	//			CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[0]);
	//			WriteGPUDescriptor(Dst, &texHandle);

	//		}
	//		else {

	//			CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[index]);
	//			Dst += WriteGPUDescriptor(Dst, &texHandle);

	//		}
	//	}

	//	Dst = entryBegin + recordSize;
	//	return Dst;

	//}

}

void BLAS::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* pData, size_t DataSize)
{

	/*===== メモリに値を書き込む処理 =====*/

	// nullチェック。
	if (Resource == nullptr) return;

	// マップ処理を行う。
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, DataSize };
	HRESULT hr = Resource->Map(0, &range, (void**)&mapped);

	// マップが成功したら値を書き込む。
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, DataSize);
		Resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> BLAS::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== バッファ全般を生成する処理 =====*/

	// 引数から設定用構造体を設定する。
	D3D12_HEAP_PROPERTIES heapProps{};
	if (HeapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}

	// 実際にバッファを生成する。
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = Size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = Flags;

	// バッファ生成命令を出す。
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		InitialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// 生成に失敗したら。
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}

D3D12_RAYTRACING_GEOMETRY_DESC BLAS::GetGeometryDesc(const bool& IsOpaque)
{

	/*===== BLAS生成時に設定を取得する用関数 =====*/

	// 形状データのフラグを設定。
	auto geometryDesc = D3D12_RAYTRACING_GEOMETRY_DESC{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	// 不透明フラグ。ここで不透明にするとシェーダー側でAnyHitShaderをOnにしていても呼ばれなくなるらしい！
	if (IsOpaque) {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	}
	else {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
	}

	// 形状データの細かい項目を設定。
	auto& triangles = geometryDesc.Triangles;
	triangles.VertexBuffer.StartAddress = vertexBuffer->GetGPUVirtualAddress();
	triangles.VertexBuffer.StrideInBytes = vertexStride;
	triangles.VertexCount = vertexCount;
	triangles.IndexBuffer = indexBuffer->GetGPUVirtualAddress();
	triangles.IndexCount = indexCount;
	triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

	return geometryDesc;

}

void BLAS::SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& GeomDesc)
{

	/*===== 加速構造体の設定用関数 =====*/

	// AS(レイとの判定を迅速に行うために必要なポリゴンデータ構造体)の設定に必要な各項目を設定。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	// D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &GeomDesc;

	// 関数を使って必要なメモリ量を求める.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blasPrebuild{};
	DirectXBase::Ins()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);

	// スクラッチバッファを生成する。
	scratchBuffer = CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// BLASのバッファを生成する。
	blasBuffer = CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// 更新用バッファを生成する。
	updateBuffer = CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// AccelerationStructureの構築。
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	// コマンドリストに積んで実行する。
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// BLASを構築する。
	CreateAccelerationStructure();

}

void BLAS::CreateAccelerationStructure()
{

	/*===== BLAS構築処理 =====*/

	// リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = blasBuffer.Get();
	DirectXBase::Ins()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Ins()->cmdList->Close();

	// BLASを構築。
	//ID3D12CommandList* pCmdList[] = { DirectXBase::Ins()->cmdList.Get() };
	// 構築用関数を呼ぶ。
	ID3D12CommandList* commandLists[] = { DirectXBase::Ins()->cmdList.Get() };
	DirectXBase::Ins()->cmdQueue->ExecuteCommandLists(1, commandLists);

	// グラフィックコマンドリストの完了待ち
	DirectXBase::Ins()->cmdQueue->Signal(DirectXBase::Ins()->fence.Get(), ++DirectXBase::Ins()->fenceVal);
	if (DirectXBase::Ins()->fence->GetCompletedValue() != DirectXBase::Ins()->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		DirectXBase::Ins()->fence->SetEventOnCompletion(DirectXBase::Ins()->fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// コマンドアロケータのリセット
	DirectXBase::Ins()->cmdAllocator->Reset();	//キューをクリア

	// コマンドリストのリセット
	DirectXBase::Ins()->cmdList->Reset(DirectXBase::Ins()->cmdAllocator.Get(), nullptr);//再びコマンドリストを貯める準備

}