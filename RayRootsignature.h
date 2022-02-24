#pragma once
#include "DirectXBase.h"
#include <array>

using namespace std;

// レイトレーシングで使用するグローバルルートシグネチャ、ローカルルートシグネチャクラス
class RayRootsignature {

private:

	/*===== 定数 =====*/

	static const int MAX_ROOTPARAM = 16;


private:

	/*===== メンバ変数 =====*/

	array<CD3DX12_ROOT_PARAMETER, MAX_ROOTPARAM> rootparam;		// ルートパラメーター
	array<CD3DX12_DESCRIPTOR_RANGE, MAX_ROOTPARAM> descRange;	// ディスクリプタテーブル
	ComPtr<ID3D12RootSignature> rootsignature;					// ルートシグネチャ
	UINT rootparamCount;										// ルートパラメーター数


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	RayRootsignature() { rootparamCount = 0; }

	// ルートパラメーター追加処理
	void AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT registerSpace = 0);

	// ルートシグネチャの生成
	void Create(const bool& isLocal, const wchar_t* name = nullptr);

	// アクセッタ
	inline ComPtr<ID3D12RootSignature>& GetRootSig() { return rootsignature; }

};