#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "TextureManager.h"
#include "FHelper.h"
#include "HitGroupMgr.h"


void BLAS::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// パスを保存。
	modelPath_ = DirectryPath + ModelName;

	// BlasのIndexを保存。
	blasIndex_ = BlasIndex;


	/*-- 形状データを読み込む --*/

	// 読み込んだデータを一時保存する用のバッファ。
	ModelDataManager::ObjectData dataBuff;

	// モデルをロード。
	ModelDataManager::Ins()->LoadObj(DirectryPath, ModelName, dataBuff, false);

	// 各成分の値を保存。
	vertexMax_ = dataBuff.vertexMax_;
	vertexMin_ = dataBuff.vertexMin_;

	// マテリアル情報を保存。
	material_.baseColor_ = dataBuff.material_.baseColor_;
	material_.mapParam_ = dataBuff.material_.mapParam_;
	material_.roughness_ = dataBuff.material_.roughness_;
	material_.metalness_ = dataBuff.material_.metalness_;
	material_.specular_ = dataBuff.material_.specular_;

	// テクスチャを保存。
	baseTextureHandle_ = dataBuff.textureHandle_;

	// 現在のQueueのIndex
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// マテリアルバッファを生成する。
	CreateMaterialBuffer();

	// 頂点バッファとインデックスバッファを生成。
	CreateVertexBuffer(dataBuff);


	/*-- BLASバッファを生成する --*/

	for (int index = 0; index < 2; ++index) {

		// 形状を設定する用の構造体を設定。
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		isOpaque_ = IsOpaque;

		// BLASバッファを設定、構築する。
		SettingAccelerationStructure(geomDesc, index);

	}


	// ヒットグループ名を保存する。
	this->hitGroupName_ = HitGroupName;

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexPos_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexNormal_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexUV_.resize(static_cast<unsigned __int64>(vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertex_) {
		vertexPos_[counter] = index_.position_;
		vertexNormal_[counter] = index_.normal_;
		vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// 頂点を保存。
	defVertex_ = vertex_;

	// ダーティフラグ
	isChangeVertex_[0] = true;
	isChangeVertex_[1] = true;
	isChangeTexture_[0] = true;
	isChangeTexture_[1] = true;

	isGenerate_ = true;

}

#include "FString.h"
void BLAS::GenerateBLASGLTF(const std::wstring& Path, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// パスを保存。
	modelPath_ = FString::WStringToString(Path);

	// BlasのIndexを保存。
	blasIndex_ = BlasIndex;


	/*-- 形状データを読み込む --*/

	// 読み込んだデータを一時保存する用のバッファ。
	ModelDataManager::ObjectData dataBuff;

	// モデルをロード。
	ModelDataManager::Ins()->LoadGLTF(Path, dataBuff);

	// 各成分の値を保存。
	vertexMax_ = dataBuff.vertexMax_;
	vertexMin_ = dataBuff.vertexMin_;

	// マテリアル情報を保存。
	material_.baseColor_ = dataBuff.material_.baseColor_;
	material_.mapParam_ = dataBuff.material_.mapParam_;
	material_.roughness_ = dataBuff.material_.roughness_;
	material_.metalness_ = dataBuff.material_.metalness_;
	material_.specular_ = dataBuff.material_.specular_;

	// テクスチャを保存。
	baseTextureHandle_ = dataBuff.textureHandle_;

	// マテリアルバッファを生成する。
	CreateMaterialBuffer();

	// 頂点数を求める。
	vertexCount_ = static_cast<UINT>(dataBuff.vertex_.size());

	// 頂点インデックス数を求める。
	indexCount_ = static_cast<UINT>(dataBuff.index_.size());

	// 頂点データを変換。
	for (int index = 0; index < static_cast<int>(vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = dataBuff.vertex_[index].normal_;
		buff.position_ = dataBuff.vertex_[index].pos_;
		buff.uv_ = dataBuff.vertex_[index].uv_;

		// データを保存。
		vertex_.push_back(buff);

	}

	// 頂点バッファを生成。
	CreateVertexBuffer(dataBuff);


	/*-- BLASバッファを生成する --*/

	for (int index = 0; index < 2; ++index) {

		// 形状を設定する用の構造体を設定。
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		isOpaque_ = IsOpaque;

		// BLASバッファを設定、構築する。
		SettingAccelerationStructure(geomDesc, index);

	}


	// ヒットグループ名を保存する。
	this->hitGroupName_ = HitGroupName;

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexPos_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexNormal_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexUV_.resize(static_cast<unsigned __int64>(vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertex_) {
		vertexPos_[counter] = index_.position_;
		vertexNormal_[counter] = index_.normal_;
		vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// 頂点を保存。
	defVertex_ = vertex_;

	// ダーティフラグ
	isChangeVertex_[0] = true;
	isChangeVertex_[1] = true;
	isChangeTexture_[0] = true;
	isChangeTexture_[1] = true;

	isGenerate_ = true;

}

void BLAS::GenerateBLASData(const ModelDataManager::ObjectData& ModelData, int BlasIndex, bool IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// パスを保存。
	modelPath_ = "SelfModel";

	// BlasのIndexを保存。
	blasIndex_ = BlasIndex;


	/*-- 形状データを読み込む --*/

	// 各成分の値を保存。
	vertexMax_ = ModelData.vertexMax_;
	vertexMin_ = ModelData.vertexMin_;

	// マテリアル情報を保存。
	material_.baseColor_ = ModelData.material_.baseColor_;
	material_.mapParam_ = ModelData.material_.mapParam_;
	material_.roughness_ = ModelData.material_.roughness_;
	material_.metalness_ = ModelData.material_.metalness_;
	material_.specular_ = ModelData.material_.specular_;

	// テクスチャを保存。
	baseTextureHandle_ = ModelData.textureHandle_;

	// マテリアルバッファを生成する。
	CreateMaterialBuffer();

	// 頂点バッファを生成。
	CreateVertexBuffer(ModelData);


	/*-- BLASバッファを生成する --*/

	for (int index = 0; index < 2; ++index) {

		// 形状を設定する用の構造体を設定。
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		isOpaque_ = IsOpaque;

		// BLASバッファを設定、構築する。
		SettingAccelerationStructure(geomDesc, index);

	}


	// ヒットグループ名を保存する。
	this->hitGroupName_ = HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::Ins()->DEF];

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexPos_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexNormal_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexUV_.resize(static_cast<unsigned __int64>(vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertex_) {
		vertexPos_[counter] = index_.position_;
		vertexNormal_[counter] = index_.normal_;
		vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// 頂点を保存。
	defVertex_ = vertex_;

	// ダーティフラグ
	isChangeVertex_[0] = true;
	isChangeVertex_[1] = true;
	isChangeTexture_[0] = true;
	isChangeTexture_[1] = true;

	isGenerate_ = true;

}

void BLAS::Update()
{

	/*===== BLASの更新 =====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// 生成していなかったら処理を飛ばす。
	if (!isGenerate_) return;

	// 頂点を書き込む。 今のところは頂点しか書き換える予定はないが、後々他のやつも書き込む。ダーティフラグみたいなのを用意したい。
	WriteToMemory(vertexMapAddress_[currentQueueIndex], vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_[currentQueueIndex].Get(), vertexUploadBuffer_[currentQueueIndex].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// 更新のための値を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(isOpaque_, currentQueueIndex);
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
	asDesc.SourceAccelerationStructureData = blasBuffer_[currentQueueIndex]->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = blasBuffer_[currentQueueIndex]->GetGPUVirtualAddress();
	// 更新用の作業バッファを設定する。
	asDesc.ScratchAccelerationStructureData = updateBuffer_[currentQueueIndex]->GetGPUVirtualAddress();

	// コマンドリストに積む。
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure(currentQueueIndex);

}

void BLAS::InitAnimation()
{
	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelIndex_).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== アニメーションさせる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelIndex_).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== アニメーションを停止させる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelIndex_).StopAnimation();

	}

}
void BLAS::IsChangeMaterial()
{

	/*===== マテリアルを書き換えた際の処理 =====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// 確保したバッファにマテリアルデータを書き込む。
	WriteToMemory(materialMapAddress_[currentQueueIndex], &material_, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)));

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(materialBuffer_[currentQueueIndex].Get(), materialUploadBuffer_[currentQueueIndex].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	isChangeVertex_[0] = true;
	isChangeVertex_[1] = true;

}
void BLAS::ChangeBaseTexture(int Index)
{
	baseTextureHandle_ = Index;
	isChangeTexture_[0] = true;
	isChangeTexture_[1] = true;
}
void BLAS::ChangeMapTexture(int Index, MAP_PARAM MapParam)
{
	mapTextureHandle_ = Index;
	material_.mapParam_ = static_cast<int>(MapParam);
	isChangeTexture_[0] = true;
	isChangeTexture_[1] = true;
	IsChangeMaterial();
}
#include "HitGroupMgr.h"
#include <assert.h>
BLAS::BLAS()
{

	isGenerate_ = false;
	baseTextureHandle_ = -1;
	mapTextureHandle_ = -1;
	blasIndex_ = -1;
	indexCount_ = 0;
	indexDescriptor_ = {};
	indexStride_ = {};
	isChangeVertex_[0] = true;
	isChangeVertex_[1] = true;
	isChangeTexture_[0] = true;
	isChangeTexture_[1] = true;
	isOpaque_ = false;
	materialDescriptor_ = {};
	modelIndex_ = 0;
	vertexCount_ = 0;
	vertexDescriptor_ = {};
	vertexStride_ = 0;
}

uint8_t* BLAS::WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName, int Index)
{

	/*===== シェーダーレコードを書き込む =====*/

	// 生成されていなかったら飛ばす。
	if (!isGenerate_) {


		Dst = Dst + recordSize;
		return Dst;

	}

	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps_;
	StateObject.As(&rtsoProps_);
	auto entryBegin = Dst;
	auto shader = GetHitGroupName();

	// 保存されているヒットグループ名と違っていたら書き込まない。
	//if (HitGroupName == shader) {

	auto mode_ = rtsoProps_->GetShaderIdentifier(shader.c_str());
	if (mode_ == nullptr) {
		throw std::logic_error("Not found ShaderIdentifier");
	}

	// 頂点関係のデータが変更されていたら。
	if (isChangeVertex_[Index]) {

		// シェーダー識別子を書き込む。
		memcpy(Dst, mode_, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		// 今回のプログラムでは以下の順序でディスクリプタを記録。
		// [0] : インデックスバッファ
		// [1] : 頂点バッファ
		// ※ ローカルルートシグネチャの順序に合わせる必要がある。
		Dst += WriteGPUDescriptor(Dst, &indexDescriptor_[Index].GetGPUHandle());
		Dst += WriteGPUDescriptor(Dst, &vertexDescriptor_[Index].GetGPUHandle());

		// マテリアル用のバッファをセット。
		Dst += WriteGPUDescriptor(Dst, &materialDescriptor_[Index].GetGPUHandle());

		isChangeVertex_[Index] = false;

	}
	else {

		Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)));
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)));
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)));

	}

	// ヒットグループ名からヒットグループ名IDを取得する。
	int hitGroupID = HitGroupMgr::Ins()->GetHitGroupID(HitGroupName);

	// 頂点、インデックス、マテリアルのオフセット
	const int OFFSET_VERTEX_INDEX_MATERIAL = 3;

	// ヒットグループIDからSRVの数を取得。
	int srvCount = HitGroupMgr::Ins()->GetHitGroupSRVCount(hitGroupID) - OFFSET_VERTEX_INDEX_MATERIAL;

	// テクスチャ関係が変更されていたら。
	if (isChangeTexture_[Index]) {

		// ここはテクスチャのサイズではなく、パイプラインにセットされたSRVの数を持ってきてそれを使う。
		// この時点でSRVの数とテクスチャの数が合っていなかったらassertを出す。
		for (int index = 0; index < srvCount; ++index) {

			// 0番目は基本テクスチャ。
			if (index == 0) {

				// 基本のテクスチャが設定されていない。
				if (baseTextureHandle_ == -1) assert(0);

				CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(baseTextureHandle_);
				Dst += WriteGPUDescriptor(Dst, &texDescHandle);

			}
			// 1番目は法線マップテクスチャ。
			else if (index == 1) {

				if (mapTextureHandle_ != -1) {

					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(mapTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}
				else {

					// 法線マップが設定されていなかったら、メモリの隙間を埋めるため通常のテクスチャを書き込む。
					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(baseTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}

			}

		}

		isChangeTexture_[Index] = false;

	}
	else {

		Dst += static_cast<UINT>(sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)) * static_cast<UINT>(srvCount);

	}

	// 使用するUAVの数を取得。
	int uavCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID);
	for (int index = 0; index < uavCount; ++index) {

		// テクスチャが存在していたら。
		if (0 < uavHandle_.size()) {

			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(uavHandle_[index]);
			Dst += WriteGPUDescriptor(Dst, &texDescHandle);

		}
		else {

			Dst += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*);

		}

	}

	Dst = entryBegin + recordSize;
	return Dst;

}

void BLAS::Init()
{

	/*===== 初期化処理 =====*/

	texturePath_.clear();
	defVertex_.clear();
	vertex_.clear();
	vertexPos_.clear();
	vertexNormal_.clear();
	vertexUV_.clear();
	vertIndex_.clear();
	isGenerate_ = false;
	baseTextureHandle_ = -1;
	mapTextureHandle_ = -1;

}

Vec3 BLAS::GetVertexLengthMax()
{

	/*===== 各成分の最大の長さを返す =====*/

	Vec3 vertexLength;

	if (fabs(vertexMin_.x_) < fabs(vertexMax_.x_)) {
		vertexLength.x_ = fabs(vertexMax_.x_);
	}
	else {
		vertexLength.x_ = fabs(vertexMin_.x_);
	}

	if (fabs(vertexMin_.y_) < fabs(vertexMax_.y_)) {
		vertexLength.y_ = fabs(vertexMax_.y_);
	}
	else {
		vertexLength.y_ = fabs(vertexMin_.y_);
	}

	if (fabs(vertexMin_.z_) < fabs(vertexMax_.z_)) {
		vertexLength.z_ = fabs(vertexMax_.z_);
	}
	else {
		vertexLength.z_ = fabs(vertexMin_.z_);
	}

	return vertexLength;
}

void BLAS::MulVec3Vertex(Vec3 Vec)
{

	/*===== すべての頂点に行列情報をかける ====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	for (auto& index_ : vertex_) {

		index_.position_ = defVertex_[&index_ - &vertex_[0]].position_ * Vec;

	}

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexMapAddress_[currentQueueIndex], vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

}

void BLAS::AssignUV(const std::vector<RayVertex>& UV)
{

	/*===== 指定のUVを代入する =====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = 0;

	for (auto& index : vertex_) {

		index.subUV_ = UV[static_cast<int>(&index - &vertex_[0])].uv_;

	}

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexMapAddress_[currentQueueIndex], vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_[currentQueueIndex].Get(), vertexUploadBuffer_[currentQueueIndex].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// 現在のQueueのインデックス
	currentQueueIndex = 1;

	for (auto& index : vertex_) {

		index.subUV_ = UV[static_cast<int>(&index - &vertex_[0])].uv_;

	}

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexMapAddress_[currentQueueIndex], vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_[currentQueueIndex].Get(), vertexUploadBuffer_[currentQueueIndex].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[currentQueueIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// 頂点を書き換えたフラグを立てる。
	isChangeVertex_[0] = true;
	isChangeVertex_[1] = true;

}

void BLAS::ChangeVertexPosition(int Index, const Vec3& Pos)
{

	/*===== 頂点情報の座標成分のみを変更 =====*/

	vertex_[Index].position_ = Pos;
	isChangeVertex_[0] = true;
	isChangeVertex_[1] = true;

}

void BLAS::WriteToMemory(void* MapAddress, const void* PData, size_t DataSize)
{

	/*===== メモリに値を書き込む処理 =====*/

	// マップ処理を行う。
	memcpy(MapAddress, PData, DataSize);

}

D3D12_RAYTRACING_GEOMETRY_DESC BLAS::GetGeometryDesc(bool IsOpaque, int Index)
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
	triangles.VertexBuffer.StartAddress = vertexBuffer_[Index]->GetGPUVirtualAddress();
	triangles.VertexBuffer.StrideInBytes = vertexStride_;
	triangles.VertexCount = vertexCount_;
	triangles.IndexBuffer = indexBuffer_[Index]->GetGPUVirtualAddress();
	triangles.IndexCount = indexCount_;
	triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

	return geometryDesc;

}

void BLAS::SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& GeomDesc, int Index)
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
	Engine::Ins()->device_.dev_->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);

	// スクラッチバッファを生成する。
	scratchBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	scratchBuffer_[Index]->SetName(L"BLASScratchBuffer");

	// BLASのバッファを生成する。
	blasBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	blasBuffer_[Index]->SetName(L"BLASBuffer");

	// 更新用バッファを生成する。
	updateBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	updateBuffer_[Index]->SetName(L"BLASUpdateBuffer");

	// AccelerationStructureの構築。
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer_[Index]->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasBuffer_[Index]->GetGPUVirtualAddress();
	// コマンドリストに積んで実行する。
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// BLASを構築する。
	CreateAccelerationStructure(Index);

}

void BLAS::CreateAccelerationStructure(int Index)
{

	/*===== BLAS構築処理 =====*/

	// リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = blasBuffer_[Index].Get();
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &uavBarrier);
	Engine::Ins()->mainGraphicsCmdList_->Close();

	// BLASを構築。
	//ID3D12CommandList* pCmdList[] = { Engine::Ins()->cmdList.Get() };
	// 構築用関数を呼ぶ。
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

}

void BLAS::CreateMaterialBuffer()
{

	/*===== マテリアルを設定 =====*/

	// マテリアルバッファを生成する。
	materialBuffer_[0] = FHelper::CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	materialBuffer_[0]->SetName(L"MaterialBuffer0");
	materialBuffer_[1] = FHelper::CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	materialBuffer_[1]->SetName(L"MaterialBuffer1");

	// マテリアルバッファMap用サイズ
	D3D12_RANGE range{ 0, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)) };

	// アップロード用マテリアルバッファを生成する。
	materialUploadBuffer_[0] = FHelper::CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	materialUploadBuffer_[0]->SetName(L"MaterialUploadBuffer0");
	materialUploadBuffer_[0]->Map(0, &range, (void**)&materialMapAddress_[0]);
	materialUploadBuffer_[1] = FHelper::CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	materialUploadBuffer_[1]->SetName(L"MaterialUploadBuffer1");
	materialUploadBuffer_[1]->Map(0, &range, (void**)&materialMapAddress_[1]);

	// 確保したバッファにマテリアルデータを書き込む。
	WriteToMemory(materialMapAddress_[0], &material_, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)));
	WriteToMemory(materialMapAddress_[1], &material_, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(materialBuffer_[0].Get(), materialUploadBuffer_[0].Get());
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_[0].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(materialBuffer_[1].Get(), materialUploadBuffer_[1].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_[1].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// マテリアルデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor_[0].CreateStructuredSRV(materialBuffer_[0], 1, 0, sizeof(ModelDataManager::GPUMaterial), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor_[0].CreateStructuredSRV(materialBuffer_[1], 1, 0, sizeof(ModelDataManager::GPUMaterial), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor_[1].CreateStructuredSRV(materialBuffer_[0], 1, 0, sizeof(ModelDataManager::GPUMaterial), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor_[1].CreateStructuredSRV(materialBuffer_[1], 1, 0, sizeof(ModelDataManager::GPUMaterial), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

}

void BLAS::CreateVertexBuffer(const ModelDataManager::ObjectData& DataBuff)
{

	/*===== 頂点とインデックスバッファを生成 =====*/

	// 頂点数を求める。
	vertexCount_ = static_cast<UINT>(DataBuff.vertex_.size());

	// 頂点インデックス数を求める。
	indexCount_ = static_cast<UINT>(DataBuff.index_.size());

	// 頂点データを変換。
	for (int index = 0; index < static_cast<int>(vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = DataBuff.vertex_[index].normal_;
		buff.position_ = DataBuff.vertex_[index].pos_;
		buff.uv_ = DataBuff.vertex_[index].uv_;
		buff.subUV_ = DataBuff.vertex_[index].uv_;

		// データを保存。
		vertex_.push_back(buff);

	}

	// 頂点インデックスデータを保存。
	vertIndex_ = DataBuff.index_;

	// 頂点サイズを求める。
	vertexStride_ = sizeof(RayVertex);

	// 頂点インデックスサイズを求める。
	indexStride_ = sizeof(UINT);

	// 頂点バッファMap用サイズ
	D3D12_RANGE range{ 0, static_cast<size_t>(vertexStride_ * vertexCount_) };

	// 頂点バッファを生成する。
	vertexBuffer_[0] = FHelper::CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	vertexBuffer_[0]->SetName(L"VertexBuffer0");
	vertexBuffer_[1] = FHelper::CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	vertexBuffer_[1]->SetName(L"VertexBuffer1");

	// アップロード用頂点バッファを生成する。
	vertexUploadBuffer_[0] = FHelper::CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	vertexUploadBuffer_[0]->SetName(L"VertexUploadBuffer0");
	vertexUploadBuffer_[0]->Map(0, &range, (void**)&vertexMapAddress_[0]);
	vertexUploadBuffer_[1] = FHelper::CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	vertexUploadBuffer_[1]->SetName(L"VertexUploadBuffer1");
	vertexUploadBuffer_[1]->Map(0, &range, (void**)&vertexMapAddress_[1]);

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexMapAddress_[0], vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));
	WriteToMemory(vertexMapAddress_[1], vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_[0].Get(), vertexUploadBuffer_[0].Get());
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[0].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_[1].Get(), vertexUploadBuffer_[1].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_[1].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// 頂点インデックスバッファMap用サイズ
	range = D3D12_RANGE{ 0, static_cast<size_t>(indexStride_ * indexCount_) };

	// 頂点インデックスバッファを生成する。
	indexBuffer_[0] = FHelper::CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	indexBuffer_[0]->SetName(L"IndexBuffer0");
	indexBuffer_[1] = FHelper::CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	indexBuffer_[1]->SetName(L"IndexBuffer1");

	// アップロード用頂点インデックスバッファを生成する。
	indexUploadBuffer_[0] = FHelper::CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	indexUploadBuffer_[0]->SetName(L"IndexUploadBuffer0");
	indexUploadBuffer_[0]->Map(0, &range, (void**)&indexMapAddress_[0]);
	indexUploadBuffer_[1] = FHelper::CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	indexUploadBuffer_[1]->SetName(L"IndexUploadBuffer1");
	indexUploadBuffer_[1]->Map(0, &range, (void**)&indexMapAddress_[1]);

	// 確保したインデックスバッファに頂点インデックスデータを書き込む。
	WriteToMemory(indexMapAddress_[0], vertIndex_.data(), static_cast<size_t>(indexStride_ * indexCount_));
	WriteToMemory(indexMapAddress_[1], vertIndex_.data(), static_cast<size_t>(indexStride_ * indexCount_));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(indexBuffer_[0].Get(), indexUploadBuffer_[0].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer_[0].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(indexBuffer_[1].Get(), indexUploadBuffer_[1].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer_[1].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// 頂点インデックスデータでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor_[0].CreateStructuredSRV(indexBuffer_[0], indexCount_, 0, indexStride_, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor_[0].CreateStructuredSRV(indexBuffer_[1], indexCount_, 0, indexStride_, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor_[1].CreateStructuredSRV(indexBuffer_[0], indexCount_, 0, indexStride_, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor_[1].CreateStructuredSRV(indexBuffer_[1], indexCount_, 0, indexStride_, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// 頂点データでディスクリプタを生成。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor_[0].CreateStructuredSRV(vertexBuffer_[0], vertexCount_, 0, vertexStride_, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor_[0].CreateStructuredSRV(vertexBuffer_[1], vertexCount_, 0, vertexStride_, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor_[1].CreateStructuredSRV(vertexBuffer_[0], vertexCount_, 0, vertexStride_, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();
	vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor_[1].CreateStructuredSRV(vertexBuffer_[1], vertexCount_, 0, vertexStride_, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

}
