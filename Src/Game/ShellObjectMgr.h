#pragma once
#include <memory>
#include <vector>
#include <Vec.h>
#include "Singleton.h"

class ShellObject;

class ShellObjectMgr : public Singleton<ShellObjectMgr> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<ShellObject>> carapace_;


public:

	/*===== メンバ関数 =====*/

	void Setting();
	void AddObject(const Vec3& Pos, const Vec3& ForwardVec);
	void Update();

};