#include "PorygonInstanceRegister.h"
#include <assert.h>

UINT PorygonInstanceRegister::SetRegister(D3D12_RAYTRACING_INSTANCE_DESC instanceDesc)
{

	/*===== InstanceDescを保存する処理 =====*/

	// InstanceDescを追加する。
	instanceDescRegister.push_back(instanceDesc);

	// 最後尾を返す。
	return instanceDescRegister.size() - 1;

}

void PorygonInstanceRegister::AddTrans(const XMMATRIX& transMat, const UINT& id)
{

	/*===== 移動処理 =====*/

	// 設定の移動行列を更新する。
	XMStoreFloat3x4(
		reinterpret_cast<XMFLOAT3X4*>(&instanceDescRegister[id].Transform),
		transMat);

}
