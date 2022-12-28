#pragma once
#include <array>
#include <d3d12.h>
#include <wrl.h>

// D3D12のリソースクラスをラップしたクラス
class DoubleResourceWrapper {

private:

	/*===== メンバ変数 =====*/

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> resource_;		// リソース本体
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> uploadResource_;	// アップロード用リソース
	std::array<void*, 2> mapAddress_;										// GPUにデータを転送する際のアドレス
	size_t bufferSize_;														// バッファのサイズ


public:

	/*===== メンバ関数 =====*/

	// バッファを生成
	void CreateBuffer(size_t Size, void* Data, const wchar_t* Name);

	// データを書き込む
	void WriteData(int Index, size_t Size, void* Data);

	// バッファを取得
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(int Index) { return resource_[Index]; }

};