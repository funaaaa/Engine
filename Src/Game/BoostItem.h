#pragma once
#include "BaseItem.h"
#include "PolygonInstance.h"

class PolygonMeshInstance;

// �����A�C�e��
class BoostItem : public BaseItem {

private:

	/*===== �����o�ϐ� =====*/


public:

	/*===== �����o�֐� =====*/

	BoostItem();
	void Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance)override;
	void Update()override;
	int Use(float CharaRotY = 0, int ParamID = 0)override;

};