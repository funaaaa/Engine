#include "GSINstanceShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, uint instanceID : SV_InstanceID)
{
    VSOutput output;
    output.pos = pos; //���W�ɍs�����Z
    output.id = instanceID;
    return output;
}