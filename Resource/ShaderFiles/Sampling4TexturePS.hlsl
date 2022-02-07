#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
Texture2D<float4> tex2 : register(t1); //1�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
Texture2D<float4> tex3 : register(t2); //2�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
Texture2D<float4> tex4 : register(t3); //3�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

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