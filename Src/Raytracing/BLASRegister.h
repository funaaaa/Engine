#pragma once
#include <vector>
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

	std::vector<std::shared_ptr<BLAS>> blas;	// 加速構造体


public:

	/*===== メンバ関数 =====*/

	// 生成処理
	int GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	int GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	int GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLASの更新
	void Update(const int& Index);

	// スキニングを計算
	void ComputeSkin(const int& Index);

	// アニメーションの有効化
	void InitAnimation(const int& Index);	// 初期化
	void PlayAnimation(const int& Index);	// 再生
	void StopAnimation(const int& Index);	// 停止

	// シェーダーレコードを書き込む。
	uint8_t* WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName);
	uint8_t* WriteShaderRecordSpecifyUAV(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName, const int& SpecifyIndex);

	// バッファを取得。
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer(const int& Index);

	// BLASの数を取得。
	int GetBLASCount() { return blas.size(); }

	// BLASを取得。
	std::vector<std::shared_ptr<BLAS>>& GetBLAS() { return blas; }

};