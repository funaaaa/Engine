#pragma once
#include "Singleton.h"
#include <wrl.h>

// InstanceIDを管理するクラス
class InstanceIDMgr : public Singleton<InstanceIDMgr> {

private:

	/*===== メンバ変数 =====*/

	UINT instanceID = 0;		// インスタンスID


public:

	/*===== メンバ関数 =====*/

	// InstanceIDを取得する関数。
	inline const UINT& GetInstanceID() {

		++instanceID;
		return instanceID - 1;

	}

};