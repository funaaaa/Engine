#pragma once
#include <vector>
#include <memory>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "Singleton.h"
#include "Struct.h"

class BLAS;

class BLASRegister : public Singleton<BLASRegister> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<BLAS>> blas;	// 加速構造体


public:

	/*===== メンバ関数 =====*/

	// 生成処理
	const int& GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath);
	const int& GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath);
	const int& GenerateData(Object3DDeliveryData Data, const wstring& HitGroupName, const int& TextureHandle);

	// BLASの更新
	void Update(const int& Index);

	// スキニングを計算
	void ComputeSkin(const int& Index);

	// アニメーションの有効化
	void InitAnimation(const int& Index);	// 初期化
	void PlayAnimation(const int& Index);	// 再生
	void StopAnimation(const int& Index);	// 停止

	// シェーダーレコードを書き込む。
	uint8_t* WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject);

	// バッファを取得。
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer(const int& Index);

	// BLASの数を取得。
	const int& GetBLASCount() { return blas.size(); }

};