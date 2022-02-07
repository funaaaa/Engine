#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv) * color;
    
    //if (texcolor.a <= 0.0)
    //    discard;
    
    //return float4(1, 1, 1, 1);
    
    //サンプリングしたカラーの明るさを計算
    float t = dot(texcolor.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    
    //ピクセルのカラーが1以下ならピクセルキルする
    clip(t - 0.1f);
    
    return texcolor;
}