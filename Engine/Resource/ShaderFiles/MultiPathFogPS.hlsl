#include "SpriteShaderHeader.hlsli"

Texture2D<float4> bokeTexture : register(t0); //0番スロットに設定されてテクスチャ
Texture2D<float4> depthTexture : register(t1); //1番スロットに設定されてテクスチャ

SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
   
    //カメラ空間での深度値をサンプリング
    float depth = depthTexture.Sample(smp, input.uv);
    
    //カメラ空間での深度値が200以下ならピクセルキル
    //clip(depth - 200.0f);
    
    //ボケ画像をサンプリング
    float4 boke = bokeTexture.Sample(smp, input.uv);
    
    //深度値から不透明度を計算する
    //深度値800からボケが始まり、深度値2000で最大のボケ具合になる
    //  →つまり深度値2000で不透明度が1になる
    boke.a = min(1.0f, (depth - 200.0f) / 500.0f);
    
    //ボケ画像を出力
    return boke;
}