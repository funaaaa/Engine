#pragma once
#include <memory>
#include <vector>
#include <Vec.h>
#include "Singleton.h"

class ShellObject;

class ShellObjectMgr : public Singleton<ShellObjectMgr> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<ShellObject>> carapace_;


public:

	/*===== �����o�֐� =====*/

	void Setting();
	void AddObject(const Vec3& Pos, const Vec3& ForwardVec);
	void Update();

};