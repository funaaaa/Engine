
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// グローバルルートシグネチャ
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneCB> gSceneParam : register(b0);

// ヒットグループのローカルルートシグネチャ
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> texture : register(t2, space1);
// サンプラー
SamplerState smp : register(s0, space1);

// RayGenerationシェーダーのローカルルートシグネチャ
RWTexture2D<float4> gOutput : register(u0);
RWTexture2D<float4> gOutputBuff : register(u1);

// 反射レイトレーシング
float3 Reflection(float3 vertexPosition, float3 vertexNormal, int recursive)
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

    Payload reflectPayload;
    reflectPayload.color = float3(0, 0, 0);
    reflectPayload.recursive = recursive;
    TraceRay(
        gRtScene,
        0,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        reflectPayload);
    return reflectPayload.color;
}

// 屈折レイトレーシング
float3 Refraction(float3 vertexPosition, float3 vertexNormal, int recursive)
{
    float4x3 mtx = ObjectToWorld4x3();
    float3 worldPos = mul(float4(vertexPosition, 1), mtx);
    float3 worldNormal = mul(vertexNormal, (float3x3) mtx);
    float3 worldRayDir = normalize(WorldRayDirection());
    worldNormal = normalize(worldNormal);

    const float refractVal = 1.4;
    float nr = dot(worldNormal, worldRayDir);
    float3 refracted;
    if (nr < 0)
    {
        // 表面. 空気中 -> 屈折媒質.
        float eta = 1.0 / refractVal;
        refracted = refract(worldRayDir, worldNormal, eta);
    }
    else
    {
        // 裏面. 屈折媒質 -> 空気中.
        float eta = refractVal / 1.0;
        refracted = refract(worldRayDir, -worldNormal, eta);
    }

    if (length(refracted) < 0.01)
    {
        return Reflection(vertexPosition, vertexNormal, recursive);
    }
    else
    {
        uint rayMask = 0xFF;

        RayDesc rayDesc;
        rayDesc.Origin = worldPos;
        rayDesc.Direction = refracted;
        rayDesc.TMin = 0.001;
        rayDesc.TMax = 100000;

        Payload refractPayload;
        refractPayload.color = float3(0, 1, 0);
        refractPayload.recursive = recursive;
        TraceRay(
            gRtScene,
            0,
            rayMask,
            0, // ray index
            1, // MultiplierForGeometryContrib
            0, // miss index
            rayDesc,
            refractPayload);
        return refractPayload.color;
    }
}

// 反射レイを射出
void ShootReflectionRay(inout Payload payload, float3 vertexPosition, float3 vertexNormal)
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
    rayDesc.TMax = 1000;
    
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

// シャドウレイ発射
bool ShootShadowRay(float3 origin, float3 direction, float tMax)
{
    RayDesc rayDesc;
    rayDesc.Origin = origin;
    rayDesc.Direction = direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = tMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    
    // ライトは除外。
    uint rayMask = 0xFF;

    TraceRay(
    gRtScene, flags, rayMask,
    0,
    1,
    1, // MISSシェーダーのインデックス
    rayDesc, payload);

    return payload.isShadow;
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
    
    
    // レイを発射した結果の色を取得
    float3 col = payload.color / (payload.recursive + 1.0f);

    // 結果格納
    if (gSceneParam.counter == 0)
    {
        
        gOutputBuff[launchIndex.xy] = float4(col, 1);
        gOutput[launchIndex.xy] = float4(col, 1);

    }
    else if (gSceneParam.counter < 128)
    {
        gOutputBuff[launchIndex.xy] += float4(col, 1);
        gOutput[launchIndex.xy] = gOutputBuff[launchIndex.xy] / (gSceneParam.counter + 1);
    }
    else
    {
        gOutput[launchIndex.xy] = gOutputBuff[launchIndex.xy] / 128.0f;
    }

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
    if (checkRecursiveLimit(payload))
    {
        return; // 呼び出し回数が越えたら即リターン.
    }

    // 完全反射
    if (instanceID == 0)
    {
        float3 reflectionColor = Reflection(vtx.Position, vtx.Normal, payload.recursive);
        payload.color = reflectionColor;
    }
    // 屈折
    if (instanceID == 1)
    {
        payload.color = Refraction(vtx.Position, vtx.Normal, payload.recursive);
    }
    // 通常ライティング
    else
    {
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
        payload.color = color;
        
        uint2 pixldx = DispatchRaysIndex().xy;
        uint2 numPix = DispatchRaysDimensions().xy;
        // ランダムなシードを計算。
        uint randSeed = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
        // 隠蔽度合い
        float visibility = 0.0f;
        
        // 飛ばすレイの回数
        const int aoRayCount = 1;
        for (int index = 0; index < aoRayCount; ++index)
        {
            
            // 法線を中心とした半球状のランダムなベクトルのサンプリング(コサイン重み分布付き)
            float3 sampleDir = GetUniformHemisphereSample(randSeed, vtx.Normal);
            
            // シャドウレイを飛ばす。
            float smpleVisiblity = ShootShadowRay(vtx.Position, sampleDir, 500);
            
            // 隠蔽度合い += サンプリングした値 * コサイン項 * 確率密度関数
            float nol = saturate(dot(vtx.Normal, sampleDir));
            float pdf = 1.0 / (2.0 * 3.14f);
            visibility += smpleVisiblity * nol / pdf;
            
        }
        
        // 平均を取る。
        visibility = (1.0f / 3.14f) * (1.0f / float(aoRayCount)) * visibility;
        
        // 光源へシャドウレイを飛ばす。
        float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
        float smpleVisiblity = ShootShadowRay(worldPosition, normalize(gSceneParam.lightDirection.xyz), 10000);
        // 隠蔽度合い += サンプリングした値 * コサイン項 * 確率密度関数
        float nol = saturate(dot(vtx.Normal, normalize(gSceneParam.lightDirection.xyz)));
        float pdf = 1.0 / (2.0 * 3.14f);
        visibility += smpleVisiblity * nol / pdf;
        
        visibility = saturate(visibility);
        
        payload.color.xyz *= visibility;

    }

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
    float4 diffuse = texture.SampleLevel(smp, vtx.uv, 0);
    if (diffuse.w < 0.5f)
    {
        IgnoreHit();

    }
}