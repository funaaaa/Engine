#include "MultiPathShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
    float4 texcolor = tex.Sample(smp, input.uv.tex0.xy) * color;
    
    if (texcolor.a <= 0.0)
        discard;
    
    //2.5テクセルずらすためのUV値を求める
    float offsetU = 1.5f / 1280.0f;
    float offsetV = 1.5f / 720.0f;
    
    //基準テクセルから右のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(offsetU, 0.0f));
    
    //基準テクセルから左のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(-offsetU, 0.0f));
    
    //基準テクセルから下のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(0.0f, offsetV));
    
    //基準テクセルから上のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(0.0f, -offsetV));
    
    //基準テクセルから右下のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(offsetU, offsetV));
    
    //基準テクセルから右上のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(offsetU, -offsetV));
    
    //基準テクセルから左下のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(-offsetU, offsetV));
    
    //基準テクセルから左上のテクセルのカラーをサンプリングする
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(-offsetU, -offsetV));
    
    //基準テクセルと近傍8テクセルの平均なので9で除算する
    texcolor /= 9.0f;
    
    return texcolor;
}