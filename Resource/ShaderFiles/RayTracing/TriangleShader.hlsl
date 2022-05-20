
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
        
        // テクスチャの色を取得
        float4 texColor = texture.SampleLevel(smp, vtx.uv, 0.0f);

        float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
        float shadowRate = 1.0f;
        
        // 点光源へシャドウレイを発射。
        bool isShadow = false;
        if (gSceneParam.pointLight.isActive)
        {
            isShadow = ShootShadowRay(mul(float4(vtx.Position, 1), ObjectToWorld4x3()), normalize(gSceneParam.pointLight.lightPos - worldPosition), length(worldPosition - gSceneParam.pointLight.lightPos), gRtScene);
        }
        if (isShadow && gSceneParam.dirLight.isActive)
        {
            //isShadow = ShootShadowRay(worldPosition, gSceneParam.dirLight.lightDir, 10000.0f);
        }

        // 影なら暗くする。
        if (isShadow)
        {
            shadowRate = 0.5f;
        }

        payload.color = texColor;
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