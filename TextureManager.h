#pragma once
#include "Singleton.h"
#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex/DirectXTex.h>

/*
テクスチャマネージャー
・SRVディスクリプタヒープとテクスチャクラス配列を持つ
・スプライトマネージャーから画像ファイル名を引数としてもらってSRVをディスクリプタヒープ上に生成し、何番目に生成したかを識別番号として保存する。
・指定のテクスチャのSRVを取得したい場合は、ディスクリプタヒープの先頭アドレスに識別番号分インクリメントすれば良い。
*/

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

/*
テクスチャクラス
・ファイル名、識別番号、テクスチャバッファを持っている構造体
*/
struct Texture {
	LPCWSTR fileName;				//ファイル名
	int IDNum;						//識別番号
	ComPtr<ID3D12Resource> texBuff;	//テクスチャバッファ
	//以下は必ず必要ではないがあったら便利
	TexMetadata metadata;			//画像のサイズなど、テクスチャとしての各情報が入る
	ScratchImage* scratchImg;
	Image* image;					//ビットマップ形式での画像データのそのもののアドレスが入る
	XMFLOAT4 colorData;				//色を指定してテクスチャを生成する際、このテクスチャがどの色なのかを渡すための変数
};

class TextureManager : public Singleton<TextureManager> {

private:

	ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	vector<Texture> texture;
	//コンストラクタ
	friend Singleton<TextureManager>;
	TextureManager();

public:
	//画像をロードして識別番号を返す関数、ロード済みだったら識別番号を返す。
	int LoadTexture(LPCWSTR fileName);
	//画像を生成して識別番号を返す関数
	int CreateTexture(XMFLOAT4 color);
	//レンダーターゲット用のテクスチャを生成して識別番号を返す関数
	int CreateRenderTargetTexture(int width, int height, int mipLevel, int arraySize, DXGI_FORMAT format, float clearColor[4]);
	//識別番号の順番のシェーダーリソースビューを返す。
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(int IDNum);
	//ディスクリプタヒープを返す。
	ComPtr<ID3D12DescriptorHeap> GetDescHeap();
	//指定の番号のテクスチャを返す。
	Texture GetTexture(int ID);
};