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
	int GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing = false, const bool& IsOpaque = true, const bool& IsNewGenerate = false);
	int GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath);
	int GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque);

	// BLASの更新
	void Update(const int& Index);

	// テクスチャを変更。
	void ChangeTex(const int& BlasIndex, const int& TexIndex, const int& TexHandle);

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
	std::array<std::shared_ptr<BLAS>, BLAS_COUNT>& GetBLAS() { return blas_; }

	// すべての頂点情報にVec3情報をかける。 重い処理なのでたくさん呼ばない。
	void MulVec3Vertex(const int& Index, Vec3 Vec);

	// 頂点の最大最小の値を取得。
	const Vec3& GetVertexMin(const int& Index);
	const Vec3& GetVertexMax(const int& Index);
	Vec3 GetVertexLengthMax(const int& Index);

	// メッシュの情報を取得。
	const std::vector<Vec3>& GetVertex(const int& Index);
	const std::vector<Vec3>& GetNormal(const int& Index);
	const std::vector<Vec2>& GetUV(const int& Index);
	const std::vector<UINT>& GetVertexIndex(const int& Index);

	// 指定の要素を削除。
	void DeleteIndex(const int& Index);

};