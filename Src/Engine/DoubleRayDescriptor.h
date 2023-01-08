#pragma once
#include <array>
#include <memory>
#include <wtypes.h>

class RayDescriptor;
class DoubleResourceWrapper;

// RayDescriptorを二重バッファにする用のクラス
class DoubleRayDescriptor {

private:

	/*===== メンバ変数 =====*/

	std::array<std::shared_ptr<RayDescriptor>, 2> rayDescriptor_;

	
public:

	/*===== メンバ関数 =====*/

	// SRVを生成。
	void CreateSRV(std::weak_ptr<DoubleResourceWrapper> Resource, UINT NumElements, UINT FirstElement, UINT Stride);

	// GPUアドレスを取得
	std::weak_ptr<RayDescriptor> GetDescriptor(int Index);

};