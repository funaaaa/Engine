#pragma once
#include "BaseStageObject.h"

// ��{�I��StageObject�N���X
class BasicStageObject : public BaseStageObject{

private:

	/*===== �����o�ϐ� =====*/


public:

	/*===== �����o�֐� =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)override;
	void Destroy()override;
	void Update(const int& Timer)override;

};