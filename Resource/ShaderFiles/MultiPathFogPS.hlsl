#include "SpriteShaderHeader.hlsli"

Texture2D<float4> bokeTexture : register(t0); //0�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
Texture2D<float4> depthTexture : register(t1); //1�ԃX���b�g�ɐݒ肳��ăe�N�X�`��

SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 CaluFog(float distance, float4 color, float4 fogColor, float density)
{
    float e = 2.71828f;
    float fogFactor = pow(e, -distance * density);
    //�t�H�O��
    fogFactor = saturate(fogFactor);
    
    //�t�H�O�̐F  
    float4 fog = fogFactor * color + (1 - fogFactor) * fogColor;
    
    return fog;
}

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
   
    //�J������Ԃł̐[�x�l���T���v�����O
    float4 depth = depthTexture.Sample(smp, input.uv);
    
    //�J������Ԃł̐[�x�l��200�ȉ��Ȃ�s�N�Z���L��
    //clip(depth - 200.0f);
    
    //�{�P�摜���T���v�����O
    float4 boke = bokeTexture.Sample(smp, input.uv);
    
    //�[�x�l����s�����x���v�Z����
    //�[�x�l800����{�P���n�܂�A�[�x�l2000�ōő�̃{�P��ɂȂ�
    //  ���܂�[�x�l2000�ŕs�����x��1�ɂȂ�
    //boke.a = min(1.0f, (depth - 200.0f) / 500.0f);
    
    //2000 ~ 3000�̊ԂŃt�H�O���s��
    //float fogStart = 3000.0f;
    //float fogEnd = 15000.0f;
    //�A���t�@�l���v�Z����
    //float buff = smoothstep(fogStart, fogEnd, depth);
    //boke.r += buff;
    //boke.g -= buff;
    //boke.b -= buff;
    //boke.a = 1.0f - buff;
    
    float4 fogColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float mitudo = 0.0004;
    float4 fog = CaluFog(depth.x, boke, fogColor, mitudo);
    
    //�{�P�摜���o��
    return fog;
}