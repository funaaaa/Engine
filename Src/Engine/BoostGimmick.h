#pragma once
#include "BaseGimmick.h"
#include "ModelDataManager.h"

class BoostGimmick : public BaseGimmick {

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	BoostGimmick() {};

	// 初期化処理
	void Setting(const ID& GimmickID, const UINT& InstanceID) override;

	// 更新処理
	void Update() override;

private:

	// OBB用のVertexを生成。
	ModelDataManager::ObjectData GenerateVertex(OBB Obb);

};