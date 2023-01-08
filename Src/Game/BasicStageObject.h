#pragma once
#include "BaseStageObject.h"

// ��{�I��StageObject�N���X
class BasicStageObject : public BaseStageObject{

private:

	/*===== �����o�ϐ� =====*/


public:

	/*===== �����o�֐� =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)override;
	void Destroy()override;
	void Update(int Timer, std::weak_ptr<CharacterMgr> Character)override;
	void Disable(int TimerToActivation, int CharaIndex)override;

};