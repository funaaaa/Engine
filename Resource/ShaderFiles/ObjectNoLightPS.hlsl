#include "ObjectShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//テクスチャの色
    float4 texcolor = tex.Sample(smp, input.uv) * color;

    if (texcolor.a <= 0.0) discard;
    return texcolor;
}