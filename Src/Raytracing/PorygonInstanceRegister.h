#pragma once
#include "DirectXBase.h"
#include "Singleton.h"

#include <DirectXMath.h>
#include <vector>

using namespace std;
using namespace DirectX;

// ポリゴンインスタンスの参照を保存するクラス
class PorygonInstanceRegister : public Singleton<PorygonInstanceRegister> {

private:

	/*===== メンバ変数 =====*/

	vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescRegister;


public:

	/*===== メンバ関数 =====*/

	// レジスターにセットする関数。
	UINT SetRegister(D3D12_RAYTRACING_INSTANCE_DESC instanceDesc);

	// 移動行列を更新。
	void AddTrans(const XMMATRIX& transMat, const UINT& id);

	// レジスターのDataを取得する関数。
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDescRegister.data(); }

	// レジスターのサイズを取得する関数。
	inline const UINT& GetRegisterSize() { return UINT(instanceDescRegister.size()); }

};