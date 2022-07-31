#pragma once
#include <memory>
#include <array>
#include <Vec.h>
#include "Singleton.h"

class ShellObject;
class StageData;
class BaseStage;

class ShellObjectMgr : public Singleton<ShellObjectMgr> {

private:

	/*===== ƒƒ“ƒo•Ï” =====*/

	static const int SHELL_COUNT = 30;
	std::array<std::shared_ptr<ShellObject>, SHELL_COUNT> shell_;


public:

	/*===== ƒƒ“ƒoŠÖ” =====*/

	void Setting();
	int AddObject(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, const int& CharaInsIndex);
	void DestroyObject(const int& Index);
	void Update(std::weak_ptr<BaseStage> StageData);

};