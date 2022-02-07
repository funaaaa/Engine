#include "ObjectGetShadowMapShaderHeader.hlsli"

float4 main(VSOutput input) : SV_TARGET
{

    // シャドウマップにZ値を書き込む。
    return float4(input.svpos.z, input.svpos.z, input.svpos.z, 1.0f);

}