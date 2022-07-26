#pragma once
#include <memory>
#include <vector>
#include <Vec.h>
#include "Singleton.h"

class ShellObject;

class ShellObjectMgr : public Singleton<ShellObjectMgr> {

private:

	/*===== ƒƒ“ƒo•Ï” =====*/

	std::vector<std::shared_ptr<ShellObject>> carapace_;


public:

	/*===== ƒƒ“ƒoŠÖ” =====*/

	void Setting();
	void AddObject(const Vec3& Pos, const Vec3& ForwardVec);
	void Update();

};