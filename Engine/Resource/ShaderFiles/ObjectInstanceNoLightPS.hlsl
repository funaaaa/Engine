#include "ObjectInstanceShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(1,1,1,1);

	//テクスチャの色
    float4 texcolor = tex.Sample(smp, input.uv);
    return texcolor * cbuff0Data[input.instanceID].color;

}