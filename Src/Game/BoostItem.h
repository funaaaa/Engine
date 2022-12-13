#pragma once
#include "BaseItem.h"
#include "PolygonInstance.h"

class PolygonMeshInstance;

// 加速アイテム
class BoostItem : public BaseItem {

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	BoostItem();
	void Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance)override;
	void Update()override;
	int Use(float CharaRotY = 0, int ParamID = 0)override;

};