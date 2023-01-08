#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "TextureManager.h"
#include "FHelper.h"
#include "HitGroupMgr.h"
#include "DoubleRayDescriptor.h"


void BLAS::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// パスを保存。
	hitGroupInfo_.modelPath_ = DirectryPath + ModelName;

	// BlasのIndexを保存。
	blasInfo_.blasIndex_ = BlasIndex;


	/*-- 形状データを読み込む --*/

	// 読み込んだデータを一時保存する用のバッファ。
	ModelDataManager::ObjectData dataBuff;

	// モデルをロード。
	ModelDataManager::Ins()->LoadObj(DirectryPath, ModelName, dataBuff, false);

	// 各成分の値を保存。
	modelInfo_.vertexMax_ = dataBuff.vertexMax_;
	modelInfo_.vertexMin_ = dataBuff.vertexMin_;

	// マテリアル情報を保存。
	blasInfo_.material_.baseColor_ = dataBuff.material_.baseColor_;
	blasInfo_.material_.mapParam_ = dataBuff.material_.mapParam_;
	blasInfo_.material_.roughness_ = dataBuff.material_.roughness_;
	blasInfo_.material_.metalness_ = dataBuff.material_.metalness_;
	blasInfo_.material_.specular_ = dataBuff.material_.specular_;

	// テクスチャを保存。
	hitGroupInfo_.baseTextureHandle_ = dataBuff.textureHandle_;

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
		modelInfo_.isOpaque_ = IsOpaque;

		// BLASバッファを設定、構築する。
		SettingAccelerationStructure(geomDesc, index);

	}


	// ヒットグループ名を保存する。
	this->hitGroupInfo_.hitGroupName_ = HitGroupName;

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexInfo_.vertexPos_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexNormal_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexUV_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertexInfo_.vertex_) {
		vertexInfo_.vertexPos_[counter] = index_.position_;
		vertexInfo_.vertexNormal_[counter] = index_.normal_;
		vertexInfo_.vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// 頂点を保存。
	vertexInfo_.defVertex_ = vertexInfo_.vertex_;

	// ダーティフラグ
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;

	blasInfo_.isGenerate_ = true;

}

#include "FString.h"
void BLAS::GenerateBLASGLTF(const std::wstring& Path, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// パスを保存。
	hitGroupInfo_.modelPath_ = FString::WStringToString(Path);

	// BlasのIndexを保存。
	blasInfo_.blasIndex_ = BlasIndex;


	/*-- 形状データを読み込む --*/

	// 読み込んだデータを一時保存する用のバッファ。
	ModelDataManager::ObjectData dataBuff;

	// モデルをロード。
	ModelDataManager::Ins()->LoadGLTF(Path, dataBuff);

	// 各成分の値を保存。
	modelInfo_.vertexMax_ = dataBuff.vertexMax_;
	modelInfo_.vertexMin_ = dataBuff.vertexMin_;

	// マテリアル情報を保存。
	blasInfo_.material_.baseColor_ = dataBuff.material_.baseColor_;
	blasInfo_.material_.mapParam_ = dataBuff.material_.mapParam_;
	blasInfo_.material_.roughness_ = dataBuff.material_.roughness_;
	blasInfo_.material_.metalness_ = dataBuff.material_.metalness_;
	blasInfo_.material_.specular_ = dataBuff.material_.specular_;

	// テクスチャを保存。
	hitGroupInfo_.baseTextureHandle_ = dataBuff.textureHandle_;

	// マテリアルバッファを生成する。
	CreateMaterialBuffer();

	// 頂点数を求める。
	modelInfo_.vertexCount_ = static_cast<UINT>(dataBuff.vertex_.size());

	// 頂点インデックス数を求める。
	modelInfo_.indexCount_ = static_cast<UINT>(dataBuff.index_.size());

	// 頂点データを変換。
	for (int index = 0; index < static_cast<int>(modelInfo_.vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = dataBuff.vertex_[index].normal_;
		buff.position_ = dataBuff.vertex_[index].pos_;
		buff.uv_ = dataBuff.vertex_[index].uv_;

		// データを保存。
		vertexInfo_.vertex_.push_back(buff);

	}

	// 頂点バッファを生成。
	CreateVertexBuffer(dataBuff);


	/*-- BLASバッファを生成する --*/

	for (int index = 0; index < 2; ++index) {

		// 形状を設定する用の構造体を設定。
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		modelInfo_.isOpaque_ = IsOpaque;

		// BLASバッファを設定、構築する。
		SettingAccelerationStructure(geomDesc, index);

	}


	// ヒットグループ名を保存する。
	this->hitGroupInfo_.hitGroupName_ = HitGroupName;

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexInfo_.vertexPos_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexNormal_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexUV_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertexInfo_.vertex_) {
		vertexInfo_.vertexPos_[counter] = index_.position_;
		vertexInfo_.vertexNormal_[counter] = index_.normal_;
		vertexInfo_.vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// 頂点を保存。
	vertexInfo_.defVertex_ = vertexInfo_.vertex_;

	// ダーティフラグ
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;

	blasInfo_.isGenerate_ = true;

}

void BLAS::GenerateBLASData(const ModelDataManager::ObjectData& ModelData, int BlasIndex, bool IsOpaque)
{

	/*===== BLASを生成する処理 =====*/

	// パスを保存。
	hitGroupInfo_.modelPath_ = "SelfModel";

	// BlasのIndexを保存。
	blasInfo_.blasIndex_ = BlasIndex;


	/*-- 形状データを読み込む --*/

	// 各成分の値を保存。
	modelInfo_.vertexMax_ = ModelData.vertexMax_;
	modelInfo_.vertexMin_ = ModelData.vertexMin_;

	// マテリアル情報を保存。
	blasInfo_.material_.baseColor_ = ModelData.material_.baseColor_;
	blasInfo_.material_.mapParam_ = ModelData.material_.mapParam_;
	blasInfo_.material_.roughness_ = ModelData.material_.roughness_;
	blasInfo_.material_.metalness_ = ModelData.material_.metalness_;
	blasInfo_.material_.specular_ = ModelData.material_.specular_;

	// テクスチャを保存。
	hitGroupInfo_.baseTextureHandle_ = ModelData.textureHandle_;

	// マテリアルバッファを生成する。
	CreateMaterialBuffer();

	// 頂点バッファを生成。
	CreateVertexBuffer(ModelData);


	/*-- BLASバッファを生成する --*/

	for (int index = 0; index < 2; ++index) {

		// 形状を設定する用の構造体を設定。
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		modelInfo_.isOpaque_ = IsOpaque;

		// BLASバッファを設定、構築する。
		SettingAccelerationStructure(geomDesc, index);

	}


	// ヒットグループ名を保存する。
	this->hitGroupInfo_.hitGroupName_ = HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::Ins()->DEF];

	// デバッグで使用する頂点のみのデータと法線のみのデータを生成する。
	vertexInfo_.vertexPos_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexNormal_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexUV_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertexInfo_.vertex_) {
		vertexInfo_.vertexPos_[counter] = index_.position_;
		vertexInfo_.vertexNormal_[counter] = index_.normal_;
		vertexInfo_.vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// 頂点を保存。
	vertexInfo_.defVertex_ = vertexInfo_.vertex_;

	// ダーティフラグ
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;

	blasInfo_.isGenerate_ = true;

}

void BLAS::Update()
{

	/*===== BLASの更新 =====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// 生成していなかったら処理を飛ばす。
	if (!blasInfo_.isGenerate_) return;

	// 頂点を書き込む。 今のところは頂点しか書き換える予定はないが、後々他のやつも書き込む。ダーティフラグみたいなのを用意したい。
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, sizeof(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

	// 更新のための値を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(modelInfo_.isOpaque_, currentQueueIndex);
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
	asDesc.SourceAccelerationStructureData = blasInfo_.blasBuffer_[currentQueueIndex]->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = blasInfo_.blasBuffer_[currentQueueIndex]->GetGPUVirtualAddress();
	// 更新用の作業バッファを設定する。
	asDesc.ScratchAccelerationStructureData = blasInfo_.updateBuffer_[currentQueueIndex]->GetGPUVirtualAddress();

	// コマンドリストに積む。
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure(currentQueueIndex);

}

void BLAS::InitAnimation()
{
	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== アニメーションさせる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== アニメーションを停止させる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).StopAnimation();

	}

}
void BLAS::IsChangeMaterial()
{

	/*===== マテリアルを書き換えた際の処理 =====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// 確保したバッファにマテリアルデータを書き込む。
	blasInfo_.materialBuffer_->WriteData(currentQueueIndex, sizeof(ModelDataManager::GPUMaterial), &blasInfo_.material_);

	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;

}
void BLAS::ChangeBaseTexture(int Index)
{
	hitGroupInfo_.baseTextureHandle_ = Index;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;
}
void BLAS::ChangeMapTexture(int Index, MAP_PARAM MapParam)
{
	hitGroupInfo_.mapTextureHandle_ = Index;
	blasInfo_.material_.mapParam_ = static_cast<int>(MapParam);
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;
	IsChangeMaterial();
}
#include "HitGroupMgr.h"
#include <assert.h>
BLAS::BLAS()
{

	blasInfo_.isGenerate_ = false;
	hitGroupInfo_.baseTextureHandle_ = -1;
	hitGroupInfo_.mapTextureHandle_ = -1;
	blasInfo_.blasIndex_ = -1;
	modelInfo_.indexCount_ = 0;
	blasInfo_.indexDescriptor_ = {};
	modelInfo_.indexStride_ = {};
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;
	modelInfo_.isOpaque_ = false;
	blasInfo_.materialDescriptor_ = {};
	modelInfo_.modelIndex_ = 0;
	modelInfo_.vertexCount_ = 0;
	blasInfo_.vertexDescriptor_ = {};
	modelInfo_.vertexStride_ = 0;
}

uint8_t* BLAS::WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName, int Index)
{

	/*===== シェーダーレコードを書き込む =====*/

	// 生成されていなかったら飛ばす。
	if (!blasInfo_.isGenerate_) {


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

	// シェーダー識別子を書き込む。
	memcpy(Dst, mode_, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;


	// 頂点関係のデータが変更されていたら。
	if (hitGroupInfo_.isChangeVertex_[Index]) {
		// 今回のプログラムでは以下の順序でディスクリプタを記録。
		// [0] : インデックスバッファ
		// [1] : 頂点バッファ
		// ※ ローカルルートシグネチャの順序に合わせる必要がある。
		Dst += WriteGPUDescriptor(Dst, &blasInfo_.indexDescriptor_->GetDescriptor(Index).lock()->GetGPUHandle());
		Dst += WriteGPUDescriptor(Dst, &blasInfo_.vertexDescriptor_->GetDescriptor(Index).lock()->GetGPUHandle());

		// マテリアル用のバッファをセット。
		Dst += WriteGPUDescriptor(Dst, &blasInfo_.materialDescriptor_->GetDescriptor(Index).lock()->GetGPUHandle());

		hitGroupInfo_.isChangeVertex_[Index] = false;

	}
	else {

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
	if (hitGroupInfo_.isChangeTexture_[Index]) {

		// ここはテクスチャのサイズではなく、パイプラインにセットされたSRVの数を持ってきてそれを使う。
		// この時点でSRVの数とテクスチャの数が合っていなかったらassertを出す。
		for (int index = 0; index < srvCount; ++index) {

			// 0番目は基本テクスチャ。
			if (index == 0) {

				// 基本のテクスチャが設定されていない。
				if (hitGroupInfo_.baseTextureHandle_ == -1) assert(0);

				CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.baseTextureHandle_);
				Dst += WriteGPUDescriptor(Dst, &texDescHandle);

			}
			// 1番目は法線マップテクスチャ。
			else if (index == 1) {

				if (hitGroupInfo_.mapTextureHandle_ != -1) {

					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.mapTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}
				else {

					// 法線マップが設定されていなかったら、メモリの隙間を埋めるため通常のテクスチャを書き込む。
					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.baseTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}

			}

		}

		hitGroupInfo_.isChangeTexture_[Index] = false;

	}
	else {

		Dst += static_cast<UINT>(sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)) * static_cast<UINT>(srvCount);

	}

	// 使用するUAVの数を取得。
	int uavCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID);
	for (int index = 0; index < uavCount; ++index) {

		// テクスチャが存在していたら。
		if (0 < hitGroupInfo_.uavHandle_.size()) {

			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.uavHandle_[index]);
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

	hitGroupInfo_.texturePath_.clear();
	vertexInfo_.defVertex_.clear();
	vertexInfo_.vertex_.clear();
	vertexInfo_.vertexPos_.clear();
	vertexInfo_.vertexNormal_.clear();
	vertexInfo_.vertexUV_.clear();
	vertexInfo_.vertIndex_.clear();
	blasInfo_.isGenerate_ = false;
	hitGroupInfo_.baseTextureHandle_ = -1;
	hitGroupInfo_.mapTextureHandle_ = -1;

}

Vec3 BLAS::GetVertexLengthMax()
{

	/*===== 各成分の最大の長さを返す =====*/

	Vec3 vertexLength;

	if (fabs(modelInfo_.vertexMin_.x_) < fabs(modelInfo_.vertexMax_.x_)) {
		vertexLength.x_ = fabs(modelInfo_.vertexMax_.x_);
	}
	else {
		vertexLength.x_ = fabs(modelInfo_.vertexMin_.x_);
	}

	if (fabs(modelInfo_.vertexMin_.y_) < fabs(modelInfo_.vertexMax_.y_)) {
		vertexLength.y_ = fabs(modelInfo_.vertexMax_.y_);
	}
	else {
		vertexLength.y_ = fabs(modelInfo_.vertexMin_.y_);
	}

	if (fabs(modelInfo_.vertexMin_.z_) < fabs(modelInfo_.vertexMax_.z_)) {
		vertexLength.z_ = fabs(modelInfo_.vertexMax_.z_);
	}
	else {
		vertexLength.z_ = fabs(modelInfo_.vertexMin_.z_);
	}

	return vertexLength;
}

void BLAS::MulVec3Vertex(Vec3 Vec)
{

	/*===== すべての頂点に行列情報をかける ====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	for (auto& index_ : vertexInfo_.vertex_) {

		index_.position_ = vertexInfo_.defVertex_[&index_ - &vertexInfo_.vertex_[0]].position_ * Vec;

	}

	// 確保したバッファに頂点データを書き込む。
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

}

void BLAS::AssignUV(const std::vector<RayVertex>& UV)
{

	/*===== 指定のUVを代入する =====*/

	// 現在のQueueのインデックス
	int currentQueueIndex = 0;

	for (auto& index : vertexInfo_.vertex_) {

		index.subUV_ = UV[static_cast<int>(&index - &vertexInfo_.vertex_[0])].uv_;

	}

	// 確保したバッファに頂点データを書き込む。
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

	// 現在のQueueのインデックス
	currentQueueIndex = 1;

	// 確保したバッファに頂点データを書き込む。
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

	// 頂点を書き換えたフラグを立てる。
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;

}

void BLAS::ChangeVertexPosition(int Index, const Vec3& Pos)
{

	/*===== 頂点情報の座標成分のみを変更 =====*/

	vertexInfo_.vertex_[Index].position_ = Pos;
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;

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
	triangles.VertexBuffer.StartAddress = blasInfo_.vertexBuffer_->GetBuffer(Index)->GetGPUVirtualAddress();
	triangles.VertexBuffer.StrideInBytes = modelInfo_.vertexStride_;
	triangles.VertexCount = modelInfo_.vertexCount_;
	triangles.IndexBuffer = blasInfo_.indexBuffer_->GetBuffer(Index)->GetGPUVirtualAddress();
	triangles.IndexCount = modelInfo_.indexCount_;
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
	blasInfo_.scratchBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(blasInfo_.scratchBuffer_[Index].Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, barrierToUAV);
	blasInfo_.scratchBuffer_[Index]->SetName(L"BLASScratchBuffer");

	// BLASのバッファを生成する。
	blasInfo_.blasBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	blasInfo_.blasBuffer_[Index]->SetName(L"BLASBuffer");

	// 更新用バッファを生成する。
	blasInfo_.updateBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	barrierToUAV[0] = { CD3DX12_RESOURCE_BARRIER::Transition(blasInfo_.updateBuffer_[Index].Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, barrierToUAV);
	blasInfo_.updateBuffer_[Index]->SetName(L"BLASUpdateBuffer");

	// AccelerationStructureの構築。
	buildASDesc.ScratchAccelerationStructureData = blasInfo_.scratchBuffer_[Index]->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasInfo_.blasBuffer_[Index]->GetGPUVirtualAddress();
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
	uavBarrier.UAV.pResource = blasInfo_.blasBuffer_[Index].Get();
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &uavBarrier);

}

void BLAS::CreateMaterialBuffer()
{

	/*===== マテリアルを設定 =====*/

	// マテリアル用バッファを生成
	blasInfo_.materialBuffer_ = std::make_shared<DoubleResourceWrapper>();
	blasInfo_.materialBuffer_->CreateBuffer(sizeof(ModelDataManager::GPUMaterial), &blasInfo_.material_, L"MaterialBuffer");

	// マテリアルデータでディスクリプタを生成。
	blasInfo_.materialDescriptor_ = std::make_shared<DoubleRayDescriptor>();
	blasInfo_.materialDescriptor_->CreateSRV(blasInfo_.materialBuffer_, 1, 0, sizeof(ModelDataManager::GPUMaterial));

}

void BLAS::CreateVertexBuffer(const ModelDataManager::ObjectData& DataBuff)
{

	/*===== 頂点とインデックスバッファを生成 =====*/

	// 頂点数を求める。
	modelInfo_.vertexCount_ = static_cast<UINT>(DataBuff.vertex_.size());

	// 頂点インデックス数を求める。
	modelInfo_.indexCount_ = static_cast<UINT>(DataBuff.index_.size());

	// 頂点データを変換。
	for (int index = 0; index < static_cast<int>(modelInfo_.vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = DataBuff.vertex_[index].normal_;
		buff.position_ = DataBuff.vertex_[index].pos_;
		buff.uv_ = DataBuff.vertex_[index].uv_;
		buff.subUV_ = DataBuff.vertex_[index].uv_;

		// データを保存。
		vertexInfo_.vertex_.push_back(buff);

	}

	// 頂点インデックスデータを保存。
	vertexInfo_.vertIndex_ = DataBuff.index_;

	// 頂点サイズを求める。
	modelInfo_.vertexStride_ = sizeof(RayVertex);

	// 頂点インデックスサイズを求める。
	modelInfo_.indexStride_ = sizeof(UINT);

	// 頂点バッファを生成。
	blasInfo_.vertexBuffer_ = std::make_shared<DoubleResourceWrapper>();
	blasInfo_.vertexBuffer_->CreateBuffer(static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data(), L"VertexBuffer");

	// 頂点インデックスバッファを生成。
	blasInfo_.indexBuffer_ = std::make_shared<DoubleResourceWrapper>();
	blasInfo_.indexBuffer_->CreateBuffer(static_cast<size_t>(modelInfo_.indexStride_ * modelInfo_.indexCount_), vertexInfo_.vertIndex_.data(), L"VertexIndexBuffer");

	// 頂点インデックスデータでディスクリプタを生成。
	blasInfo_.indexDescriptor_ = std::make_shared<DoubleRayDescriptor>();
	blasInfo_.indexDescriptor_->CreateSRV(blasInfo_.indexBuffer_, modelInfo_.indexCount_, 0, modelInfo_.indexStride_);

	// 頂点データでディスクリプタを生成。
	blasInfo_.vertexDescriptor_ = std::make_shared<DoubleRayDescriptor>();
	blasInfo_.vertexDescriptor_->CreateSRV(blasInfo_.vertexBuffer_, modelInfo_.vertexCount_, 0, modelInfo_.vertexStride_);

}
