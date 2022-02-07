#include "ObjectShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VSOutput output;
    
    float expantionAmount = 0.1f;
    
    //�r���[�s����|����
    float4 viewPos = mul(viewproj, pos);
    
    //�����̊��������߂�
    float rate = saturate(viewPos.a / viewPos.w);
    
    //���Ƃ̑傫�����傫�����邽�߂̃X�P�[���s��𐶐�����
    float4x4 scalemat;
    scalemat[0] = float4(1.0f + (abs(normal.r) * (rate * expantionAmount)), 0, 0, 0);
    scalemat[1] = float4(0, 1.0f + (abs(normal.g) * (rate * expantionAmount)), 0, 0);
    scalemat[2] = float4(0, 0, 1.0f + (abs(normal.b) * (rate * expantionAmount)), 0);
    scalemat[3] = float4(0, 0, 0, 1.0f);
    
    output.svpos = mul(mul(viewproj, mul(world, scalemat)), pos); //���W�ɍs�����Z

    output.worldpos = mul(world, pos);

    float4 inormal = float4(normal, 0);
	//output.normal = inormal;
    output.normal = normalize(mul(world, inormal).rgb); //�@���ɂ����[���h�ϊ����s��
    output.uv = uv;
    return output;
}