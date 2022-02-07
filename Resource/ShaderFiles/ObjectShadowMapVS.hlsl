#include "ObjectShadowMapShaderHeader.hlsli"

// 影が落とされる対象の頂点シェーダー
VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{

    VSOutput output;
    output.svpos = mul(mul(viewproj, world), pos); //座標に行列を乗算

    output.worldpos = mul(world, pos);

    float4 inormal = float4(normal, 0);
	//output.normal = inormal;
    output.normal = normalize(mul(world, inormal).rgb); //法線にもワールド変換を行う
    output.uv = uv;

    output.svposInLVP = mul(lightViewProj, output.worldpos);

    return output;

}