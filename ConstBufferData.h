#pragma once
#include <DirectXMath.h>

using namespace DirectX;

// ライトカメラ用定数バッファ
struct LightCameraConstBufferData{
	XMMATRIX lightViewProj;
};