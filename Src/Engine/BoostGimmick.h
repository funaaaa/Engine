#pragma once
#include "BaseGimmick.h"

class BoostGimmick : public BaseGimmick {

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	BoostGimmick() {};

	// 初期化処理
	void Setting(const ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID) override;

	// 更新処理
	void Update() override;

};