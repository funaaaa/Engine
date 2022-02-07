#include "SpriteShaderHeader.hlsli"

Texture2D<float4> bokeTexture : register(t0); //0番スロットに設定されてテクスチャ
Texture2D<float4> depthTexture : register(t1); //1番スロットに設定されてテクスチャ

SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 CaluFog(float distance, float4 color, float4 fogColor, float density)
{
    float e = 2.71828f;
    float fogFactor = pow(e, -distance * density);
    //フォグ量
    fogFactor = saturate(fogFactor);
    
    //フォグの色  
    float4 fog = fogFactor * color + (1 - fogFactor) * fogColor;
    
    return fog;
}

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
   
    //カメラ空間での深度値をサンプリング
    float4 depth = depthTexture.Sample(smp, input.uv);
    
    //カメラ空間での深度値が200以下ならピクセルキル
    //clip(depth - 200.0f);
    
    //ボケ画像をサンプリング
    float4 boke = bokeTexture.Sample(smp, input.uv);
    
    //深度値から不透明度を計算する
    //深度値800からボケが始まり、深度値2000で最大のボケ具合になる
    //  →つまり深度値2000で不透明度が1になる
    //boke.a = min(1.0f, (depth - 200.0f) / 500.0f);
    
    //2000 ~ 3000の間でフォグを行う
    //float fogStart = 3000.0f;
    //float fogEnd = 15000.0f;
    //アルファ値を計算する
    //float buff = smoothstep(fogStart, fogEnd, depth);
    //boke.r += buff;
    //boke.g -= buff;
    //boke.b -= buff;
    //boke.a = 1.0f - buff;
    
    float4 fogColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float mitudo = 0.0004;
    float4 fog = CaluFog(depth.x, boke, fogColor, mitudo);
    
    //ボケ画像を出力
    return fog;
}