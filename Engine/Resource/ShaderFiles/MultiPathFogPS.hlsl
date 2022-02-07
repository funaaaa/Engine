#include "SpriteShaderHeader.hlsli"

Texture2D<float4> bokeTexture : register(t0); //0�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
Texture2D<float4> depthTexture : register(t1); //1�ԃX���b�g�ɐݒ肳��ăe�N�X�`��

SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
   
    //�J������Ԃł̐[�x�l���T���v�����O
    float depth = depthTexture.Sample(smp, input.uv);
    
    //�J������Ԃł̐[�x�l��200�ȉ��Ȃ�s�N�Z���L��
    //clip(depth - 200.0f);
    
    //�{�P�摜���T���v�����O
    float4 boke = bokeTexture.Sample(smp, input.uv);
    
    //�[�x�l����s�����x���v�Z����
    //�[�x�l800����{�P���n�܂�A�[�x�l2000�ōő�̃{�P��ɂȂ�
    //  ���܂�[�x�l2000�ŕs�����x��1�ɂȂ�
    boke.a = min(1.0f, (depth - 200.0f) / 500.0f);
    
    //�{�P�摜���o��
    return boke;
}