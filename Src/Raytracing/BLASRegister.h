#pragma once
#include <array>
#include <memory>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "Singleton.h"
#include "ModelDataManager.h"

class BLAS;

class BLASRegister : public Singleton<BLASRegister> {

private:

	/*===== メンバ変数 =====*/

	static const int BLAS_COUNT = 256;
	std::array<std::shared_ptr<BLAS>, BLAS_COUNT> blas_;	// 加速構造体


public:

	/*===== メンバ関数 =====*/

	// BLASを準備。
	void Setting();

	// 生成処理
	std::weak_ptr<BLAS> GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	std::weak_ptr<BLAS> GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	std::weak_ptr<BLAS> GenerateGLTF(const std::wstring& Path, const std::wstring& HitGroupName, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	
	// シェーダーレコードを書き込む。
	uint8_t* WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);

	// BLASの数を取得。
	int GetBLASCount() { return static_cast<int>(blas_.size()); }

	// BLASを取得。
	std::array<std::shared_ptr<BLAS>, BLAS_COUNT>& GetBLAS() { return blas_; }
	std::weak_ptr<BLAS> GetBlasSpecificationIndex(const int& Index) { return blas_[Index]; }

};