#pragma once
#include "BaseItem.h"

class ShellItem : public BaseItem {

private:

	/*===== メンバ変数 =====*/

	int behindShellIndex_;	// 後ろに保持された甲羅のインデックス

public:

	enum class PARAM_ID {

		BEHIND,				// 後ろに保持
		FORWARD_THROW,		// 前に投げる
		BEHIND_THROW,		// 後ろに保持

	};


public:

	/*===== メンバ関数 =====*/

	ShellItem();
	void Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance)override;
	void Update()override;
	void Use(const float& CharaRotY = 0, const int ParamID = 0)override;

};