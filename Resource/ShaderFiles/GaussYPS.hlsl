#include "MultiPathShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
    float4 texcolor;
    
    //基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    texcolor = gaussianWeight[0].x * tex.Sample(smp, input.uv.tex0.xy);
    texcolor += gaussianWeight[0].y * tex.Sample(smp, input.uv.tex1.xy);
    texcolor += gaussianWeight[0].z * tex.Sample(smp, input.uv.tex2.xy);
    texcolor += gaussianWeight[0].w * tex.Sample(smp, input.uv.tex3.xy);
    texcolor += gaussianWeight[1].x * tex.Sample(smp, input.uv.tex4.xy);
    texcolor += gaussianWeight[1].y * tex.Sample(smp, input.uv.tex5.xy);
    texcolor += gaussianWeight[1].z * tex.Sample(smp, input.uv.tex6.xy);
    texcolor += gaussianWeight[1].w * tex.Sample(smp, input.uv.tex7.xy);
    
    //基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    texcolor += gaussianWeight[0].x * tex.Sample(smp, input.uv.tex0.zw);
    texcolor += gaussianWeight[0].y * tex.Sample(smp, input.uv.tex1.zw);
    texcolor += gaussianWeight[0].z * tex.Sample(smp, input.uv.tex2.zw);
    texcolor += gaussianWeight[0].w * tex.Sample(smp, input.uv.tex3.zw);
    texcolor += gaussianWeight[1].x * tex.Sample(smp, input.uv.tex4.zw);
    texcolor += gaussianWeight[1].y * tex.Sample(smp, input.uv.tex5.zw);
    texcolor += gaussianWeight[1].z * tex.Sample(smp, input.uv.tex6.zw);
    texcolor += gaussianWeight[1].w * tex.Sample(smp, input.uv.tex7.zw);
    
    //if (texcolor.a <= 0.0)
    //    discard;
    
    return float4(texcolor.xyz, 1.0f);
}