#pragma once
#include "Singleton.h"
#include <wrl.h>

// InstanceID���Ǘ�����N���X
class InstanceIDMgr : public Singleton<InstanceIDMgr> {

private:

	/*===== �����o�ϐ� =====*/

	UINT instanceID = 0;		// �C���X�^���XID


public:

	/*===== �����o�֐� =====*/

	// InstanceID���擾����֐��B
	inline const UINT& GetInstanceID() {

		++instanceID;
		return instanceID - 1;

	}

};