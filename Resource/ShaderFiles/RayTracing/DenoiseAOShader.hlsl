
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
RWTexture2D<float4> outputColor : register(u0);
RWTexture2D<float4> outputLuminance : register(u1);

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
    flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShaderスキップ
    
    // ライトは除外。
    uint rayMask = ~(0x08);

    TraceRay(
    gRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // MISSシェーダーのインデックス
    rayDesc,
    payload);

    return payload.isShadow;
}

// シャドウレイ発射
bool ShootAOShadowRay(float3 origin, float3 direction, float tMax)
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
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderスキップ
    
    // ライトは除外。
    uint rayMask = ~(0x08);

    TraceRay(
    gRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // MISSシェーダーのインデックス
    rayDesc,
    payload);

    return payload.isShadow;
}

// ソフトシャドウ射出関数
float SoftShadow(Vertex vtx)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // 光源への中心ベクトル
    float3 pointLightPosition = gSceneParam.lightPos;
    float3 lightDir = normalize(pointLightPosition - worldPosition);
    
    // ライトベクトルと垂直なベクトルを求める。
    float3 perpL = cross(lightDir, float3(0, 1, 0));
    if (all(perpL == 0.0f))
    {
        perpL.x = 1.0f;
    }
    
    // 光源の端を求める。
    float3 toLightEdge = (pointLightPosition + perpL * gSceneParam.lightSize) - worldPosition;
    toLightEdge = normalize(toLightEdge);
    
    // 角度を求める。
    float coneAngle = acos(dot(lightDir, toLightEdge)) * 2.0f;
    
    // 乱数の種を求める。
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    int randSeed = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
    
    float3 shadowRayDir = GetConeSample(randSeed, lightDir, coneAngle);

    return ShootShadowRay(worldPosition, shadowRayDir, length(vtx.Position - gSceneParam.lightPos));
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
    DenoisePayload payload;
    payload.color = float3(0, 0, 0);
    payload.luminance = float3(0, 0, 0);
    payload.recursive = 0;

    // TransRayに必要な設定を作成
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderを無視。

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
    
    
   // 色情報と明るさ情報を保存。
    outputColor[launchIndex.xy] = float4(payload.color, 1.0f);
    outputLuminance[launchIndex.xy] = float4(payload.luminance, 1.0f);

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout DenoisePayload payload)
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
void mainCHS(inout DenoisePayload payload, MyAttribute attrib)
{
    
    // 呼び出し回数が制限を超えないようにする。
    if (checkRecursiveLimitDenoiseAO(payload))
    {
        return;
    }
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    
    // ポリゴンの描画するフラグが立っていたら。
    if (gSceneParam.isMeshScene)
    {
        payload.color = CalcBarycentrics(attrib.barys);
        return;
    }
    
    // 法線を描画するフラグが立っていたら。
    if (gSceneParam.isNormalScene)
    {
        payload.color = vtx.Normal;
        return;
    }

    // 法線マップが存在していたら法線マップから法線情報を抽出。
    vtx.Normal = normalMap.SampleLevel(smp, vtx.uv, 0.0f);

    // 通常ライティング
    if (instanceID == 2)
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
        float3 resultColor = color;
        
        uint2 pixldx = DispatchRaysIndex().xy;
        uint2 numPix = DispatchRaysDimensions().xy;
        
        // 隠蔽度合い
        float visibility = 0.0f;
        
        // 光源へシャドウレイを飛ばす。
        float smpleVisiblity = SoftShadow(vtx);
        
        // 隠蔽度合い += サンプリングした値 * コサイン項 * 確率密度関数
        float nol = saturate(dot(vtx.Normal, normalize(gSceneParam.lightPos.xyz)));
        float pdf = 1.0 / (2.0 * 3.14f);
        float lightVisibility = 0;
        lightVisibility += smpleVisiblity;
            
        // ライティングの結果明るかったら処理を飛ばす。
        if (lightVisibility != 1.0f)
        {
            
            // 飛ばすレイの回数
            for (int index = 0; index < gSceneParam.aoSampleCount; ++index)
            {
                // アンビエントオクリュージョンを行わないフラグが立っていたら処理を飛ばす。
                if (gSceneParam.isNoAO)
                {
                    break;
                }
                
                // 乱数を生成してレイを飛ばす方向を決める。
                float randSeedX = (frac(sin(dot(vtx.Position.xy + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
                float randSeedY = (frac(sin(dot(vtx.Position.xz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(78.233, 12.9898)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
                float randSeedZ = (frac(sin(dot(vtx.Position.yz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(32.9898, 48.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
                randSeedX = randSeedX * 2.0f - 1.0f;
                randSeedY = randSeedY * 2.0f - 1.0f;
                randSeedZ = randSeedZ * 2.0f - 1.0f;
                float3 sampleDir = float3(randSeedX, randSeedY, randSeedZ);
            
                // シャドウレイを飛ばす。
                float smpleVisiblity = ShootAOShadowRay(vtx.Position, sampleDir, 300);
            
                // 隠蔽度合い += サンプリングした値 * コサイン項 / 確率密度関数
                float nol = saturate(dot(vtx.Normal, sampleDir));
                float pdf = 1.0 / (2.0 * PI);
                visibility += smpleVisiblity * nol / pdf;
               
            }
            // 平均を取る。
            visibility = (1.0f / PI) * (1.0f / float(gSceneParam.aoSampleCount)) * visibility;
            
        }
        
        
        // 光源へのライティングを加算する。
        visibility += lightVisibility;
        
        // 隠蔽度合いが限界を超えないようにする。
        visibility = saturate(visibility);
        
        // ノイズのみを描画するフラグが立っていたら。
        if (gSceneParam.isNoiseOnlyScene)
        {
            payload.luminance = float3(visibility, visibility, visibility);
            return;

        }
        
        // 最終結果の色を保存。
        payload.color.xyz = resultColor;
        
        // 明るさ情報を保存。
        payload.luminance = visibility;
        
        // ライトに当たった面だけ表示するフラグが立っていたら。
        if (gSceneParam.isLightHitScene)
        {
            
            // 光にあたっていたら。
            if (0.0f < lightVisibility)
            {
                payload.luminance = float3(1, 1, 1);
            }
            else
            {
                payload.luminance = float3(0, 0, 0);
            }
            
            return;
            
        }
        
        //ShootReflectionRay(payload, vtx.Position, vtx.Normal);

    }
    else if (instanceID == 3)
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
void mainAnyHit(inout DenoisePayload payload, MyAttribute attrib)
{
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    float4 diffuse = texture.SampleLevel(smp, vtx.uv, 0);
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
