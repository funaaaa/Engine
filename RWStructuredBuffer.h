#pragma once
#include <DirectXTex/d3dx12.h>

/// <summary>
/// 構造化バッファ
/// </summary>
class RWStructuredBuffer{
public:

	/// <summary>
	/// 構造化バッファを初期化。
	/// </summary>
	/// <param name="sizeOfElement">エレメントのサイズ。</param>
	/// <param name="numElement">エレメントの数。</param>
	/// <param name="initData">初期データ。</param>
	void Init(int sizeOfElement, int numElement, void* initData);

	/// <summary>
	/// UAVに登録。
	/// </summary>
	/// <param name=""></param>
	/// <param name="bufferNo"></param>
	void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo);

	/// <summary>
	/// SRVに登録。
	/// </summary>
	/// <param name="descriptorHandle"></param>
	/// <param name="bufferNo"></param>
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo);

	/// <summary>
	/// 初期化されているか判定。
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return isInited;
	}

	/// <summary>
	/// CPUからアクセス可能なリソースを取得する。
	/// </summary>
	/// <returns></returns>
	void* GetResourceOnCPU();


	ID3D12Resource* GetD3DResoruce() ;


public:

	ID3D12Resource* buffersOnGPU = {nullptr};
	void* buffersOnCPU = { nullptr };			//CPU側からアクセスできるするストラクチャバッファのアドレス。
	int numElement = 0;								//要素数。
	int sizeOfElement = 0;							//エレメントのサイズ。
	bool isInited = false;							//初期化済み？

};