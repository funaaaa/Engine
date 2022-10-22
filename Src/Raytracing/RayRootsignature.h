#pragma once
#include <DirectXTex/d3dx12.h>
#include <array>
#include <wrl.h>

// レイトレーシングで使用するグローバルルートシグネチャ、ローカルルートシグネチャクラス
class RayRootsignature {

private:

	/*===== 定数 =====*/

	static const int MAX_ROOTPARAM = 16;
	static const int MAX_SAMPLER = 26;


private:

	/*===== メンバ変数 =====*/

	std::array<CD3DX12_ROOT_PARAMETER, MAX_ROOTPARAM> rootparam_;	// ルートパラメーター
	std::array<CD3DX12_DESCRIPTOR_RANGE, MAX_ROOTPARAM> descRange_;	// ディスクリプタテーブル
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature_;		// ルートシグネチャ
	std::array<CD3DX12_STATIC_SAMPLER_DESC, MAX_SAMPLER> sampler_;	// スタティックサンプラー
	UINT rootparamCount_;											// ルートパラメーター数
	UINT samplerCount_;												// サンプラーの数


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	RayRootsignature() {
		rootparamCount_ = 0;
		samplerCount_ = 0;
	}

	// ルートパラメーター追加処理
	void AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT ShaderRegister, UINT RegisterSpace = 0);

	// スタティックサンプラー追加処理
	void AddStaticSampler(int RegisterSpace = 0);

	// ルートシグネチャの生成
	void Create(bool IsLocal, const wchar_t* Name = nullptr);

	// アクセッサ
	inline Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSig() { return rootsignature_; }

};