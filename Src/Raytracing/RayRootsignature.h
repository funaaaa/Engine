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

	std::array<CD3DX12_ROOT_PARAMETER, MAX_ROOTPARAM> rootparam;	// ルートパラメーター
	std::array<CD3DX12_DESCRIPTOR_RANGE, MAX_ROOTPARAM> descRange;	// ディスクリプタテーブル
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature_;		// ルートシグネチャ
	std::array<CD3DX12_STATIC_SAMPLER_DESC, MAX_SAMPLER> sampler;	// スタティックサンプラー
	UINT rootparamCount;											// ルートパラメーター数
	UINT samplerCount;												// サンプラーの数


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	RayRootsignature() {
		rootparamCount = 0;
		samplerCount = 0;
	}

	// ルートパラメーター追加処理
	void AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT ShaderRegister, UINT RegisterSpace = 0);

	// スタティックサンプラー追加処理
	void AddStaticSampler(const int& RegisterSpace = 0);

	// ルートシグネチャの生成
	void Create(const bool& IsLocal, const wchar_t* Name = nullptr);

	// アクセッタ
	inline Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSig() { return rootsignature_; }

};