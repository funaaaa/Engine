#pragma once
#include <string>
#include <vector>
#include <wtypes.h>
#include "PolygonInstanceRegister.h"
#include "OBB.h"

class BaseGimmick {

public:

	/*===== メンバ変数 =====*/

	// ギミックのID
	enum class ID {

		BOOST,		// 加速ギミック

	};

protected:

	ID gimmickID;	// ギミックのID
	int blasIndex;	// オブジェクトのBLASのインデックス
	int insIndex;	// オブジェクトのINSTANCEのインデックス
	bool isActive;	// 有効化フラグ
	OBB collider;	// 当たり判定用OBB


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	virtual void Setting(const ID& GimmickID, const UINT& InstanceID, const UINT& ShaderID) = 0;

	// 更新処理
	virtual void Update() = 0;

	// 有効化。
	inline void Activate() { isActive = true; }
	// 無効化。
	inline void Invalidate() { isActive = false; }

	// ゲッタ
	inline const int& GetBLASIndex() { return blasIndex; }
	inline const int& GetINSTANCEIndex() { return insIndex; }

};