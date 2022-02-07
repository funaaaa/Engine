#include "MultiPathShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
    float4 texcolor = tex.Sample(smp, input.uv.tex0.xy) * color;
    
    if (texcolor.a <= 0.0)
        discard;
    
    //2.5�e�N�Z�����炷���߂�UV�l�����߂�
    float offsetU = 1.5f / 1280.0f;
    float offsetV = 1.5f / 720.0f;
    
    //��e�N�Z������E�̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(offsetU, 0.0f));
    
    //��e�N�Z�����獶�̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(-offsetU, 0.0f));
    
    //��e�N�Z�����牺�̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(0.0f, offsetV));
    
    //��e�N�Z�������̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(0.0f, -offsetV));
    
    //��e�N�Z������E���̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(offsetU, offsetV));
    
    //��e�N�Z������E��̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(offsetU, -offsetV));
    
    //��e�N�Z�����獶���̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(-offsetU, offsetV));
    
    //��e�N�Z�����獶��̃e�N�Z���̃J���[���T���v�����O����
    texcolor += tex.Sample(smp, input.uv.tex0.xy + float2(-offsetU, -offsetV));
    
    //��e�N�Z���ƋߖT8�e�N�Z���̕��ςȂ̂�9�ŏ��Z����
    texcolor /= 9.0f;
    
    return texcolor;
}