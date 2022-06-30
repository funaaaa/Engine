
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// グローバルルートシグネチャ
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<ConstBufferData> gSceneParam : register(b0);

// ヒットグループのローカルルートシグネチャ
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
StructuredBuffer<Material> material : register(t2, space1);
Texture2D<float4> texture : register(t3, space1);
Texture2D<float4> normalTexture : register(t4, space1);
// サンプラー
SamplerState smp : register(s0, space1);

// RayGenerationシェーダーのローカルルートシグネチャ
RWTexture2D<float4> aoOutput : register(u0);
RWTexture2D<float4> lightingOutput : register(u1);
RWTexture2D<float4> colorOutput : register(u2);
RWTexture2D<float4> giOutput : register(u3);
RWTexture2D<float4> denoiseMaskoutput : register(u4);

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

// 大気散乱
float3 AtmosphericScattering(float3 pos, inout float3 mieColor, inout bool isUnderGround)
{
    
    // 地平線以下は真っ黒にする。
    if (pos.y < -10)
    {
        isUnderGround = true;
        return float3(0.1f, 0.1f, 0.1f);
    }
    
    // レイリー散乱定数
    float kr = gSceneParam.as.kr;
    // ミー散乱定数
    float km = gSceneParam.as.km;
 
    // 大気中の線分をサンプリングする数。
    float fSamples = gSceneParam.as.samples;
 
    // 謎の色 色的には薄めの茶色
    float3 three_primary_colors = float3(0.68f, 0.55f, 0.44f);
    // 光の波長？
    float3 v3InvWaveLength = 1.0f / pow(three_primary_colors, 4.0f);
 
    // 大気圏の一番上の高さ。
    float fOuterRadius = gSceneParam.as.outerRadius;
    // 地球全体の地上の高さ。
    float fInnerRadius = gSceneParam.as.innerRadius;
 
    // 太陽光の強さ？
    float fESun = gSceneParam.as.eSun;
    // 太陽光の強さにレイリー散乱定数をかけてレイリー散乱の強さを求めている。
    float fKrESun = kr * fESun;
    // 太陽光の強さにミー散乱定数をかけてレイリー散乱の強さを求めている。
    float fKmESun = km * fESun;

    // レイリー散乱定数に円周率をかけているのだが、限りなく0に近い値。
    float fKr4PI = kr * 4.0f * PI;
    // ミー散乱定数に円周率をかけているのだが、ミー散乱定数は0なのでこれの値は0。
    float fKm4PI = km * 4.0f * PI;
 
    // 地球全体での大気の割合。
    float fScale = 1.0f / (fOuterRadius - fInnerRadius);
    // 平均大気密度を求める高さ。
    float fScaleDepth = gSceneParam.as.aveHeight;
    // 地球全体での大気の割合を平均大気密度で割った値。
    float fScaleOverScaleDepth = fScale / fScaleDepth;
 
    // 散乱定数を求める際に使用する値。
    float g = gSceneParam.as.g;
    // 散乱定数を求める際に使用する値を二乗したもの。なぜ。
    float g2 = g * g;
    
    // 当たった天球のワールド座標
    float3 worldPos = normalize(pos) * fOuterRadius;
    worldPos = IntersectionPos(normalize(worldPos), float3(0.0, fInnerRadius, 0.0), fOuterRadius);
    
    // カメラ座標 元計算式だと中心固定になってしまっていそう。
    float3 v3CameraPos = float3(0.0, fInnerRadius + 1.0f, 0.0f);
    
    // ディレクショナルライトの場所を求める。
    float3 dirLightPos = -gSceneParam.light.dirLight.lightDir * 1000000.0f;
    
    // ディレクショナルライトへの方向を求める。
    float3 v3LightDir = normalize(dirLightPos - worldPos);
 
    // 天球上頂点からカメラまでのベクトル(光が大気圏に突入した点からカメラまでの光のベクトル)
    float3 v3Ray = worldPos - v3CameraPos;
    
    // 大気に突入してからの点とカメラまでの距離。
    float fFar = length(v3Ray);
    
    // 正規化された拡散光が来た方向。
    v3Ray /= fFar;
 
    // サンプリングする始点座標 資料だとAの頂点
    float3 v3Start = v3CameraPos;
    // サンプルではカメラの位置が(0,Radius,0)なのでカメラの高さ。どの位置に移動しても地球視点で見れば原点(地球の中心)からの高さ。
    float fCameraHeight = length(v3CameraPos);
    // 地上からの法線(?)と拡散光がやってきた角度の内積によって求められた角度をカメラの高さで割る。
    float fStartAngle = dot(v3Ray, v3Start) / fCameraHeight;
    // 開始地点の高さに平均大気密度をかけた値の指数を求める？
    float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    // 開始地点のなにかの角度のオフセット。
    float fStartOffset = fStartDepth * scale(fStartAngle);
    
    // サンプルポイント間の長さ。
    float fSampleLength = fFar / fSamples;
    // サンプルポイント間の長さに地球の大気の割合をかける。何故。
    float fScaledLength = fSampleLength * fScale;
    // 拡散光が来た方向にサンプルの長さをかけることでサンプルポイント間のレイをベクトルを求める。
    float3 v3SampleRay = v3Ray * fSampleLength;
    // 最初のサンプルポイントを求める。0.5をかけてるのは少し動かすため？
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;
 
    // 色情報
    float3 v3FrontColor = 0.0f;
    for (int n = 0; n < int(fSamples); n++)
    {
        // サンプルポイントの高さ。どちらにせよ原点は地球の中心なので、この値が現在位置の高さになる。
        float fHeight = length(v3SamplePoint);
        // 地上からサンプルポイントの高さの差に平均大気密度をかけたもの。  高度に応じて大気密度が指数的に小さくなっていくのを表現している？
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        // 地上から見たサンプルポイントの法線とディレクショナルライトの方向の角度を求めて、サンプルポイントの高さで割る。
        float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight; // こいつの値が-1になる→Scale内の計算でexpの引数が43になり、とてつもなくでかい値が入る。 → -にならないようにする？
        // 地上から見たサンプルポイントの法線と散乱光が飛んできている方区の角度を求めて、サンプルポイントの高さで割る。
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
        // 散乱光？
        float fScatter = (fStartOffset + fDepth * (scale(fLightAngle * 1) - scale(fCameraAngle * 1)));
        
        // 色ごとの減衰率？
        float3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI));
        // サンプルポイントの位置を考慮して散乱した色を求める。
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        // サンプルポイントを移動させる。
        v3SamplePoint += v3SampleRay;
        
    }
 
    // レイリー散乱に使用する色情報
    float3 c0 = v3FrontColor * (v3InvWaveLength * fKrESun);
    // ミー散乱に使用する色情報
    float3 c1 = v3FrontColor * fKmESun;
    // カメラ座標から天球の座標へのベクトル。
    float3 v3Direction = v3CameraPos - worldPos;
 
    //float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos2 = fcos * fcos;
 
    // レイリー散乱の明るさ。
    float rayleighPhase = 0.75f * (1.0f + fcos2);
    // ミー散乱の明るさ。
    float miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fcos2) / pow(1.0f + g2 - 2.0f * g * fcos, 1.5f);
    
    // ミー散乱の色を保存。
    mieColor = c0 * rayleighPhase;
    
    // 交点までのベクトルと太陽までのベクトルが近かったら白色に描画する。
    if (0.999f < dot(normalize(dirLightPos - v3CameraPos), normalize(worldPos - v3CameraPos)))
    {
        return float3(1, 1, 1);
    }
 
    // 最終結果の色
    float3 col = 1.0f;
    col.rgb = rayleighPhase * c0 + miePhase * c1;
    return col;
    
}

// ソフトシャドウ射出関数
float SoftShadow(Vertex vtx, float lightSize, float length, int lightIndex)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // 光源への中心ベクトル
    float3 pointLightPosition = gSceneParam.light.pointLight[lightIndex].lightPos;
    float3 lightDir = normalize(pointLightPosition - worldPosition);
    
    // ライトベクトルと垂直なベクトルを求める。
    float3 perpL = cross(lightDir, float3(0, 1, 0));
    if (all(perpL == 0.0f))
    {
        perpL.x = 1.0f;
    }
    
    // 光源の端を求める。
    float3 toLightEdge = (pointLightPosition + perpL * lightSize) - worldPosition;
    toLightEdge = normalize(toLightEdge);
    
    // 角度を求める。
    float coneAngle = acos(dot(lightDir, toLightEdge)) * 2.0f;
    
    // 乱数の種を求める。
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    int randSeed = initRand(DispatchRaysIndex().x + (vtx.Position.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);
    
    float3 shadowRayDir = GetConeSample(randSeed, lightDir, coneAngle);
    return ShootShadowRay(worldPosition, shadowRayDir, length, gRtScene);
    
}

// 太陽光の影チェック用レイの準備関数 戻り値は太陽光の色
bool ShootDirShadow(Vertex vtx, float length)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // ライトベクトルと垂直なベクトルを求める。
    float3 perpL = cross(-gSceneParam.light.dirLight.lightDir, float3(0, 1, 0));
    if (all(perpL == 0.0f))
    {
        perpL.x = 1.0f;
    }
    
    // 並行光源の座標を仮で求める。
    float3 dirLightPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;
    
    // 並行光源までのベクトル。
    float3 dirLightVec = dirLightPos - worldPosition;
    dirLightVec = normalize(dirLightVec);
    
    //// 光源の端を求める。
    //float3 toLightEdge = ((worldPosition + dirLightPos) + perpL * 10000) - worldPosition;
    //toLightEdge = normalize(toLightEdge);
    
    //// 角度を求める。
    //float coneAngle = acos(dot(dirLightVec, toLightEdge)) * 2.0f;
    
    //// 乱数の種を求める。
    //uint2 pixldx = DispatchRaysIndex().xy;
    //uint2 numPix = DispatchRaysDimensions().xy;
    //int randSeed = initRand(DispatchRaysIndex().x + (worldPosition.x * gSceneParam.light.dirLight.lightDir.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);
    
    //// レイを撃つベクトル
    //float3 shadowRayDir = GetConeSample(randSeed, dirLightVec, coneAngle);
    float3 shadowRayDir = dirLightVec;
    
    return ShootShadowRayNoAH(worldPosition, shadowRayDir, length, gRtScene);
    
}

// GI
float3 ShootGIRay(Vertex vtx, float length)
{
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldNormal = mul(vtx.Normal, (float3x3) ObjectToWorld4x3());
    float3 worldRayDir = WorldRayDirection();
    float3 reflectDir = reflect(worldRayDir, worldNormal);
    
    // レイのフラグを設定。
    RAY_FLAG flag = RAY_FLAG_NONE;
    //flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderを無視。
    
    // レイのマスク
    uint rayMask = 0xFF;

    // レイのパラメーターを設定。
    RayDesc rayDesc;
    rayDesc.Origin = worldPos;
    rayDesc.Direction = reflectDir;
    rayDesc.TMin = 0.01;
    rayDesc.TMax = length;

    // ペイロードを初期化。
    DenoisePayload reflectPayload;
    reflectPayload.color = float3(0, 0, 0);
    reflectPayload.aoLuminance = float3(0, 0, 0);
    reflectPayload.lightLuminance = float3(0, 0, 0);
    reflectPayload.giColor = float3(0, 0, 0);
    reflectPayload.recursive = 0;
    reflectPayload.rayID = CHS_IDENTIFICATION_RAYID_GI;
    
    // レイを発射。
    TraceRay(
        gRtScene,
        flag,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        reflectPayload);
    return reflectPayload.giColor;
}

// RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);

    float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
    float aspect = dims.x / dims.y;

    matrix mtxViewInv = gSceneParam.camera.mtxViewInv;
    matrix mtxProjInv = gSceneParam.camera.mtxProjInv;

    // レイの設定
    RayDesc rayDesc;
    rayDesc.Origin = mul(mtxViewInv, float4(0, 0, 0, 1)).xyz;

    float4 target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
    float3 dir = mul(mtxViewInv, float4(target.xyz, 0)).xyz;

    rayDesc.Direction = normalize(dir);
    rayDesc.TMin = 0;
    rayDesc.TMax = 300000;

    // ペイロードの設定
    DenoisePayload payload;
    payload.color = float3(0, 0, 0);
    payload.aoLuminance = float3(0, 0, 0);
    payload.lightLuminance = float3(0, 0, 0);
    payload.giColor = float3(0, 0, 0);
    payload.recursive = 0;
    payload.rayID = CHS_IDENTIFICATION_RAYID_DEF;

    // TransRayに必要な設定を作成
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
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
    
    // 結果書き込み用UAVを一旦初期化。
    lightingOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    aoOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    colorOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    giOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    denoiseMaskoutput[launchIndex.xy] = float4(1, 1, 1, 1);

    // 結果格納
    lightingOutput[launchIndex.xy] = float4(payload.lightLuminance, 1);
    aoOutput[launchIndex.xy] = float4(payload.aoLuminance, 1);
    colorOutput[launchIndex.xy] = float4(payload.color, 1);
    giOutput[launchIndex.xy] = float4(payload.giColor, 1);
    denoiseMaskoutput[launchIndex.xy] = float4(payload.denoiseMask, 1);

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout DenoisePayload payload)
{

    // 単色を返すようにする。
    payload.color = float3(0, 0, 0);
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
    ++payload.recursive;
    if (2 < payload.recursive)
    {
        return;
    }
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldNormal = normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3()));

    // テクスチャの色を保存。
    float3 texColor = (float3) texture.SampleLevel(smp, vtx.uv, 0.0f);
    
    // 法線マップの色を取得。
    float3 normalMapColor = (float3) normalTexture.SampleLevel(smp, vtx.uv, 0.0f);
    
    // マスク用のテクスチャを保存。
    payload.denoiseMask = normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3())) * (InstanceIndex() / 10.0f);
    
    // 法線マップの色とテクスチャの色が同じだったら、その法線マップはメモリの隙間を埋めるためにいれられたテクスチャなので、法線マップとして適応させない。
    if (!(texColor.x == normalMapColor.x && texColor.y == normalMapColor.y && texColor.z == normalMapColor.z))
    {
        worldNormal = normalize(mul(normalMapColor, (float3x3) ObjectToWorld4x3()));
    }
    
    // レイのIDがCHS_IDENTIFICATION_GIだったらGI用のレイなのでテクスチャの色を返す。
    if (payload.rayID == CHS_IDENTIFICATION_RAYID_GI)
    {
        // レイの長さ
        float rayLength = length(WorldRayOrigin() - worldPos);
        
        // レイの長さの最大値
        const float MAX_RAY = 500.0f;
        
        // 割合を求める。
        float rate = rayLength / MAX_RAY;
        rate = 1.0f - saturate(rate);
        
        payload.giColor += texColor * rate;
        
        // このオブジェクトが全反射だったら黒を返す。
        if (instanceID == CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION)
        {
            payload.giColor = float3(0, 0, 0);
        }
        
        return;
    }

    
    // Instance数がCHS_IDENTIFICATION_ASだったら大気散乱を計算。
    if (instanceID == CHS_IDENTIFICATION_INSTNACE_AS)
    {
        float3 mieColor = float3(1, 1, 1);
        bool isUnderGround = false;
        
        payload.lightLuminance += float3(1, 1, 1);
        payload.color += AtmosphericScattering(worldPos, mieColor, isUnderGround);
        payload.aoLuminance += float3(1, 1, 1);
        payload.giColor += float3(0, 0, 0);
        
        // マスク用のテクスチャを白くする。
        payload.denoiseMask = float3(1, 1, 1);
        
        // 天球の明るさを求める。
        float t = dot(payload.color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
        
        // 地平線より下じゃなかったら星を描画する。
        if (!isUnderGround)
        {
            t = (1.0f - t);
            t = t == 0.0f ? 0.0f : pow(2.0f, 10.0f * t - 10.0f);
            t = t == 0.0f ? 0.0f : pow(2.0f, 10.0f * t - 10.0f);
            payload.color += texColor * t;
            payload.color = saturate(payload.color);
            
        }
        
        return;
    }
    
    // Instance数がCHS_IDENTIFICATION_TEXCOLORだったらテクスチャの色をそのまま返す。
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_TEXCOLOR || instanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT)
    {
        payload.lightLuminance += float3(1, 1, 1);
        payload.color += texColor;
        payload.aoLuminance += float3(1, 1, 1);
        payload.giColor += float3(0, 0, 0);
        
        return;
    }
    
    // InstanceIDが反射レイだったら完全反射させる。
    if (instanceID == CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION)
    {
        
        // 完全反射レイを飛ばす。
        ShootRay(CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION, worldPos, reflect(WorldRayDirection(), worldNormal), payload, gRtScene);
        
        // デバッグ用で若干白っぽくする。
        payload.color += 0.1f;
        
        return;
        
    }
    
    // ポリゴンの描画するフラグが立っていたら。
    if (gSceneParam.debug.isMeshScene)
    {
        payload.lightLuminance = CalcBarycentrics(attrib.barys);
        return;
    }
    
    // 法線を描画するフラグが立っていたら。
    if (gSceneParam.debug.isNormalScene)
    {
        payload.lightLuminance = worldNormal;
        return;
    }

    // 通常ライティング
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    
    // ライティングの結果の明るさ
    float pointLightVisibility = 0;
    float dirLightVisibility = 0;
    float aoLightVisibility = 0;
    
    // ポイントライトのライティング結果の色
    float3 pointLightColor = float3(0, 0, 0);
    
    // 並行光源のライティング結果の色
    float3 dirLightColor = float3(0, 0, 0);
    
    for (int index = 0; index < POINT_LIGHT_COUNT; ++index)
    {
        
        // ライトが有効化されていなかったら処理を飛ばす。
        if (!gSceneParam.light.pointLight[index].isActive)
        {
            continue;
        }
        
        // ライトまでの距離
        float lightLength = length(gSceneParam.light.pointLight[index].lightPos - worldPos);
    
        // 点光源へシャドウレイを飛ばす。
        if (lightLength < gSceneParam.light.pointLight[index].lightPower && gSceneParam.light.pointLight[index].isActive)
        {
            
            // 影を出さないフラグが立っていたら影を飛ばさない。
            float pointLightVisibilityBuff = 0;
            if (!gSceneParam.light.pointLight[index].isShadow)
            {
                pointLightVisibilityBuff = SoftShadow(vtx, gSceneParam.light.pointLight[index].lightSize, length(gSceneParam.light.pointLight[index].lightPos - worldPos), index);
            }
            else
            {
                pointLightVisibilityBuff = 1;
            }
        
            // 影だったら
            if (0 <= pointLightVisibilityBuff)
            {
                
                float3 pointLightDir = worldPos - gSceneParam.light.pointLight[index].lightPos;
                pointLightDir = normalize(pointLightDir);
            
                // 明るさを減衰させる。
                float rate = lightLength / gSceneParam.light.pointLight[index].lightPower;
                rate = pow(rate, 5);
                rate = 1.0f - rate;
                
                // ディフューズを計算する。
                float mDiffuse = material[0].diffuse.x;
                float3 diffuse = dot(-pointLightDir, worldNormal) * mDiffuse;
			    // 光沢度
                const float shininess = 4.0f;
		    	// 頂点から視点への方向ベクトル
                float3 eyedir = normalize(WorldRayOrigin() - worldPos);
		    	// 反射光ベクトル
                float3 reflect = normalize(pointLightDir + 2.0f * dot(-pointLightDir, worldNormal) * worldNormal);
                // 鏡面反射光
                float mSpecular = material[0].specular.x;
                float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
                
                // ランバートの反射率と明るさをかける。
                pointLightVisibilityBuff = pointLightVisibilityBuff * ((diffuse.x + specular.x) * rate);
                
                pointLightColor += gSceneParam.light.pointLight[index].lightColor * pointLightVisibilityBuff;
            
                pointLightVisibility += pointLightVisibilityBuff;
                
            
            }
        
        }
    }
    
    
    
    pointLightColor = saturate(pointLightColor);
    pointLightVisibility = saturate(pointLightVisibility);
            
    // 太陽の位置
    float3 sunPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;
    float3 sunDir = normalize(sunPos - worldPos);
    
    // 並行光源にシャドウレイを飛ばす。
    if (gSceneParam.light.dirLight.isActive/* && gSceneParam.light.dirLight.lightDir.y < 0.2f*/)
    {
        
        // 並行光源での影情報を取得。
        dirLightVisibility = ShootDirShadow(vtx, 10000.0f);
        
        // 影じゃない場合、天球上でのライティングの色を取得する。
        if (0 < dirLightVisibility && gSceneParam.light.dirLight.lightDir.y < 0.2f)
        {
        
            // 影の色を計算。
            //dirLightColor = dirLightVisibility * gSceneParam.light.dirLight.lightColor;
            dirLightColor = float3(0, 0, 0);
            
            // ディフューズを計算する。
            float mDiffuse = material[0].diffuse.x;
            float diffuse = dot(sunDir, worldNormal) * mDiffuse;
            
			// 光沢度
            const float shininess = 4.0f;
			// 頂点から視点への方向ベクトル
            float3 eyedir = normalize(WorldRayOrigin() - worldPos);
			// 反射光ベクトル
            float3 reflect = normalize(-sunDir + 2.0f * dot(sunDir, worldNormal) * worldNormal);
            // 鏡面反射光
            float mSpecular = material[0].specular.x;
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // 隠蔽度を更新。
            dirLightVisibility = diffuse + specular;
            
            const float SKYDOME_RADIUS = 15000.0f;
            const float SAMPLING_POS_Y = 0.0f;
            
            // サンプリングしたい天球上での位置までのベクトル。
            //float3 samplingVec = normalize(float3(1.0f, 0.1f, 0.1f)) * SKYDOME_RADIUS;
            float3 samplingVec = normalize(-gSceneParam.light.dirLight.lightDir * float3(1.0f, 0.0f, 1.0f)) * SKYDOME_RADIUS;
            samplingVec.y = 0.1f;
            
            // Y軸のサンプリングしたい位置を設定する。
            samplingVec.y = SAMPLING_POS_Y;
            
            // 正規化する。
            samplingVec = normalize(samplingVec);
            
            // 天球上のサンプリングしたい位置の座標を求める。
            float3 samplingPos;
            samplingPos = samplingVec * SKYDOME_RADIUS;
            
            // 大気散乱の色を取得。
            float3 mieColor = float3(1, 1, 1);
            bool isUnderGround = false;
            float3 skydomeColor = AtmosphericScattering(samplingPos, mieColor, isUnderGround);
            
            // ディレクショナルライトの色を設定。 夕焼けのときに赤っぽくするために赤い成分以外を弱める。
            dirLightColor += mieColor * float3(1.0f, 0.5f, 0.5f);
            
            dirLightColor = normalize(dirLightColor);
            

            
        }
        else
        {
            dirLightColor = float3(0, 0, 0);
            dirLightVisibility = 0;
        }
        
    }
    
        
    // 飛ばすレイの回数
    for (int aoindex = 0; aoindex < 1; ++aoindex)
    {
        // アンビエントオクリュージョンを行わないフラグが立っていたら処理を飛ばす。
        if (gSceneParam.debug.isNoAO)
        {
            break;
        }
       
        int seed = initRand(DispatchRaysIndex().x + (worldPos.x / 1000.0f) + aoindex + DispatchRaysIndex().y * numPix.x, 100);
        float3 sampleDir = GetUniformHemisphereSample(seed, worldNormal);
        
        // シャドウレイを飛ばす。
        float aoLightVisibilityBuff = ShootAOShadowRay(worldPos, sampleDir, 10, gRtScene);
        
        // 隠蔽度合い += サンプリングした値 * コサイン項 / 確率密度関数
        float NoL = saturate(dot(worldNormal, sampleDir));
        float pdf = 1.0 / (2.0 * PI);
        aoLightVisibility += aoLightVisibilityBuff;
        
            
    }
    // 平均を取る。
    //aoLightVisibility = (1.0f / PI) * (1.0f / float(gSceneParam.debug.aoSampleCount)) * aoLightVisibility;
        
    
    
    // 光源へのライティングを加算する。
    float lightVisibility = 0;
    lightVisibility += pointLightVisibility + dirLightVisibility;
    float aoVisibility = aoLightVisibility;
    
    // 最終結果の色を保存。
    payload.color.xyz += texColor;
    payload.lightLuminance += lightVisibility + (pointLightColor + dirLightColor) / PI;
    payload.aoLuminance += aoVisibility;
    
    // GIの色を取得する。
    if (instanceID == 10 && !gSceneParam.debug.isNoGI)
    {
        payload.giColor += ShootGIRay(vtx, 500) * 1.0f;
        payload.giColor = saturate(payload.giColor);
    }
    else
    {
        payload.giColor += float3(0, 0, 0);
    }
    
    // GIのみを描画するフラグが立っていたらGI以外の色を無効化する。
    if (gSceneParam.debug.isGIOnlyScene)
    {
        
        payload.lightLuminance = float3(1, 1, 1);
        payload.color = float3(0, 0, 0);
        payload.aoLuminance = float3(0, 0, 0);
        
    }
    
    // ライトに当たった面だけ表示するフラグが立っていたら。
    if (gSceneParam.debug.isLightHitScene)
    {
        
        // 光にあたっていたら。
        if (0.0f < lightVisibility)
        {
            payload.color = float3(1, 1, 1);
            payload.lightLuminance = float3(1, 1, 1);
        }
        else
        {
            payload.color = float3(0, 0, 0);
            payload.lightLuminance = float3(0, 0, 0);
        }
        
        return;
        
    }
    
    
    // InstanceIDが反射レイだったら反射させる。
    if (instanceID == CHS_IDENTIFICATION_ISNTANCE_REFLECTION)
    {
        
        // 反射レイを飛ばす。
        ShootRay(CHS_IDENTIFICATION_RAYID_RECLECTION, worldPos, reflect(WorldRayDirection(), worldNormal), payload, gRtScene);
        
    }

    // instanceIDが屈折のレイだったら屈折のレイを飛ばす。
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_REFRACTION)
    {

        float refractVal = 1.4f;
        float3 rayDir = float3(0, 0, 0);

        // 法線とレイの方向を内積して空気中からオブジェクトに屈折するのか、オブジェクトから空気に屈折するのかを判定する。
        float nr = dot(worldNormal, WorldRayDirection());
        if (nr < 0)
        {

            // 空気中からオブジェクト
            float eta = 1.0f / refractVal;
            rayDir = refract(WorldRayDirection(), worldNormal, eta);

        }
        else
        {

            // オブジェクトから空気中
            float eta = refractVal / 1.0f;
            rayDir = refract(WorldRayDirection(), -worldNormal, eta);
      
        }
        
        if (length(rayDir) == 0)
        {
            payload.color = worldNormal;
            return;
        }

        ShootRay(CHS_IDENTIFICATION_RAYID_REFRACTION, worldPos, rayDir, payload, gRtScene);

    }
    
    //// フォグの計算を行う。
    //const float MAX_RAY_LENGTH = 8000.0f; // レイの最大の長さ
    //float rayLength = RayTCurrent(); // 現在の深度。
    //float depthRate = rayLength / MAX_RAY_LENGTH;
    //// レイの長さのレートにイージングをかける。
    //depthRate = 1.0f - cos((depthRate * PI) / 2.0f);
    //const float SKYDOME_RADIUS = 15000.0f;
    //const float SAMPLING_POS_Y = 0.0f;
    //// サンプリングしたい天球上での位置までのベクトル。
    //float3 samplingVec = WorldRayDirection() * SKYDOME_RADIUS;
    //// Y軸のサンプリングしたい位置を設定する。
    //samplingVec.y = SAMPLING_POS_Y;
    //// 正規化する。
    //samplingVec = normalize(samplingVec);
    //// 天球上のサンプリングしたい位置の座標を求める。
    //float3 samplingPos;
    //samplingPos = samplingVec * SKYDOME_RADIUS;
    //// 大気散乱の色を取得。
    //float3 mieColor = float3(1, 1, 1);
    //float3 skydomeColor = AtmosphericScattering(samplingPos, mieColor);
    //payload.color += mieColor * (depthRate - 0.5f);
    //payload.color = saturate(payload.color);
    
    
    // レイの種類によって要素を割る。
    if (!(payload.rayID == CHS_IDENTIFICATION_RAYID_DEF))
    {
        
        // 0を割りたくないので0以外だったらという条件式を追加。
        if (length(payload.aoLuminance) != 0)
        {
            payload.aoLuminance /= 2.0f;
        }
        // 0を割りたくないので0以外だったらという条件式を追加。
        if (length(payload.color) != 0)
        {
            payload.color /= 2.0f;
        }
        // 0を割りたくないので0以外だったらという条件式を追加。
        if (length(payload.giColor) != 0)
        {
            payload.giColor /= 2.0f;
        }
        // 0を割りたくないので0以外だったらという条件式を追加。
        if (length(payload.lightLuminance) != 0)
        {
            payload.lightLuminance /= 2.0f;
        }
        
        // ペイロードのレイのIDをデフォルトに戻す。
        payload.rayID = CHS_IDENTIFICATION_RAYID_DEF;
        
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
    
    // インスタンスIDが1(ライト)なら当たり判定を棄却する。
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT || instanceID == CHS_IDENTIFICATION_INSTANCE_INVISIBILITY)
    {
        IgnoreHit();

    }
    
}