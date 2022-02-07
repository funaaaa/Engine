#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> tex2 : register(t1); //1番スロットに設定されたテクスチャ
Texture2D<float4> tex3 : register(t2); //2番スロットに設定されたテクスチャ
Texture2D<float4> tex4 : register(t3); //3番スロットに設定されたテクスチャ
Texture2D<float4> tex5 : register(t4); //4番スロットに設定されたテクスチャ
Texture2D<float4> tex6 : register(t5); //5番スロットに設定されたテクスチャ
Texture2D<float4> tex7 : register(t6); //6番スロットに設定されたテクスチャ
Texture2D<float4> tex8 : register(t7); //7番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);

    float4 combineColor = tex.Sample(smp, input.uv);
    combineColor += tex2.Sample(smp, input.uv);
    combineColor += tex3.Sample(smp, input.uv);
    combineColor += tex4.Sample(smp, input.uv);
    combineColor += tex5.Sample(smp, input.uv);
    combineColor += tex6.Sample(smp, input.uv);
    combineColor += tex7.Sample(smp, input.uv);
    combineColor += tex8.Sample(smp, input.uv);
    combineColor.x /= 8.0f;
    combineColor.y /= 8.0f;
    combineColor.z /= 8.0f;
    //combineColor.a = 1.0f;

    saturate(combineColor);

    //if (combineColor.a <= 0.0)
    //    discard;

    return combineColor;
}