#include "ObjectShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VSOutput output;
    
    float expantionAmount = 0.1f;
    
    //ビュー行列を掛ける
    float4 viewPos = mul(viewproj, pos);
    
    //距離の割合を求める
    float rate = saturate(viewPos.a / viewPos.w);
    
    //もとの大きさより大きくするためのスケール行列を生成する
    float4x4 scalemat;
    scalemat[0] = float4(1.0f + (abs(normal.r) * (rate * expantionAmount)), 0, 0, 0);
    scalemat[1] = float4(0, 1.0f + (abs(normal.g) * (rate * expantionAmount)), 0, 0);
    scalemat[2] = float4(0, 0, 1.0f + (abs(normal.b) * (rate * expantionAmount)), 0);
    scalemat[3] = float4(0, 0, 0, 1.0f);
    
    output.svpos = mul(mul(viewproj, mul(world, scalemat)), pos); //座標に行列を乗算

    output.worldpos = mul(world, pos);

    float4 inormal = float4(normal, 0);
	//output.normal = inormal;
    output.normal = normalize(mul(world, inormal).rgb); //法線にもワールド変換を行う
    output.uv = uv;
    return output;
}