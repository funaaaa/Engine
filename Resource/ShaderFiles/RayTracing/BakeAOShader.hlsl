
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// グローバルルートシグネチャ
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneCB> gSceneParam : register(b0);

// ヒットグループのローカルルートシグネチャ
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> debugTexImg : register(t2, space1);
Texture2D<float4> debugTexImg2 : register(t3, space1); // 使ってない
RWTexture2D<float4> gOutput : register(u0, space1);
// サンプラー
SamplerState smp : register(s0, space1);

// RayGenerationシェーダーのローカルルートシグネチャ
RWTexture2D<float4> debugTex : register(u0);

// 当たった位置の情報を取得する関数
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer)
{
    Vertex v = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attrib.barys);
    uint vertexId = PrimitiveIndex() * 3; // Triangle List のため.

    float weights[3] =
    {
        barycentrics.x, barycentrics.y, barycentrics.z
    };

    for (int i = 0; i < 3; ++i)
    {
        uint index = indexBuffer[vertexId + i];
        float w = weights[i];
        v.Position += vertexBuffer[index].Position * w;
        v.Normal += vertexBuffer[index].Normal * w;
        v.uv += vertexBuffer[index].uv * w;
    }
    v.Normal = normalize(v.Normal);

    return v;
}

// 反射させる。
void Reflection(float3 vertexPosition, float3 vertexNormal, inout Payload payload)
{
    float3 worldPos = mul(float4(vertexPosition, 1), ObjectToWorld4x3());
    float3 worldNormal = mul(vertexNormal, (float3x3) ObjectToWorld4x3());
    float3 worldRayDir = WorldRayDirection();
    float3 reflectDir = reflect(worldRayDir, worldNormal);
    
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
void mainRayGen()
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
    Payload payload;
    payload.color = float3(0, 0, 0);
    payload.recursive = 0;

    // TransRayに必要な設定を作成
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    //flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderを無視。

    // レイを発射
    TraceRay(
    gRtScene, // TLAS
    flag, // 衝突判定制御をするフラグ
    rayMask, // 衝突判定対象のマスク値
    0, // ray index
    1, // MultiplierForGeometryContrib
    0, // miss index
    rayDesc,
    payload);
    
    
    // レイを発射した結果の色を取得
    float3 col = payload.color;

    debugTex[launchIndex.xy] = float4(col, 1);

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload payload)
{

    // 単色を返すようにする。
    //payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);
    //payload.color = float3(0x32 / 255.0f, 0x90 / 255.0f, 0xD0 / 255.0f);

}

// シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout ShadowPayload payload)
{
    // 何にも当たっていないということなので、影は生成しない。
    payload.isShadow = true;
}

// closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]
void mainCHS(inout Payload payload, MyAttribute attrib)
{
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    
    // 呼び出し回数が制限を超えないようにする。
    if (checkRecursiveLimit(payload, 1))
    {
        return; // 呼び出し回数が越えたら即リターン.
    }
    
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
        
        // 隠蔽度合い
    float visibility = 0.0f;
        
    const int SAMPLING_COUNT = 30;
        
        // 飛ばすレイの回数
    for (int index = 0; index < SAMPLING_COUNT; ++index)
    {
                
            // 乱数を生成してレイを飛ばす方向を決める。
        float randSeedX = (frac(sin(dot(vtx.Position.xy + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
        float randSeedY = (frac(sin(dot(vtx.Position.xz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(78.233, 12.9898)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
        float randSeedZ = (frac(sin(dot(vtx.Position.yz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(32.9898, 48.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
        randSeedX = randSeedX * 2.0f - 1.0f;
        randSeedY = randSeedY * 2.0f - 1.0f;
        randSeedZ = randSeedZ * 2.0f - 1.0f;
        float3 sampleDir = float3(randSeedX, randSeedY, randSeedZ);
            
            // シャドウレイを飛ばす。
        float smpleVisiblity = ShootAOShadowRay(vtx.Position, sampleDir, 500, gRtScene);
            
            // 隠蔽度合い += サンプリングした値 * コサイン項 / 確率密度関数
        float nol = saturate(dot(vtx.Normal, sampleDir));
        float pdf = 1.0 / (2.0 * PI);
        visibility += smpleVisiblity * nol / pdf;
                
    }
        // 平均を取る。
    visibility = (1.0f / PI) * (1.0f / float(SAMPLING_COUNT)) * visibility;
        
        // 隠蔽度合いが限界を超えないようにする。
    visibility = saturate(visibility);

        // 最終結果の色を保存。
    float3 texColor = debugTexImg.SampleLevel(smp, vtx.uv, 0.0f).xyz;
    payload.color.xyz = float3(visibility, visibility, visibility) * texColor;
    
    uint uavSizeX = 0;
    uint uavSizeY = 0;
    gOutput.GetDimensions(uavSizeX, uavSizeY);
    gOutput[uint2(uavSizeY * vtx.uv.y, uavSizeX * vtx.uv.x)] = float4(float3(visibility, visibility, visibility), 1);
        
    Reflection(vtx.Position, vtx.Normal, payload);

}

// closesthitシェーダー シャドウ用
[shader("closesthit")]
void shadowCHS(inout ShadowPayload payload, MyAttribute attrib)
{
}

// アルファ抜きAnyHitShader
[shader("anyhit")]
void mainAnyHit(inout Payload payload, MyAttribute attrib)
{
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    float4 diffuse = 0;
    if (diffuse.w < 0.5f)
    {
        IgnoreHit();

    }
    
    int instanceID = InstanceID();
    
    // インスタンスIDが3(ライト)なら当たり判定を棄却する。
    if (instanceID == 3)
    {
        IgnoreHit();

    }
    
}
