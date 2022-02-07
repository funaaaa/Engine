#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
Texture2D<float4> tex2 : register(t1); //1番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);

    float4 combineColor = tex.Sample(smp, input.uv);
    combineColor += tex2.Sample(smp, input.uv);
    //combineColor.x /= 8.0f;
    //combineColor.y /= 8.0f;
    //combineColor.z /= 8.0f;
    //combineColor.a = 1.0f;

    saturate(combineColor);

    //if (combineColor.a <= 0.0)
    //    discard;

    return combineColor;
}