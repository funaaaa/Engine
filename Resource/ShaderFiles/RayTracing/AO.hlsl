
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// グローバルルートシグネチャ
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneAOCB> gSceneParam : register(b0);

// ヒットグループのローカルルートシグネチャ
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> texture : register(t2, space1);
// サンプラー
SamplerState smp : register(s0, space1);

// RayGenerationシェーダーのローカルルートシグネチャ
RWTexture2D<float4> gOutput : register(u0);

struct AOPayload
{
    float3 color;
    float3 light;
    uint recursive;
};

float GetRandomNumber(float2 texCoord, int Seed)
{
    return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

// 反射
void Reflection(inout AOPayload payload, Vertex vtx)
{
    // ベクトルをランダムで求めて反射させる。
    float3 random = float3(GetRandomNumber(float2(vtx.Position.x, vtx.Position.y), int(gSceneParam.seed.x + gSceneParam.seed.y)) * 2.0f - 1.0f,
    GetRandomNumber(float2(vtx.Position.x, vtx.Position.y), int(gSceneParam.seed.x + gSceneParam.seed.y)) * 2.0f - 1.0f,
    GetRandomNumber(float2(vtx.Position.x, vtx.Position.y), int(gSceneParam.seed.x + gSceneParam.seed.y)) * 2.0f - 1.0f);
    
    // -1 ~ 1 の乱数が求められたので、値を小さくする。
    random /= 10.0f;
    
    float3 worldRayDir = WorldRayDirection();
    float3 worldNormal = mul(vtx.Normal, (float3x3) ObjectToWorld4x3());
    float3 reflectDir = reflect(worldRayDir, worldNormal);
    reflectDir += random;
    reflectDir = normalize(reflectDir);
    
    // レイを発射。
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    uint rayMask = 0xFF;

    RayDesc rayDesc;
    rayDesc.Origin = worldPos;
    rayDesc.Direction = reflectDir;
    rayDesc.TMin = 0.01;
    rayDesc.TMax = 100000;

    TraceRay(
        gRtScene,
        0,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        payload);
}

// RayGenerationシェーダー
[shader("raygeneration")]
void mainAORayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);

    float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
    float aspect = dims.x / dims.y;

    matrix mtxViewInv = gSceneParam.mtxViewInv;
    matrix mtxProjInv = gSceneParam.mtxProjInv;

    // レイの設定
    RayDesc rayDesc;
    rayDesc.Origin = mul(mtxViewInv, float4(0, 0, 0, 1)).xyz;

    float4 target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
    float3 dir = mul(mtxViewInv, float4(target.xyz, 0)).xyz;

    rayDesc.Direction = normalize(dir);
    rayDesc.TMin = 0;
    rayDesc.TMax = 100000;

    // ペイロードの設定
    AOPayload payload;
    payload.color = float3(0, 0, 0);
    payload.light = float3(0, 0, 0);
    payload.recursive = 0;

    // TransRayに必要な設定を作成
    uint rayMask = 0xFF;

    // レイを発射
    TraceRay(
    gRtScene, // TLAS
    0, // 衝突判定制御をするフラグ
    rayMask, // 衝突判定対象のマスク値
    0, // ray index
    1, // MultiplierForGeometryContrib
    0, // miss index
    rayDesc,
    payload);
    
    // 取得した色を試行回数で割る。
    payload.color /= payload.recursive;

    // レイを発射した結果の色を取得
    float3 col = payload.color;

    // 結果格納
    gOutput[launchIndex.xy] = float4(col, 1);

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainAOMS(inout AOPayload payload)
{

    // 単色を返すようにする。
    //payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);
    //payload.color = float3(0x32 / 255.0f, 0x90 / 255.0f, 0xD0 / 255.0f); // 空の色
    //payload.color = float3(0x01 / 255.0f, 0x01 / 255.0f, 0x01 / 255.0f);

}

// closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]
void mainAOCHS(inout AOPayload payload, MyAttribute attrib)
{

    // レイの反射数
    const int RECURSICE_RAY_COUNT = 3;

    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();

    // 呼び出し回数が制限を超えないようにする。
    if (RECURSICE_RAY_COUNT < payload.recursive)
    {
        return; // 呼び出し回数が越えたら即リターン.
    }
    
    // 当たったのが0(ライトだったら)
    if (instanceID == 0)
    {
        payload.light = 1.0f;
        return;
    }
    
    // レイの開始地点
    float3 rayStart = WorldRayOrigin() + (WorldRayDirection() * RayTMin());
    // レイの終了地点
    float3 rayEnd = WorldRayOrigin() + (WorldRayDirection() * RayTCurrent());
    // 距離
    float rayLength = length(rayStart - rayEnd);
    // レイの長さの既定値
    const float RAY_LENGTH = 1000;
    // レイの長さが規定値以下だったらリターン
    if (rayLength < RAY_LENGTH)
    {
       // return;
    }
    
    // 呼び出し回数を更新。
    ++payload.recursive;

    // lambert ライティングを行う.
    float3 lightdir = -normalize(gSceneParam.lightDirection.xyz);

    float nl = saturate(dot(vtx.Normal, lightdir));

    float3 lightcolor = gSceneParam.lightColor.xyz;
    float3 ambientcolor = gSceneParam.ambientColor.xyz;
    float4 materialcolor = texture.SampleLevel(smp, vtx.uv, 0.0f);
    float3 color = 0;
    color += lightcolor * materialcolor.xyz * nl;
    color += ambientcolor * materialcolor.xyz;
    
    // ライティングの結果の色を保存。
    payload.color += color;
    
    Reflection(payload, vtx);

}