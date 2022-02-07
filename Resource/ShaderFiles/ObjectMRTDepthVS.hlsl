#include "ObjectMRTShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VSOutput output;
    output.svpos = mul(mul(viewproj, world), pos); //座標に行列を乗算

    output.worldpos = mul(world, pos);

    float4 inormal = float4(normal, 0);
	//output.normal = inormal;
    output.normal = normalize(mul(world, inormal).rgb); //法線にもワールド変換を行う
    output.uv = uv;
    
    //深度を保存
    output.depth = output.svpos.z;
    
    return output;
}