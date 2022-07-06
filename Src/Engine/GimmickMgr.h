#pragma once
#include <vector>
#include <memory>
#include "BaseGimmick.h"
#include "Singleton.h"

class GimmickMgr : public Singleton<GimmickMgr> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<BaseGimmick>> gimmicks;


public:

	/*===== �����o�ϐ� =====*/

	// �ǉ�����B
	void AddGimmick(const BaseGimmick::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);

	// �M�~�b�N�W��ǉ��B
	std::vector<std::shared_ptr<BaseGimmick>> GetGimmickData() { return gimmicks; }

};
