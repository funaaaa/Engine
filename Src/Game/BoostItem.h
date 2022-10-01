#pragma once
#include "BaseItem.h"
#include "PolygonInstance.h"

class PolygonMeshInstance;

class BoostItem : public BaseItem {

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	BoostItem();
	void Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance)override;
	void Update()override;
	void Use(const float& CharaRotY = 0, const int ParamID = 0)override;

};