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

	std::vector<std::shared_ptr<BLAS>> blas_;	// 加速構造体


public:

	/*===== メンバ関数 =====*/

	// 生成処理
	int GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing = false, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
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

	// バッファを取得。
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBLASBuffer(const int& Index);

	// BLASの数を取得。
	int GetBLASCount() { return static_cast<int>(blas_.size()); }

	// BLASを取得。
	std::vector<std::shared_ptr<BLAS>>& GetBLAS() { return blas_; }

	// すべての頂点情報にVec3情報をかける。 重い処理なのでたくさん呼ばない。
	void MulVec3Vertex(const int& Index, Vec3 Vec);

	// 頂点の最大最小の値を取得。
	const Vec3& GetVertexMin(const int& Index);
	const Vec3& GetVertexMax(const int& Index);
	Vec3 GetVertexLengthMax(const int& Index);

};