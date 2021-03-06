#pragma once
#include "DirectXBase.h"
#include "Singleton.h"

class DescriptorHeapMgr : public Singleton<DescriptorHeapMgr> {

private:

	/*===== メンバ変数 =====*/

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;	// CBV,SRV,UAV用ディスクリプタヒープ
	int head_;										// 先頭のインデックス


private:

	/*===== 定数 =====*/

	const int CBV_SRV_UAV_COUNT = 4096;	// CBV,SRV,UAV用ディスクリプタの数


public:

	/*===== メンバ関数 =====*/

	// ディスクリプタヒープの生成。
	void GenerateDescriptorHeap() {

		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// シェーダーから見える
		descHeapDesc.NumDescriptors = CBV_SRV_UAV_COUNT;
		// ディスクリプタヒープの生成。
		DirectXBase::Ins()->dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
		descriptorHeap_->SetName(L"DescriptorHeapMgr");

		// 先頭を初期化
		head_ = 0;

	}

	// ディスクリプタヒープのゲッタ。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_; }

	// 先頭のゲッタ
	const int& GetHead() { return head_; }

	// 先頭をインクリメント
	inline void IncrementHead() { ++head_; }

	// 指定のインデックスのCPUハンドルを取得
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleIncrement(const int& Index){

	return CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descriptorHeap_.Get()->GetGPUDescriptorHandleForHeapStart(), Index, DirectXBase::Ins()->dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	}


};