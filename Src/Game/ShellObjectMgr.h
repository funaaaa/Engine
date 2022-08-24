#pragma once
#include <memory>
#include <array>
#include <Vec.h>
#include "Singleton.h"

class ShellObject;
class StageData;
class BaseStage;
class OBB;

class ShellObjectMgr : public Singleton<ShellObjectMgr> {

private:

	/*===== �����o�ϐ� =====*/

	static const int SHELL_COUNT = 100;
	std::array<std::shared_ptr<ShellObject>, SHELL_COUNT> shell_;

public:


public:

	/*===== �����o�֐� =====*/

	void Setting();
	int AddObject(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, const int& CharaInsIndex);
	void DestroyObject(const int& Index);
	void Update(std::weak_ptr<BaseStage> StageData);

	// �����蔻��
	bool Collider(std::weak_ptr<OBB> CharaOBB);

};