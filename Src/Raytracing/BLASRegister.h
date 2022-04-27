#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Singleton.h"

class BLAS;

class BLASRegister : public Singleton<BLASRegister> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<BLAS>> blas;


public:

	/*===== メンバ関数 =====*/

	// BLASの生成
	int GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName);
	int GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName);

	// BLASの更新
	void Update(const int& Index);

	// スキニングを計算
	void ComputeSkin(const int& Index);

	// アニメーションの有効化
	void InitAnimation(const int& Index);	// 初期化
	void PlayAnimation(const int& Index);	// 再生
	void StopAnimation(const int& Index);	// 停止

	// デバッグ用　すぐ消す
	std::shared_ptr<BLAS> GetBlas(const int& Index) { return blas[Index]; }

};