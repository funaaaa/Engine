#include "ObjectGetShadowMapShaderHeader.hlsli"

float4 main(VSOutput input) : SV_TARGET
{

    // �V���h�E�}�b�v��Z�l���������ށB
    return float4(input.svpos.z, input.svpos.z, input.svpos.z, 1.0f);

}