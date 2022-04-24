#pragma once

#include <string>
#include <DirectXMath.h>
#include <vector>
#include <xaudio2.h>
#include <wrl.h>
#include <d3d12.h>
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_dx12.h"
//#include "imgui/imgui_impl_win32.h"

#pragma comment(lib,"xaudio2.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

#define SUBONE(x) x - 1

//#define GetRand(x) (float)(rand() % (x + 1)) - (x / 2.0f)
//#define GetRandPlus(x) (float)(rand() % (x + 1))

#pragma comment(lib,"winmm.lib")

//インスタンシング描画をする際の最大数
#define INSTANCE_COUNT 600

//頂点データ構造体
struct Vertex {
	XMFLOAT3 pos;			//xyz座標
	XMFLOAT3 normal;		//法線
	XMFLOAT2 uv;			//uv座標
};

//シェーダー側に渡すための行列データ
struct MatrixData {
	XMMATRIX world;			//回転させたり移動させたりする行列
	XMMATRIX viewproj;		//ビューとプロジェクション合成行列
};

//定数バッファ構造体b0
struct ConstBufferDataB0 {
	MatrixData mat;			//行列
	XMFLOAT4 color = { 1,1,1,1 };			//色データ
	XMFLOAT3 eye;			//カメラ座標
	float pad1;
};
struct GSConstBufferDataB0 {
	XMMATRIX world;			//回転させたり移動させたりする行列
	XMFLOAT4 color = { 1,1,1,1 };			//色データ
};
//定数バッファ構造体b1
struct ConstBufferDataB1 {
	XMFLOAT3 ambient;		//アンビエント係数
	float pad1;				//パディング
	XMFLOAT3 diffuse;		//ディフューズ係数
	float pad2;				//パディング
	XMFLOAT3 specular;		//スペキュラー係数
	float alpha;			//アルファ
	XMFLOAT3 pad3;			//パディング
};

/*-----ライト関係-----*/
//各ライトの数
#define POINTLIGHT_NUM 120
#define DIRLIGHT_NUM 1
#define SPOTLIGHT_NUM 1
#define CIRCLESHADOW_NUM 120
//点光源用の構造体
struct PointLightData {
	XMFLOAT3 lightpos;				//ライト座標
	float pad1;
	XMFLOAT3 lightcolor;			//ライト色
	float pad2;
	XMFLOAT3 lightatten;			//ライト距離減衰係数
	unsigned int active = 0;		//アクティブフラグ
};
//面光源用の構造体
struct DirLightData {
	XMFLOAT3 lightvec;				//ライトの方向ベクトル
	float pad1;
	XMFLOAT3 lightcolor;			//ライト色
	unsigned int active = 0;		//アクディブフラグ
};
//スポットライト用の構造体
struct SpotLightData {
	XMVECTOR lightv;				//ライトの方向ベクトル
	XMFLOAT3 lightpos;				//ライト座標
	float pad1;
	XMFLOAT3 lightcolor;			//ライト色
	float pad2;
	XMFLOAT3 lightatten;			//ライト距離減衰係数
	float pad3;
	XMFLOAT2 lightfactoranglecos;	//ライト角度減衰係数
	unsigned int active = 0;		//アクティブフラグ
	float pad4;
};
//シャドウライト用の構造体
struct CircleShadowData {
	XMVECTOR dir;					//影の投影方向を表す単位ベクトル
	XMFLOAT3 centerpos;				//光を遮る影の主体の座標
	float distanceCasterLight;		//光を遮る影の主体の座標とシャドウライト光源の距離
	XMFLOAT3 atten;					//距離減衰に使う数値
	float pad3;
	XMFLOAT2 facterAngleCos;		//減衰開始角度と終了角度
	unsigned int active;			//アクティブフラグ
	float pad4;
};
//定数バッファb
struct ConstBufferDataB2 {
	PointLightData pointlights[POINTLIGHT_NUM];			//点光源
	DirLightData dirlights[DIRLIGHT_NUM];				//面光源
	SpotLightData spotlights[SPOTLIGHT_NUM];			//スポットライト
	//CircleShadowData circleShadowData[CIRCLESHADOW_NUM];//丸影光源
};
struct ConstBufferDataB2Shadow {
	PointLightData pointlights[POINTLIGHT_NUM];			//点光源
	DirLightData dirlights[DIRLIGHT_NUM];				//面光源
	SpotLightData spotlights[SPOTLIGHT_NUM];			//スポットライト
	CircleShadowData circleShadowData[CIRCLESHADOW_NUM];//丸影光源
};


//基本的な描画データ
struct BasicDrawingData {
	bool isDisplay = true;						//描画するかどうか
	vector<int> textureID;						//テクスチャのIDナンバー
	int projectionID;							//どの投影をするかのID
	int piplineID;								//使用するパイプラインの名前
};

//ライン用データ構造体
//struct Line {
//	/*-----各種基本データ-----*/
//	BasicDrawingData basicdata;					//基本的な描画データ
//	/*-----頂点データ関係-----*/
//	vector<Vertex> vertex;						//頂点
//	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
//	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
//	/*-----定数データ関係-----*/
//	ConstBufferDataB0 constBufferDataB0;		//定数 行列や視点座標、カラーなど
//	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
//	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBV用ディスクリプタヒープ
//	/*-----各種行列-----*/
//	XMMATRIX scaleMat;							//スケーリング行列
//	XMMATRIX rotationMat;						//回転行列(集積値)
//	XMMATRIX positionMat;						//座標行列
//	/*-----デバッグor当たり判定用-----*/
//	XMFLOAT3 pos1;								//座標
//	XMFLOAT3 pos2;								//座標
//};

//2Dスプライト用データ構造体
//struct Sprite {
//	/*-----各種基本データ-----*/
//	BasicDrawingData basicdata;					//基本的な描画データ
//	/*-----頂点データ関係-----*/
//	vector<Vertex> vertex;						//頂点
//	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
//	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
//	/*-----定数データ関係-----*/
//	ConstBufferDataB0 constBufferDataB0;		//定数 行列や視点座標、カラーなど
//	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
//	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBV用ディスクリプタヒープ
//	/*-----各種行列-----*/
//	XMMATRIX scaleMat;							//スケーリング行列
//	XMMATRIX rotationMat;						//回転行列(集積値)
//	XMMATRIX positionMat;						//座標行列
//	/*-----デバッグor当たり判定用-----*/
//	XMFLOAT3 pos;								//座標
//};


struct SpriteMultiTexture {
	/*-----各種基本データ-----*/
	bool isDisplay = true;						//描画するかどうか
	vector<int> textureID;						//テクスチャのIDナンバー
	int projectionID;							//どの投影をするかのID
	int piplineID;								//使用するパイプラインの名前
	/*-----頂点データ関係-----*/
	vector<Vertex> vertex;						//頂点
	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
	/*-----定数データ関係-----*/
	ConstBufferDataB0 constBufferDataB0;		//定数 行列や視点座標、カラーなど
	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBV用ディスクリプタヒープ
	/*-----各種行列-----*/
	XMMATRIX scaleMat;							//スケーリング行列
	XMMATRIX rotationMat;						//回転行列(集積値)
	XMMATRIX positionMat;						//座標行列
	/*-----デバッグor当たり判定用-----*/
	XMFLOAT3 pos;								//座標
};

//マルチパスレンダリング用の定数構造体
struct ConstBufferDataMultiPath {
	XMFLOAT4 gaussianWeight[2];					//ガウスフィルターに使用する重み
	float pad1;
};

//マルチパスレンダリング用データ構造体
//struct SpriteMultiPath {
//	/*-----各種基本データ-----*/
//	BasicDrawingData basicdata;					//基本的な描画データ
//	/*-----頂点データ関係-----*/
//	vector<Vertex> vertex;						//頂点
//	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
//	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
//	/*-----定数データ関係-----*/
//	ConstBufferDataB0 constBufferDataB0;		//定数 行列や視点座標、カラーなど
//	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
//	ConstBufferDataMultiPath constBufferDataB1;	//定数 マルチパスレンダリングに使用する各情報
//	ComPtr<ID3D12Resource> constBuffB1;			//定数バッファ
//	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBV用ディスクリプタヒープ
//	/*-----各種行列-----*/
//	XMMATRIX scaleMat;							//スケーリング行列
//	XMMATRIX rotationMat;						//回転行列(集積値)
//	XMMATRIX positionMat;						//座標行列
//	/*-----デバッグor当たり判定用-----*/
//	XMFLOAT3 pos;								//座標
//};

//3Dオブジェクト用データ構造体
//struct Object3DData {
//	/*-----各種基本データ-----*/
//	BasicDrawingData basicdata;					//基本的な描画データ
//	/*-----頂点データ関係-----*/
//	vector<Vertex> vertex;						//頂点
//	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
//	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
//	vector<unsigned short> index;				//頂点インデックス
//	ComPtr<ID3D12Resource> indexBuff;			//頂点インデックスバッファ
//	D3D12_INDEX_BUFFER_VIEW ibView;				//頂点インデックスバッファビュー
//	/*-----定数データ関係-----*/
//	ConstBufferDataB0 constBuffDataB0;			//定数1	行列など
//	ConstBufferDataB1 constBuffDataB1;			//定数2	マテリアルなど
//	ConstBufferDataB2 constBuffDataB2;			//定数3	ライトなど
//	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB1;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB2;			//定数バッファ
//	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBVディスクリプタヒープ
//	/*-----各種行列-----*/
//	XMMATRIX scaleMat;							//スケーリング行列
//	XMMATRIX rotationMat;						//回転行列(集積値)
//	XMMATRIX positionMat;						//座標行列
//	/*-----デバッグor当たり判定用-----*/
//	XMFLOAT3 pos;								//座標
//	float radius;								//半径
//};

////3Dオブジェクト用データ構造体
//struct Object3D {
//	/*-----各種基本データ-----*/
//	BasicDrawingData basicdata;					//基本的な描画データ
//	/*-----頂点データ関係-----*/
//	vector<Vertex> vertex;						//頂点
//	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
//	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
//	vector<unsigned short> index;				//頂点インデックス
//	ComPtr<ID3D12Resource> indexBuff;			//頂点インデックスバッファ
//	D3D12_INDEX_BUFFER_VIEW ibView;				//頂点インデックスバッファビュー
//	/*-----定数データ関係-----*/
//	ConstBufferDataB0 constBuffDataB0;			//定数1	行列など
//	ConstBufferDataB1 constBuffDataB1;			//定数2	マテリアルなど
//	ConstBufferDataB2 constBuffDataB2;			//定数3	ライトなど
//	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB1;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB2;			//定数バッファ
//	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBVディスクリプタヒープ
//	/*-----各種行列-----*/
//	XMMATRIX scaleMat;							//スケーリング行列
//	XMMATRIX rotationMat;						//回転行列(集積値)
//	XMMATRIX positionMat;						//座標行列
//	/*-----デバッグor当たり判定用-----*/
//	XMFLOAT3 pos;								//座標
//	float radius;								//半径
//};
////3Dオブジェクト用データ構造体
//struct Object3DShadow {
//	/*-----各種基本データ-----*/
//	BasicDrawingData basicdata;					//基本的な描画データ
//	/*-----頂点データ関係-----*/
//	vector<Vertex> vertex;						//頂点
//	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
//	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
//	vector<unsigned short> index;				//頂点インデックス
//	ComPtr<ID3D12Resource> indexBuff;			//頂点インデックスバッファ
//	D3D12_INDEX_BUFFER_VIEW ibView;				//頂点インデックスバッファビュー
//	/*-----定数データ関係-----*/
//	ConstBufferDataB0 constBuffDataB0;			//定数1	行列など
//	ConstBufferDataB1 constBuffDataB1;			//定数2	マテリアルなど
//	ConstBufferDataB2Shadow constBuffDataB2;			//定数3	ライトなど
//	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB1;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB2;			//定数バッファ
//	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBVディスクリプタヒープ
//	/*-----各種行列-----*/
//	XMMATRIX scaleMat;							//スケーリング行列
//	XMMATRIX rotationMat;						//回転行列(集積値)
//	XMMATRIX positionMat;						//座標行列
//	/*-----デバッグor当たり判定用-----*/
//	XMFLOAT3 pos;								//座標
//	float radius;								//半径
//};
//インスタンシング描画用 チーム制作が終わったらインスタンシング描画用の定数バッファとして用意する
//struct Object3DInstance {
//	/*-----各種基本データ-----*/
//	BasicDrawingData basicdata;					//基本的な描画データ
//	/*-----頂点データ関係-----*/
//	vector<Vertex> vertex;						//頂点
//	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
//	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
//	vector<unsigned short> index;				//頂点インデックス
//	ComPtr<ID3D12Resource> indexBuff;			//頂点インデックスバッファ
//	D3D12_INDEX_BUFFER_VIEW ibView;				//頂点インデックスバッファビュー
//	/*-----定数データ関係-----*/
//	ConstBufferDataB0 constBuffDataB0;			//定数1	行列など
//	ConstBufferDataB1 constBuffDataB1;			//定数2	マテリアルなど
//	ConstBufferDataB2 constBuffDataB2;			//定数3	ライトなど
//	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB1;			//定数バッファ
//	ComPtr<ID3D12Resource> constBuffB2;			//定数バッファ
//	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBVディスクリプタヒープ
//	/*-----各種行列-----*/
//	XMMATRIX scaleMat;							//スケーリング行列
//	XMMATRIX rotationMat;						//回転行列(集積値)
//	XMMATRIX positionMat;						//座標行列
//	/*-----デバッグor当たり判定用-----*/
//	XMFLOAT3 pos;								//座標
//	float radius;								//半径
//};

//ModelDataManagerとObject3D間のデータの受け渡し用関数
struct Object3DDeliveryData {
	vector<Vertex> vertex;
	vector<UINT> index;
	ConstBufferDataB1 constBufferDataB1;
};

//GSパーティクル用データ構造体
struct GSParticleData {
	int textureID;								//テクスチャのIDナンバー
	int piplineID;								//使うパイプラインの名前
	XMFLOAT3 vertex;
	ComPtr<ID3D12Resource> vertBuff;			//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;			//頂点バッファビュー
	GSConstBufferDataB0 constBuffDataB0;		//定数1	行列など
	ComPtr<ID3D12Resource> constBuffB0;			//定数バッファ
	ComPtr<ID3D12DescriptorHeap> constDescHeap;	//CBVディスクリプタヒープ
	XMMATRIX scaleMat;							//スケーリング倍率
	XMMATRIX rotationMat;						//回転行列(集積値)
	XMMATRIX positionMat;						//座標行列
	/*-----デバッグor当たり判定用-----*/
	XMFLOAT3 pos;								//座標
};

//マテリアル
struct Material {
	string name;								//マテリアル名
	string textureName;							//テクスチャの名前
	XMFLOAT3 ambient = { 0.3f,0.3f,0.3f };		//アンビエント影響度
	XMFLOAT3 diffuse = { 0.3f,0.3f,0.3f };		//ディフューズ影響度
	XMFLOAT3 specular = { 0.3f,0.3f,0.3f };		//スペキュラー影響度
	float alpha;								//アルファ
};

//modelのデータ構造体
struct ModelData {
	string modelName;							//モデルファイル名
	Material material;					//マテリアル
	vector<unsigned short> index;		//頂点インデックス
	vector<Vertex> vertex;				//頂点
	bool isSmoothing;							//法線をスムーズにするかどうか
};

//3D図形データ構造体
struct ShapeData {
	int dataID;									//図形識別ID
	vector<unsigned short> index;				//頂点インデックス
	vector<Vertex> vertex;						//頂点
};

/*
テクスチャクラス
・ファイル名、識別番号、テクスチャバッファを持っている構造体
*/
//struct Texture {
//	LPCWSTR fileName;				//ファイル名
//	int IDNum;						//識別番号
//	ComPtr<ID3D12Resource> texBuff;	//テクスチャバッファ
//	//以下は必ず必要ではないがあったら便利
//	TexMetadata metadata;			//画像のサイズなど、テクスチャとしての各情報が入る
//	ScratchImage* scratchImg;
//	Image* image;					//ビットマップ形式での画像データのそのもののアドレスが入る
//	XMFLOAT4 colorData;				//色を指定してテクスチャを生成する際、このテクスチャがどの色なのかを渡すための変数
//};

//レイ構造体
struct Ray {
	XMVECTOR pos;
	XMVECTOR dir;
};
//uv用構造体
struct UVs {
	XMFLOAT2 leftUp;		//左上
	XMFLOAT2 leftDown;		//左下
	XMFLOAT2 rightDown;		//右下
	XMFLOAT2 rightUp;		//右上
};

//サウンド関係の構造体
//チャンクヘッダ
struct ChunkHeader {
	char id[4];		//チャンク毎のID
	int32_t size;	//チャンクサイズ
};
//RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	//"RIFF"
	char type[4];	//"WAVE
};
//FMTチャンク
struct FormatChunk {
	ChunkHeader chunk;		//"fmt"
	WAVEFORMATEX fmt;	//波形フォーマット
};

//音声データ
struct SoundData {
	WAVEFORMATEX wfex;			//波形フォーマット
	BYTE* pBuffer;				//バッファの先頭アドレス
	unsigned int bufferSize;	//バッファのサイズ
	IXAudio2SourceVoice* source;
	float volume;
};