#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
Texture2D<float4> tex2 : register(t1); //1番スロットに設定されてテクスチャ
Texture2D<float4> tex3 : register(t2); //2番スロットに設定されてテクスチャ
Texture2D<float4> tex4 : register(t3); //3番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
   
    float4 combineColor = tex.Sample(smp, input.uv);
    combineColor += tex2.Sample(smp, input.uv);
    combineColor += tex3.Sample(smp, input.uv);
    combineColor += tex4.Sample(smp, input.uv);
    combineColor /= 4.0f;
    return combineColor;
}