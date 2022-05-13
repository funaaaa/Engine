
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// グローバルルートシグネチャ
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneCB> gSceneParam : register(b0);

// ヒットグループのローカルルートシグネチャ
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> texture : register(t2, space1);
Texture2D<float4> normalMap : register(t3, space1);
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

// シャドウレイ発射
bool ShootShadowRay(Vertex vtx, float3 origin, float3 direction)
{
    RayDesc rayDesc;
    rayDesc.Origin = origin;
    rayDesc.Direction = direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = length(vtx.Position - gSceneParam.lightPos);

    ShadowPayload payload;
    payload.isShadow = true;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShaderをスキップ。

    // ライトは除外。
    uint rayMask = ~(0x08);

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
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flag |= RAY_FLAG_FORCE_OPAQUE;

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

    // 結果格納
    gOutput[launchIndex.xy] = float4(col, 1);
    gOutputBuff[launchIndex.xy] = float4(col, 1);
    
    gOutput[launchIndex.xy] = pow(gOutput[launchIndex.xy], 1 / 1.4);

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload payload)
{

    // 単色を返すようにする。
    //payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);
    payload.color = float3(0x32 / 255.0f, 0x90 / 255.0f, 0xD0 / 255.0f);

}

// シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout ShadowPayload payload)
{
    // 何にも当たっていないということなので、影は生成しない。
    payload.isShadow = false;
}

// closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]
void mainCHS(inout Payload payload, MyAttribute attrib)
{
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    
    // ポリゴンの描画するフラグが立っていたら。
    if (gSceneParam.isMeshScene)
    {
        payload.color.xy = attrib.barys;
        return;
    }
     
    // 法線を描画するフラグが立っていたら。
    if (gSceneParam.isNormalScene)
    {
        payload.color = vtx.Normal;
        return;
    }
       
    // 法線マップから法線情報を抽出。
    vtx.Normal = normalMap.SampleLevel(smp, vtx.uv, 0.0f);

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
    else if (instanceID == 2)
    {
        // lambert ライティングを行う.
        float3 lightdir = -normalize(gSceneParam.lightPos.xyz);

        float nl = saturate(dot(vtx.Normal, lightdir));
        
        // 正規化ランバート？
        nl /= PI;

        float3 lightcolor = gSceneParam.lightColor.xyz;
        float3 ambientcolor = gSceneParam.ambientColor.xyz;
        float4 materialcolor = texture.SampleLevel(smp, vtx.uv, 0.0f);
        float3 color = 0;
        color += lightcolor * materialcolor.xyz * nl;
        color += ambientcolor * materialcolor.xyz;

        // ライティングの結果の色を保存。
        payload.color = color;

        // シャドウレイを発射。
        float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
        float shadowRate = 1.0f;
        bool isShadow = ShootShadowRay(vtx, worldPosition, normalize(gSceneParam.lightPos - worldPosition));
        //bool isShadow = ShootShadowRay(worldPosition, normalize(float3(0, 7, 0) - worldPosition));

        // 影なら暗くする。
        if (isShadow)
        {
            shadowRate = 0.5f;
        }

        payload.color.xyz *= shadowRate;
        
        payload.color = saturate(payload.color);
        
        // ライトに当たった面だけ表示するフラグが立っていたら。
        if (gSceneParam.isLightHitScene)
        {
            
            // 光にあたっていたら。
            if (isShadow)
            {
                payload.color = float3(0, 0, 0);
            }
            else
            {
                payload.color = float3(1, 1, 1);
            }
            
        }

    }
    else
    {
        payload.color = float3(1, 1, 1);
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
    
    uint instanceID = InstanceID();
    // インスタンスIDが3(ライト)なら当たり判定を棄却する。
    if (instanceID == 3)
    {
        IgnoreHit();

    }
}