#include "ObjectInstanceShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
    VSOutput output;
    output.instanceID = instanceID;
    output.svpos = mul(mul(cbuff0Data[output.instanceID].viewproj, cbuff0Data[output.instanceID].world), pos); //座標に行列を乗算

    output.worldpos = mul(cbuff0Data[output.instanceID].world, pos);

    float4 inormal = float4(normal, 0);
	//output.normal = inormal;
    output.normal = normalize(mul(cbuff0Data[output.instanceID].world, inormal).rgb); //法線にもワールド変換を行う
    output.uv = uv;
    return output;
}