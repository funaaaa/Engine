
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// TLAS
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<ConstBufferData> gSceneParam : register(b0);

// 各リソース等
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
StructuredBuffer<Material> material : register(t2, space1);
Texture2D<float4> texture : register(t3, space1);
Texture2D<float4> normalTexture : register(t4, space1);
RWTexture2D<float4> tireMaskTexture : register(u0, space1);
// サンプラー
SamplerState smp : register(s0, space1);

// 出力先UAV
RWTexture2D<float4> aoOutput : register(u0);
RWTexture2D<float4> lightingOutput : register(u1);
RWTexture2D<float4> colorOutput : register(u2);
RWTexture2D<float4> giOutput : register(u3);
RWTexture2D<float4> denoiseMaskoutput : register(u4);

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
    float fStartOffset = fStartDepth * Scale(fStartAngle);

    // サンプルポイント間の長さ。
    float fSampleLength = fFar / fSamples;
    // サンプルポイント間の長さに地球の大気の割合をかける。
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
        float fScatter = (fStartOffset + fDepth * (Scale(fLightAngle * 1) - Scale(fCameraAngle * 1)));

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
    int randSeed = InitRand(DispatchRaysIndex().x + (vtx.Position.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);
    
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
    //int randSeed = InitRand(DispatchRaysIndex().x + (worldPosition.x * gSceneParam.light.dirLight.lightDir.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);

    //// レイを撃つベクトル
    //float3 shadowRayDir = GetConeSample(randSeed, dirLightVec, coneAngle);
    float3 shadowRayDir = dirLightVec;
    
    return ShootShadowRayNoAH(worldPosition + normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3())) * 2.0f, shadowRayDir, length, gRtScene);
    
}

// GI
void ShootGIRay(Vertex vtx, float length, inout Payload PayloadData)
{
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldRayDir = WorldRayDirection();
    float3 reflectDir = reflect(worldRayDir, vtx.Normal);
    
    // レイのフラグを設定。
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderを無視。

    // レイのマスク
    uint rayMask = 0xFF;

    // レイのパラメーターを設定。
    RayDesc rayDesc;
    rayDesc.Origin = worldPos;
    rayDesc.Direction = reflectDir;
    rayDesc.TMin = 0.0;
    rayDesc.TMax = length;

    // ペイロードを初期化。
    int rayID = PayloadData.rayID_;
    PayloadData.rayID_ = CHS_IDENTIFICATION_RAYID_GI;
    
    // レイを発射。
    TraceRay(
        gRtScene,
        flag,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        PayloadData);
    
    PayloadData.rayID_ = rayID;
    
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
    Payload payloadData;
    payloadData.impactAmount_ = 1.0f;
    payloadData.rayID_ = CHS_IDENTIFICATION_RAYID_DEF;
    payloadData.recursive_ = 0;
    payloadData.ao_ = 0;
    payloadData.color_ = float3(0, 0, 0);
    payloadData.denoiseMask_ = float3(0, 0, 0);
    payloadData.gi_ = float3(0, 0, 0);
    payloadData.light_ = float3(0, 0, 0);
    payloadData.isCullingAlpha_ = false;
    payloadData.alphaCounter_ = 0;

    // TransRayに必要な設定を作成
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShaderを実行。

    // レイを発射
    TraceRay(
    gRtScene, // TLAS
    flag, // 衝突判定制御をするフラグ
    rayMask, // 衝突判定対象のマスク値
    0, // ray index
    1, // MultiplierForGeometryContrib
    0, // miss index
    rayDesc,
    payloadData);
    
    if (payloadData.color_.x <= 0.4f && payloadData.color_.y <= 0.4f && payloadData.color_.z <= 0.4f)
    {
        //payloadData.color_ = float3(1, 1, 0);
    }

    // 結果書き込み用UAVを一旦初期化。
    lightingOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    aoOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    colorOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    giOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    denoiseMaskoutput[launchIndex.xy] = float4(1, 1, 1, 1);

    // 結果格納
    lightingOutput[launchIndex.xy] = float4(saturate(payloadData.light_), 1);
    aoOutput[launchIndex.xy] = float4(saturate(payloadData.ao_), saturate(payloadData.ao_), saturate(payloadData.ao_), 1);
    colorOutput[launchIndex.xy] = float4(saturate(payloadData.color_), 1);
    giOutput[launchIndex.xy] = float4(saturate(payloadData.gi_), 1);
    denoiseMaskoutput[launchIndex.xy] = float4(payloadData.denoiseMask_, 1);

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload payload)
{

    // 単色を返すようにする。
   //payload.color_ = float3(0, 0, 0);
    //payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);
    //payload.color = float3(0x32 / 255.0f, 0x90 / 255.0f, 0xD0 / 255.0f);

}

// シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout Payload payload)
{
    // 何にも当たっていないということなので、影は生成しない。
    payload.impactAmount_ = 1.0f;
}

// ライティング前処理
bool ProcessingBeforeLighting(inout Payload PayloadData, Vertex Vtx, MyAttribute Attrib, float3 WorldPos, float3 WorldNormal, inout float4 TexColor, uint InstanceID)
{
    
    // デノイズ用のマスクに使用するテクスチャに法線の色とInstanceIndexをかけたものを書き込む。
    if (PayloadData.recursive_ == 1)
    {
        PayloadData.denoiseMask_ = (WorldNormal);
    }
    
    // InstanceIDがCHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASKだったらテクスチャに色を加算。
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK)
    {
        float4 tiremasktex = (float4) tireMaskTexture[uint2((uint) (Vtx.uv.x * 4096.0f), (uint) (Vtx.uv.y * 4096.0f))];
        if (tiremasktex.x == 0)
        {
            TexColor = tiremasktex;
        }
        else if (tiremasktex.x != 1)
        {
            TexColor += tiremasktex;
            TexColor = normalize(TexColor);

        }
    }
    
    // 今発射されているレイのIDがGI用だったら
    if (PayloadData.rayID_ == CHS_IDENTIFICATION_RAYID_GI)
    {
        
        // レイの長さを求める。
        float rayLength = length(WorldRayOrigin() - WorldPos);
        
        // GIを表示するレイの長さの最大値。
        const float MAX_RAY = 300.0f;
        
        // レイの長さの割合。
        float rate = rayLength / MAX_RAY;
        rate = 1.0f - saturate(rate);
        
        float3 giBuff = (float3) TexColor * rate;
        
        // 当たったオブジェクトが完全反射だったらGIの色を黒くする。(完全反射で色がないから。黒は色として反映されない。)
        if (InstanceID == CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION)
        {
            giBuff = float3(0, 0, 0);
        }
        
        // 当たったオブジェクトが反射だったらGIの色を薄くする。
        if (InstanceID == CHS_IDENTIFICATION_ISNTANCE_REFLECTION)
        {
            giBuff /= 2.0f;
        }
        
        PayloadData.gi_ += giBuff;
        
        return true;
    }

    
    // 当たったオブジェクトが天球だったら。
    if (InstanceID == CHS_IDENTIFICATION_INSTNACE_AS)
    {
        float3 mieColor = float3(1, 1, 1);
        bool isUnderGround = false;
        
        // 影響度をかけつつ色を保存。
        PayloadData.light_ += float3(1, 1, 1);
        PayloadData.color_ += AtmosphericScattering(WorldPos, mieColor, isUnderGround) * PayloadData.impactAmount_;
        PayloadData.ao_ += 1;
        PayloadData.gi_ += float3(0, 0, 0);
        
        // マスクの色を白くする。(ライトリーク対策で他のマスクの色とかぶらないようにするため。)
        PayloadData.denoiseMask_ = float3(1, 1, 1);
        
        // サンプリングした点の輝度を取得する。
        float t = dot(PayloadData.color_.xyz, float3(0.2125f, 0.7154f, 0.0721f));
        
        // サンプリングした点が地上より下じゃなかったら。
        if (!isUnderGround)
        {
            
            // サンプリングした輝度をもとに、暗かったら星空を描画する。
            t = (1.0f - t);
            t = t == 0.0f ? 0.0f : pow(2.0f, 10.0f * t - 10.0f);
            t = t == 0.0f ? 0.0f : pow(2.0f, 10.0f * t - 10.0f);
            PayloadData.color_ += (float3) TexColor * t;
            PayloadData.color_ = saturate(PayloadData.color_);
            
        }
        
        // 影響度を0にする。
        PayloadData.impactAmount_ = 0.0f;
        
        return true;
    }
    
    // 当たったオブジェクトInstanceIDがテクスチャの色をそのまま返す or ライト用オブジェクトだったら
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_TEXCOLOR || InstanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT)
    {
        PayloadData.light_ += float3(1, 1, 1) * PayloadData.impactAmount_;
        PayloadData.color_ += (float3) TexColor * PayloadData.impactAmount_;
        PayloadData.ao_ += 1 * PayloadData.impactAmount_;
        PayloadData.gi_ += float3(0, 0, 0);
        
        // 影響度を0にする。
        PayloadData.impactAmount_ = 0.0f;
        
        return true;
    }
    
    // 当たったオブジェクトのInstanceIDが完全反射だったら。
    if (InstanceID == CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION)
    {
        
        // 完全反射用のレイを発射。
        ShootRay(CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION, WorldPos, reflect(WorldRayDirection(), WorldNormal), PayloadData, gRtScene);
        
        // 色を少しだけ明るくする。
        //PayloadData.color += 0.1f;
        
        PayloadData.color_ = saturate(PayloadData.color_);
        
        //// 残った影響度を入れる。
        //PayloadData.rayData_[RayDataIndex].impactRate_ = 0.0f;
        
        return true;
        
    }
    
    // メッシュ情報を返す。デバッグ機能。
    if (gSceneParam.debug.isMeshScene)
    {
        PayloadData.light_ = CalcBarycentrics(Attrib.barys);
        
        // 影響度を0にする。
        PayloadData.impactAmount_ = 0.0f;
        return true;
    }
    
    // 法線情報を返す。デバッグ機能。
    if (gSceneParam.debug.isNormalScene)
    {
        PayloadData.light_ = WorldNormal;
        
        // 影響度を0にする。
        PayloadData.impactAmount_ = 0.0f;
        return true;
    }
    
    return false;
    
}

// ライティング処理
bool Lighting(inout Payload PayloadData, float3 WorldPos, float3 WorldNormal, Vertex Vtx)
{
    
    // 乱数の種となる値を取得。
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    
    // 各光源の明るさ情報
    float pointLightVisibility = 0;
    float dirLightVisibility = 0;
    float aoLightVisibility = 0;
    
    // ポイントライトの色
    float3 pointLightColor = float3(0, 0, 0);
    
    // ディレクショナルライトの色
    float3 dirLightColor = float3(0, 0, 0);
    
    for (int index = 0; index < POINT_LIGHT_COUNT; ++index)
    {
        
        // ポイントライトが有効化されていなかったら処理を飛ばす。
        if (!gSceneParam.light.pointLight[index].isActive)
        {
            continue;
        }
        
        // 光源までの長さ
        float lightLength = length(gSceneParam.light.pointLight[index].lightPos - WorldPos);
    
        // 光源までの長さが点光源の強さより小さかったら処理を飛ばす。
        if (lightLength < gSceneParam.light.pointLight[index].lightPower && gSceneParam.light.pointLight[index].isActive)
        {
            
            // ライトの隠蔽度
            float pointLightVisibilityBuff = 0;
            if (!gSceneParam.light.pointLight[index].isShadow)
            {
                pointLightVisibilityBuff = SoftShadow(Vtx, gSceneParam.light.pointLight[index].lightSize, length(gSceneParam.light.pointLight[index].lightPos - WorldPos), index);
            }
            else
            {
                pointLightVisibilityBuff = 1;
            }
        
            // ライトの明るさが一定以上だったらスペキュラーなどを計算する。
            if (0 <= pointLightVisibilityBuff)
            {
                
                float3 pointLightDir = WorldPos - gSceneParam.light.pointLight[index].lightPos;
                pointLightDir = normalize(pointLightDir);
            
                // ライトまでの距離の割合
                float rate = lightLength / gSceneParam.light.pointLight[index].lightPower;
                rate = pow(rate, 5);
                rate = 1.0f - rate;
                
                // ディフーズの色
                float mDiffuse = material[0].diffuse.x;
                float3 diffuse = dot(-pointLightDir, WorldNormal) * mDiffuse;
			    // 光沢
                const float shininess = 4.0f;
		    	// 視点までの距離
                float3 eyedir = normalize(WorldRayOrigin() - WorldPos);
		    	// 反射ベクトル
                float3 reflect = normalize(pointLightDir + 2.0f * dot(-pointLightDir, WorldNormal) * WorldNormal);
                // スペキュラー
                float mSpecular = material[0].specular.x;
                float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
                
                // 最終結果の色
                pointLightVisibilityBuff = pointLightVisibilityBuff * ((diffuse.x + specular.x) * rate);
                
                pointLightColor += gSceneParam.light.pointLight[index].lightColor * pointLightVisibilityBuff;
            
                pointLightVisibility += pointLightVisibilityBuff;
                
            
            }
        
        }
    }
    
    
    
    pointLightColor = saturate(pointLightColor);
    pointLightVisibility = saturate(pointLightVisibility);
            
    // 太陽の位置とベクトル
    float3 sunPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;
    float3 sunDir = normalize(sunPos - WorldPos);
    
    // ディレクショナルライトの色
    if (gSceneParam.light.dirLight.isActive/* && gSceneParam.light.dirLight.lightDir.y < 0.2f*/)
    {
        
        // ディレクショナルライトの方向にレイを飛ばす。
        dirLightVisibility = ShootDirShadow(Vtx, 10000.0f);
        
        // ディレクショナルライトの明るさが一定以上だったら
        if (0 < dirLightVisibility && gSceneParam.light.dirLight.lightDir.y < 0.2f)
        {
        
            // ディレクショナルライトの色
            //dirLightColor = dirLightVisibility * gSceneParam.light.dirLight.lightColor;
            dirLightColor = float3(0, 0, 0);
            
            // ディフーズの色
            float mDiffuse = material[0].diffuse.x;
            float diffuse = dot(sunDir, WorldNormal) * mDiffuse;
            
			// 光沢
            const float shininess = 4.0f;
			// 視点ベクトル
            float3 eyedir = normalize(WorldRayOrigin() - WorldPos);
			// 反射ベクトル
            float3 reflect = normalize(-sunDir + 2.0f * dot(sunDir, WorldNormal) * WorldNormal);
            // スペキュラー
            float mSpecular = material[0].specular.x;
            float specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // 最終結果の色
            dirLightVisibility = diffuse + specular;
            
            const float SKYDOME_RADIUS = 15000.0f;
            const float SAMPLING_POS_Y = 0.0f;
            
            // 天球の色をサンプリング
            //float3 samplingVec = normalize(float3(1.0f, 0.1f, 0.1f)) * SKYDOME_RADIUS;
            float3 samplingVec = normalize(-gSceneParam.light.dirLight.lightDir * float3(1.0f, 0.0f, 1.0f)) * SKYDOME_RADIUS;
            samplingVec.y = 0.1f;
            
            // サンプリングするベクトル
            samplingVec.y = SAMPLING_POS_Y;
            samplingVec = normalize(samplingVec);
            
            // サンプリングする座標
            float3 samplingPos;
            samplingPos = samplingVec * SKYDOME_RADIUS;
            
            // 大気散乱の色
            float3 mieColor = float3(1, 1, 1);
            bool isUnderGround = false;
            float3 skydomeColor = AtmosphericScattering(samplingPos, mieColor, isUnderGround);
            
            dirLightColor += mieColor * float3(1.0f, 0.5f, 0.5f);
            
            dirLightColor = normalize(dirLightColor);
            
            
        }
        else
        {
            dirLightColor = float3(0, 0, 0);
            dirLightVisibility = 0;
        }
        
    }
    
        
    // AOを行わないデバッグ機能がついていたらAOを行わない。
    if (!gSceneParam.debug.isNoAO)
    {
       
        int seed = InitRand(DispatchRaysIndex().x + (WorldPos.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100, 16);
        float3 sampleDir = GetUniformHemisphereSample(seed, WorldNormal);
        
        // AOのレイを飛ばす。
        float aoLightVisibilityBuff = ShootAOShadowRay(WorldPos, sampleDir, 10, gRtScene);
        
        float NoL = saturate(dot(WorldNormal, sampleDir));
        float pdf = 1.0 / (2.0 * PI);
        aoLightVisibility += aoLightVisibilityBuff;
        
    }
        
   
        
    
    
    // ライトの総合隠蔽度を求める。
    float lightVisibility = 0;
    lightVisibility += pointLightVisibility + dirLightVisibility;
    float aoVisibility = aoLightVisibility;
    
    // 各色を設定。
    PayloadData.light_ += lightVisibility + (pointLightColor + dirLightColor) / PI;
    PayloadData.ao_ += aoVisibility;
    
    
    
    // デバッグ機能でライトに当たった位置のみを描画するフラグが立っていたら。
    if (gSceneParam.debug.isLightHitScene)
    {
        
        if (0.0f < lightVisibility)
        {
            PayloadData.color_ = float3(1, 1, 1);
            PayloadData.light_ = float3(1, 1, 1);
        }
        else
        {
            PayloadData.color_ = float3(0, 0, 0);
            PayloadData.light_ = float3(0, 0, 0);
        }
        
        PayloadData.impactAmount_ = 0;
        
        return true;
        
    }
    
    return false;
    
}

// ライティング後処理
void ProccessingAfterLighting(inout Payload PayloadData, Vertex Vtx, float3 WorldPos, float3 WorldNormal, inout float4 TexColor, uint InstanceID)
{
    
    // デフォルトのレイだったら。
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF)
    {
        
        PayloadData.color_.xyz += (float3) TexColor * PayloadData.impactAmount_;
        PayloadData.impactAmount_ = 0;
        
    }
    
    // 当たったオブジェクトがGIを行うオブジェクトで、GIを行うフラグが立っていたら。
    if ((InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_GI || InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK) && !gSceneParam.debug.isNoGI)
    {
        if (0.0f < PayloadData.impactAmount_)
        {
            ShootGIRay(Vtx, 300, PayloadData);
            PayloadData.gi_ = (PayloadData.gi_ * PayloadData.impactAmount_) * (material[0].specular / 2.0f);
            PayloadData.gi_ = saturate(PayloadData.gi_);
            PayloadData.color_.xyz += (float3) TexColor * PayloadData.impactAmount_;
        
            PayloadData.impactAmount_ = 0.0f;
        }
        
    }
    
    // GIのみを描画するフラグがたったら。
    if (gSceneParam.debug.isGIOnlyScene)
    {
        
        PayloadData.light_ = float3(1, 1, 1);
        PayloadData.color_ = float3(0, 0, 0);
        PayloadData.ao_ = 0;
        
        PayloadData.impactAmount_ = 0;
        
        return;
        
    }
    
    
    // 当たったオブジェクトのInstanceIDが反射だったら
    if (InstanceID == CHS_IDENTIFICATION_ISNTANCE_REFLECTION)
    {
     
        if (PayloadData.impactAmount_ < 0.4f)
        {
            PayloadData.color_.xyz += (float3) TexColor * PayloadData.impactAmount_;
            PayloadData.impactAmount_ = 0.0f;
        }
        else
        {
            PayloadData.color_.xyz += (float3) TexColor * 0.4f;
            PayloadData.impactAmount_ -= 0.4f;
            
            if (0.0f < PayloadData.impactAmount_)
            {
                
                // 反射レイを飛ばす。
                ShootRay(CHS_IDENTIFICATION_RAYID_RECLECTION, WorldPos, reflect(WorldRayDirection(), WorldNormal), PayloadData, gRtScene);
                
            }
        
        }
        
    }
    
    // 当たったオブジェクトのInstanceIDが屈折だったら
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_REFRACTION)
    {
        
        if (PayloadData.impactAmount_ < 0.3f)
        {
            PayloadData.color_.xyz += (float3) TexColor * PayloadData.impactAmount_;
            PayloadData.impactAmount_ = 0.0f;

        }
        else
        {
            PayloadData.color_.xyz += (float3) TexColor * 0.3f;
            PayloadData.impactAmount_ -= 0.3f;
        }

        float refractVal = 1.4f;
        float3 rayDir = float3(0, 0, 0);

        float nr = dot(WorldNormal, WorldRayDirection());
        if (nr < 0)
        {

            // 空気中->オブジェクト
            float eta = 1.0f / refractVal;
            rayDir = refract(WorldRayDirection(), WorldNormal, eta);

        }
        else
        {

            // オブジェクト->空気中
            float eta = refractVal / 1.0f;
            rayDir = refract(WorldRayDirection(), -WorldNormal, eta);
      
        }
            
        if (0.0f < PayloadData.impactAmount_)
        {
                
            ShootRay(CHS_IDENTIFICATION_RAYID_REFRACTION, WorldPos, rayDir, PayloadData, gRtScene);
            
        }

    }
    
    
    // 当たったオブジェクトのInstanceIDがアルファだったら
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_ALPHA || InstanceID == CHS_IDENTIFICATION_INSTANCE_ADD)
    {
        
        // アルファ値を求める。
        int instanceIndex = InstanceIndex();
        float alpha = 0;
        for (int alphaIndex = 0; alphaIndex < ALPHA_DATA_COUNT; ++alphaIndex)
        {
            if (gSceneParam.alphaData_.alphaData_[alphaIndex].instanceIndex_ != instanceIndex)
            {
                continue;
            }
            alpha = gSceneParam.alphaData_.alphaData_[alphaIndex].alpha_;
            break;
        }
        
        // アルファブレンドだったら
        if (InstanceID == CHS_IDENTIFICATION_INSTANCE_ALPHA)
        {
        
            if (PayloadData.impactAmount_ < alpha * TexColor.w)
            {
                PayloadData.color_.xyz += (float3) TexColor * PayloadData.impactAmount_;
                PayloadData.light_ += float3(1 * PayloadData.impactAmount_, 1 * PayloadData.impactAmount_, 1 * PayloadData.impactAmount_);
                PayloadData.impactAmount_ = 0.0f;

            }
            else
            {
                PayloadData.color_.xyz += (float3) TexColor * alpha;
                PayloadData.light_ += float3(1 * alpha * TexColor.w, 1 * alpha * TexColor.w, 1 * alpha * TexColor.w);
                PayloadData.impactAmount_ -= alpha * TexColor.w;
            }
            
        }
        // 加算合成だったら
        else if (InstanceID == CHS_IDENTIFICATION_INSTANCE_ADD)
        {
            
            PayloadData.light_ = float3(TexColor.w, TexColor.w, TexColor.w);
            
            if (PayloadData.impactAmount_ < alpha * TexColor.w)
            {
                PayloadData.color_.xyz += (float3) TexColor;
                PayloadData.light_ += float3(1 * PayloadData.impactAmount_, 1 * PayloadData.impactAmount_, 1 * PayloadData.impactAmount_);
                PayloadData.impactAmount_ = 0.0f;

            }
            else
            {
                PayloadData.color_.xyz += (float3) TexColor;
                PayloadData.light_ += float3(1 * alpha * TexColor.w, 1 * alpha * TexColor.w, 1 * alpha * TexColor.w);
                PayloadData.impactAmount_ -= alpha * TexColor.w;
            }
        }
        
        // アルファが一定以下だったら。
        if (alpha < 0.5f)
        {
            ++PayloadData.alphaCounter_;
            if (3 <= PayloadData.alphaCounter_)
            {
                PayloadData.isCullingAlpha_ = true;
            }
        }
            
        if (0.0f < PayloadData.impactAmount_)
        {
                
            // 反射レイを飛ばす。
            ShootRay(CHS_IDENTIFICATION_RAYID_DEF, WorldPos, WorldRayDirection(), PayloadData, gRtScene);
            
        }
        
    }
    
    // 当たったオブジェクトのInstanceIDが屈折アルファだったら。
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_REFRACTION_ALPHA)
    {
        
        // アルファ値を求める。
        int instanceIndex = InstanceIndex();
        float alpha = 0;
        for (int alphaIndex = 0; alphaIndex < ALPHA_DATA_COUNT; ++alphaIndex)
        {
            if (gSceneParam.alphaData_.alphaData_[alphaIndex].instanceIndex_ != instanceIndex)
            {
                continue;
            }
            alpha = gSceneParam.alphaData_.alphaData_[alphaIndex].alpha_;
            break;
        }
        
        // payloadに入れる色を計算する。
        if (PayloadData.impactAmount_ < alpha * TexColor.w)
        {
            PayloadData.color_.xyz += (float3) TexColor * PayloadData.impactAmount_;
            PayloadData.light_ += float3(1 * PayloadData.impactAmount_, 1 * PayloadData.impactAmount_, 1 * PayloadData.impactAmount_);
            PayloadData.impactAmount_ = 0.0f;

        }
        else
        {
            PayloadData.color_.xyz += (float3) TexColor * alpha;
            PayloadData.light_ += float3(1 * alpha * TexColor.w, 1 * alpha * TexColor.w, 1 * alpha * TexColor.w);
            PayloadData.impactAmount_ -= alpha * TexColor.w;
        }

        
        // アルファが一定以下だったら。
        if (alpha < 0.5f)
        {
            ++PayloadData.alphaCounter_;
            if (3 <= PayloadData.alphaCounter_)
            {
                PayloadData.isCullingAlpha_ = true;
            }
        }
            
        if (0.0f < PayloadData.impactAmount_)
        {
            
            float refractVal = 1.4f * (alpha * TexColor.w);
            float3 rayDir = float3(0, 0, 0);

            float eta = 1.0f / refractVal;
            rayDir = refract(WorldRayDirection(), WorldNormal, eta);
                
            // 反射レイを飛ばす。
            ShootRay(CHS_IDENTIFICATION_RAYID_DEF, WorldPos, rayDir, PayloadData, gRtScene);
            
        }
        
    }
    
}

// closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]

    void mainCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
    
    // 影用レイだったら。
    if (payload.rayID_ == CHS_IDENTIFICATION_RAYID_SHADOW)
    {
        
        if (InstanceID() == CHS_IDENTIFICATION_INSTANCE_ALPHA || InstanceID() == CHS_IDENTIFICATION_INSTANCE_ADD)
        {
                
            payload.impactAmount_ = 1;
            
            return;

        }
        
        if (InstanceID() == CHS_IDENTIFICATION_INSTANCE_REFRACTION)
        {
            
            payload.impactAmount_ = 0.3f;
            
            return;

        }
                
        payload.impactAmount_ = false;
        
        return;
    }

    // 呼び出し回数が制限を超えないようにする。
    ++payload.recursive_;
    if (5 < payload.recursive_ || payload.impactAmount_ <= 0 || payload.isCullingAlpha_)
    {
        
        if (0.0f < payload.impactAmount_)
        {
            
            //payload.color_ = normalize(payload.color_);
            
        }
        
        return;
    }
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldNormal = normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3()));

    // テクスチャの色を取得。
    float4 texColor = (float4) texture.SampleLevel(smp, vtx.uv, 0.0f);
    texColor.xyz *= texColor.w;
    
    // 法線マップの色を取得。
    float3 normalMapColor = (float3) normalTexture.SampleLevel(smp, vtx.uv, 0.0f);
    
    // 法線マップの色とテクスチャの色が同じじゃなかったら法線マップの色を再取得。(法線マップがないテクスチャにはメモリの隙間を埋めるために一応テクスチャをいれているから。)
    if (!(texColor.x == normalMapColor.x && texColor.y == normalMapColor.y && texColor.z == normalMapColor.z))
    {
        worldNormal = normalize(mul(normalMapColor, (float3x3) ObjectToWorld4x3()));
        
        // 当たったメッシュの情報を取得。
        Vertex meshInfo[3];
        GetHitMeshInfo(attrib, vertexBuffer, indexBuffer, meshInfo);
        
        // 接空間変換用
        float3 tan;
        float3 bnorm;
        CalcTangentAndBinormal(meshInfo[0].Position, meshInfo[1].Position, meshInfo[2].Position, meshInfo[0].uv, meshInfo[1].uv, meshInfo[2].uv, tan, bnorm);
        
        // 説空間行列を求める。
        float4x4 mat =
        {
            float4(tan, 0.0f),
            float4(bnorm, 0.0f),
            float4(vtx.Normal, 0.0f),
            { 0, 0, 0, 1 }
        };
      //  mat = transpose(mat); // 転置
        
        worldNormal = mul(worldNormal, mat);

    }
    

    // ライティング前の処理を実行。----- 全反射オブジェクトやテクスチャの色をそのまま使うオブジェクトの色取得処理。
    if (ProcessingBeforeLighting(payload, vtx, attrib, worldPos, worldNormal, texColor, instanceID))
    {
        return;
    }
    

    // ライティングの処理
    if (instanceID != CHS_IDENTIFICATION_INSTANCE_ALPHA)
    {
        if (Lighting(payload, worldPos, worldNormal, vtx))
        {
            return;
        }
    }
    
    // ライティング後の処理を実行。 ----- ライティングの結果を合成する処理や反射や屈折等の再びレイを飛ばす必要があるオブジェクトの処理。
    ProccessingAfterLighting(payload, vtx, worldPos, worldNormal, texColor, instanceID);
    
}

// 影用CHS 使用していない。
[shader("closesthit")]

    void shadowCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
}

// AnyHitShader
[shader("anyhit")]

    void mainAnyHit
    (inout
    Payload payload, MyAttribute
    attrib)
{
        
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    float4 diffuse = texture.SampleLevel(smp, vtx.uv, 0);
    if (diffuse.w < 0.1f)
    {
        IgnoreHit();

    }
    
    int instanceID = InstanceID();
    
    // 当たったオブジェクトのInstanceIDがライト用or不可視のオブジェクトだったら当たり判定を棄却
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT || instanceID == CHS_IDENTIFICATION_INSTANCE_INVISIBILITY)
    {
        IgnoreHit();

    }
    
    // 当たったオブジェクトのInstanceIDがアルファだったら
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_ALPHA || instanceID == CHS_IDENTIFICATION_INSTANCE_ADD)
    {
        
        // 一定以上薄いアルファ値のオブジェクトとあたっていたら。
        if (payload.isCullingAlpha_)
        {
            IgnoreHit();

        }
        
        if (payload.rayID_ == CHS_IDENTIFICATION_RAYID_SHADOW)
        {
            IgnoreHit();

        }
        
        // アルファ値を求める。
        int instanceIndex = InstanceIndex();
        float alpha = 0;
        for (int alphaIndex = 0; alphaIndex < ALPHA_DATA_COUNT; ++alphaIndex)
        {
            if (gSceneParam.alphaData_.alphaData_[alphaIndex].instanceIndex_ != instanceIndex)
            {
                continue;
            }
            alpha = gSceneParam.alphaData_.alphaData_[alphaIndex].alpha_;
            break;
        }
        
        // テクスチャの色を取得。
        float4 texColor = (float4) texture.SampleLevel(smp, vtx.uv, 0.0f);
        texColor.xyz *= texColor.w;
        
        alpha *= texColor.w;
        
        if (alpha <= 0.1f)
        {
            IgnoreHit();

        }
        
    }
    
    //AcceptHitAndEndSearch();
    
}