#include "ObjectInstanceShaderHeader.hlsli"


VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
    VSOutput output;
    output.instanceID = instanceID;
    
    //���Ƃ̑傫�����傫�����邽�߂̃X�P�[���s��𐶐�����
    column_major float4x4 scalemat;
    scalemat[0] = float4(1.1f, 0, 0, 0);
    scalemat[1] = float4(0, 1.1f, 0, 0);
    scalemat[2] = float4(0, 0, 1.1f, 0);
    scalemat[3] = float4(0, 0, 0, 1.0f);
    
    float4 svpos = mul(mul(cbuff0Data[output.instanceID].viewproj, mul(cbuff0Data[output.instanceID].world, scalemat)), pos); //���W�ɍs�����Z
    
    output.svpos = svpos;

    output.worldpos = mul(cbuff0Data[output.instanceID].world, pos);
    output.uv = uv;
    return output;
}