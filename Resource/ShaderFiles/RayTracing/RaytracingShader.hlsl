#include "RaytracingShaderHeader.hlsli"

// TLAS
RaytracingAccelerationStructure gRtScene : register(t0);
Texture2D<float4> debugMaskTexture : register(t1);
ConstantBuffer<ConstBufferData> gSceneParam : register(b0);

// 各リソース等
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
StructuredBuffer<Material> material : register(t2, space1);
Texture2D<float4> texture : register(t3, space1);
Texture2D<float4> mapTexture : register(t4, space1);
RWTexture2D<float4> tireMaskTexture : register(u0, space1);
// サンプラー
SamplerState smp : register(s0, space1);

// 出力先UAV
RWTexture2D<float4> lightingOutput : register(u0);
RWTexture2D<float4> colorOutput : register(u1);
RWTexture2D<float4> denoiseMaskoutput : register(u2);
RWTexture2D<float4> emissiveOutput : register(u3);

// 大気散乱
float16_t3 AtmosphericScattering(float3 pos, inout float16_t3 mieColor)
{

    // レイリー散乱定数
    float16_t kr = 0.0025f;
    // ミー散乱定数
    float16_t km = 0.005f;

    // 大気中の線分をサンプリングする数。
    float16_t fSamples = 2.0f;

    // 謎の色 色的には薄めの茶色
    float16_t3 three_primary_colors = float3(0.68f, 0.55f, 0.44f);
    // 光の波長？
    float16_t3 v3InvWaveLength = 1.0f / pow(three_primary_colors, 4.0f);

    // 大気圏の一番上の高さ。
    float16_t fOuterRadius = 10250.0f;
    // 地球全体の地上の高さ。
    float16_t fInnerRadius = 10200.0f;

    // 太陽光の強さ？
    float16_t fESun = 10.0f;
    // 太陽光の強さにレイリー散乱定数をかけてレイリー散乱の強さを求めている。
    float16_t fKrESun = kr * fESun;
    // 太陽光の強さにミー散乱定数をかけてレイリー散乱の強さを求めている。
    float16_t fKmESun = km * fESun;

    // レイリー散乱定数に円周率をかけているのだが、限りなく0に近い値。
    float16_t fKr4PI = kr * 4.0f * PI;
    // ミー散乱定数に円周率をかけているのだが、ミー散乱定数は0なのでこれの値は0。
    float16_t fKm4PI = km * 4.0f * PI;

    // 地球全体での大気の割合。
    float16_t fScale = 1.0f / (fOuterRadius - fInnerRadius);
    // 平均大気密度を求める高さ。
    float16_t fScaleDepth = 0.35f;
    // 地球全体での大気の割合を平均大気密度で割った値。
    float16_t fScaleOverScaleDepth = fScale / fScaleDepth;

    // 散乱定数を求める際に使用する値。
    float16_t g = -0.999f;
    // 散乱定数を求める際に使用する値を二乗したもの。なぜ。
    float16_t g2 = g * g;

    // 当たった天球のワールド座標
    float3 worldPos = normalize(pos) * fOuterRadius;
    worldPos = IntersectionPos(normalize(worldPos), float3(0.0, fInnerRadius, 0.0), fOuterRadius);

    // カメラ座標 元計算式だと中心固定になってしまっていそう。
    float3 v3CameraPos = float3(0.0, fInnerRadius + 1.0f, 0.0f);

    // ディレクショナルライトの場所を求める。
    float3 dirLightPos = -gSceneParam.light.dirLight.lightDir * 1000000.0f;

    // ディレクショナルライトへの方向を求める。
    float16_t3 v3LightDir = normalize(dirLightPos - worldPos);

    // 天球上頂点からカメラまでのベクトル(光が大気圏に突入した点からカメラまでの光のベクトル)
    float3 v3Ray = worldPos - v3CameraPos;

    // 大気に突入してからの点とカメラまでの距離。
    float16_t fFar = length(v3Ray);

    // 正規化された拡散光が来た方向。
    v3Ray /= fFar;

    // サンプリングする始点座標 資料だとAの頂点
    float3 v3Start = v3CameraPos;
    // サンプルではカメラの位置が(0,Radius,0)なのでカメラの高さ。どの位置に移動しても地球視点で見れば原点(地球の中心)からの高さ。
    float16_t fCameraHeight = length(v3CameraPos);
    // 地上からの法線(?)と拡散光がやってきた角度の内積によって求められた角度をカメラの高さで割る。
    float16_t fStartAngle = dot(v3Ray, v3Start) / fCameraHeight;
    // 開始地点の高さに平均大気密度をかけた値の指数を求める？
    float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    // 開始地点のなにかの角度のオフセット。
    float fStartOffset = fStartDepth * Scale(fStartAngle);

    // サンプルポイント間の長さ。
    float fSampleLength = fFar / fSamples;
    // サンプルポイント間の長さに地球の大気の割合をかける。
    float fScaledLength = fSampleLength * fScale;
    // 拡散光が来た方向にサンプルの長さをかけることでサンプルポイント間のレイをベクトルを求める。
    float16_t3 v3SampleRay = v3Ray * fSampleLength;
    // 最初のサンプルポイントを求める。0.5をかけてるのは少し動かすため？
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;

    // 色情報
    float16_t3 v3FrontColor = 0.0f;
    for (int n = 0; n < int(fSamples); ++n)
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
        float16_t3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI));
        // サンプルポイントの位置を考慮して散乱した色を求める。
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        // サンプルポイントを移動させる。
        v3SamplePoint += v3SampleRay;

    }

    // レイリー散乱に使用する色情報
    float16_t3 c0 = v3FrontColor * (v3InvWaveLength * fKrESun);
    // ミー散乱に使用する色情報
    float16_t3 c1 = v3FrontColor * fKmESun;
    // カメラ座標から天球の座標へのベクトル。
    float3 v3Direction = v3CameraPos - worldPos;

    //float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float16_t fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float16_t fcos2 = fcos * fcos;

    // レイリー散乱の明るさ。
    float16_t rayleighPhase = 0.75f * (1.0f + fcos2);
    // ミー散乱の明るさ。
    float16_t miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fcos2) / pow(1.0f + g2 - 2.0f * g * fcos, 1.5f);

    // ミー散乱の色を保存。
    mieColor = c0 * rayleighPhase;

    // 最終結果の色
    float16_t3 col = 1.0f;
    col.rgb = rayleighPhase * c0 + miePhase * c1;

    // 交点までのベクトルと太陽までのベクトルが近かったら白色に描画する。
    int sunWhite = step(0.999f, dot(normalize(dirLightPos - v3CameraPos), normalize(worldPos - v3CameraPos)));
    
    return col + float16_t3(sunWhite, sunWhite, sunWhite);

}

// 太陽光の影チェック用レイの準備関数 戻り値は太陽光の色
float ShootDirShadow(Vertex vtx, float length)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());

    // 並行光源の座標を仮で求める。
    float3 dirLightPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;

    // 並行光源までのベクトル。
    float3 dirLightVec = dirLightPos - worldPosition;
    dirLightVec = normalize(dirLightVec);

    float3 shadowRayDir = dirLightVec;
    
    return ShootShadowRayNoAH(worldPosition + normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3())) * 2.0f, shadowRayDir, length, gRtScene);
    
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
    payloadData.light_ = float3(0, 0, 0);
    payloadData.emissive_ = float3(0, 0, 0);
    payloadData.isCullingAlpha_ = false;
    payloadData.alphaCounter_ = 0;
    payloadData.roughnessOffset_ = 1.0f;
    payloadData.pad_ = 1.0f;

    // TransRayに必要な設定を作成
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    
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

    // Linear -> sRGB
    payloadData.light_ = gSceneParam.light.dirLight.lightColor.y * pow(payloadData.light_, 1.0f / gSceneParam.light.dirLight.lightColor.x) - 0.055f;
    payloadData.ao_ = gSceneParam.light.dirLight.lightColor.y * pow(payloadData.ao_, 1.0f / gSceneParam.light.dirLight.lightColor.x) - 0.055f;
    
    
    payloadData.emissive_ += payloadData.light_ * step(2.0f, length(payloadData.light_));

    // 結果格納
    lightingOutput[launchIndex.xy] = float4((payloadData.light_), 1);
    colorOutput[launchIndex.xy] = float4((payloadData.color_), payloadData.ao_);
    denoiseMaskoutput[launchIndex.xy] = float4(payloadData.denoiseMask_, 1);
    emissiveOutput[launchIndex.xy] = float4(payloadData.emissive_, 1);

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
    
    if (PayloadData.rayID_ == CHS_IDENTIFICATION_RAYID_SHADOW || PayloadData.rayID_ == CHS_IDENTIFICATION_RAYID_AO_SHADOW)
        return;
    
    // ペイロード受け取り用変数。
    Payload payloadBuff = PayloadData;
    
    // 影響度をかけつつ色を保存。
    float3 mieColor = float3(1, 1, 1);
    payloadBuff.light_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
    payloadBuff.color_ += AtmosphericScattering(WorldRayOrigin() + WorldRayDirection() * RayTCurrent(), mieColor) * payloadBuff.impactAmount_ * payloadBuff.impactAmount_;
    payloadBuff.ao_ += 1.0f * payloadBuff.impactAmount_;
        
    // マスクの色を白くする。(ライトリーク対策で他のマスクの色とかぶらないようにするため。)
    payloadBuff.denoiseMask_ = float3(1, 1, 1);
        
    // サンプリングした点の輝度を取得する。
    float t = dot(payloadBuff.color_.xyz, float3(0.2125f, 0.7154f, 0.0721f));
        
    // 影響度を0にする。
    payloadBuff.impactAmount_ = 0.0f;
        
    PayloadData = payloadBuff;

}

// シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout Payload payload)
{
    // 何にも当たっていないということなので、影は生成しない。
    payload.impactAmount_ = 1.0f;
}

// ライティング前処理
bool ProcessingBeforeLighting(inout Payload PayloadData, Vertex Vtx, MyAttribute Attrib, float3 WorldPos, float3 WorldNormal, float3 NormalMap, inout float4 TexColor, uint InstanceID)
{
    
    // ペイロード受け取り用変数。
    Payload payloadBuff = PayloadData;
    
    // デノイズ用のマスクに使用するテクスチャに法線の色とInstanceIndexをかけたものを書き込む。
    if (payloadBuff.recursive_ == 1)
    {
        payloadBuff.denoiseMask_ = (NormalMap);
    }
    
    // エミッシブだったらエミッシブマップに書き込む。
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_EMISSIVE)
    {
        payloadBuff.emissive_ += TexColor * payloadBuff.impactAmount_;
    }
    
    // InstanceIDがCHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASKだったらテクスチャに色を加算。
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK || InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_TIREMASK || InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_TIREMASK_AO)
    {
        float4 tiremasktex = (float4) tireMaskTexture[uint2((uint) (Vtx.subUV.x * 4096.0f), (uint) (Vtx.subUV.y * 4096.0f))];
        TexColor += tiremasktex * tiremasktex.a;
        TexColor = normalize(TexColor);
    }
    
    // 当たったオブジェクトInstanceIDがテクスチャの色をそのまま返す or ライト用オブジェクトだったら
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_TEXCOLOR || InstanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT)
    {
        payloadBuff.light_ += float3(1.0f, 1.0f, 1.0f) * payloadBuff.impactAmount_;
        payloadBuff.color_ += (float3) TexColor * payloadBuff.impactAmount_;
        payloadBuff.ao_ += 1.0f * payloadBuff.impactAmount_;
        payloadBuff.emissive_ += payloadBuff.color_ * payloadBuff.impactAmount_;
        
        // 影響度を0にする。
        payloadBuff.impactAmount_ = 0.0f;
        
        PayloadData = payloadBuff;
        
        return true;
    }
    
    PayloadData = payloadBuff;
    
    return false;
    
}

// UE4のGGX分布
float DistributionGGX(float16_t Alpha, float16_t NdotH)
{
    float16_t alpha2 = Alpha * Alpha;
    float16_t t = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * t * t);
}

// Schlickによるフレネルの近似 
float16_t SchlickFresnel(float16_t F0, float16_t F90, float16_t Cosine)
{
    float16_t m = saturate(1.0f - Cosine);
    float16_t m2 = m * m;
    float16_t m5 = m2 * m2 * m;
    return lerp(F0, F90, m5);
}
float16_t3 SchlickFresnel3(float16_t3 F0, float16_t3 F90, float16_t Cosine)
{
    float16_t m = saturate(1.0f - Cosine);
    float16_t m2 = m * m;
    float16_t m5 = m2 * m2 * m;
    return lerp(F0, F90, m5);
}

// ディズニーのフレネル計算
float16_t3 DisneyFresnel(float16_t LdotH, float16_t3 BaseColor)
{
    
    // 輝度
    float16_t luminance = 0.3f * BaseColor.r + 0.6f * BaseColor.g + 0.1f * BaseColor.b;
    // 色合い
    float16_t3 tintColor = BaseColor / luminance;
    // 非金属の鏡面反射色を計算
    float16_t3 nonMetalColor = material[0].specular_ * 0.08f * tintColor;
    // metalnessによる色補完 金属の場合はベースカラー
    float16_t3 specularColor = lerp(nonMetalColor, BaseColor, material[0].metalness_);
    // NdotHの割合でSchlickFresnel補間
    return SchlickFresnel3(specularColor, float16_t3(1.0f, 1.0f, 1.0f), LdotH);
    
}

// UE4のSmithモデル
float16_t GeometricSmith(float16_t Cosine)
{
    float16_t k = (material[0].roughness_ + 1.0f);
    k = k * k / 8.0f;
    return Cosine / (Cosine * (1.0f - k) + k);
}

// 鏡面反射の計算
float16_t3 CookTorranceSpecular(float16_t NdotL, float16_t NdotV, float16_t NdotH, float16_t LdotH, float16_t3 BaseColor)
{
    
    // D項(分布:Distribution)
    float16_t Ds = DistributionGGX(material[0].roughness_ * material[0].roughness_, NdotH);
    
    // F項(フレネル:Fresnel)
    float16_t3 Fs = DisneyFresnel(LdotH, BaseColor);
    
    // G項(幾何減衰:Geometry attenuation)
    float16_t Gs = GeometricSmith(NdotL) * GeometricSmith(NdotV);
    
    // M項(分母)
    float16_t m = 4.0f * NdotL * NdotV;
    
    // 合成して鏡面反射の色を得る。
    return Ds * Fs * Gs / m;
    
}

// 双方向反射分布関数
float16_t3 BRDF(float16_t3 LightVec, float16_t3 ViewVec, float16_t3 Normal, float16_t3 BaseColor)
{
    // 法線とライト方向の内積
    float16_t NdotL = dot(Normal, LightVec);
    
    // 法線とカメラ方向の内積
    float16_t NdotV = dot(Normal, ViewVec);
    
    // ライト方向とカメラ方向の中間であるハーフベクトル
    float16_t3 floatVec = normalize(LightVec + ViewVec);
    
    // 法線とハーフベクトルの内積
    float16_t NdotH = dot(Normal, floatVec);
    
    // ライトとハーフベクトルの内積
    float16_t LdotH = dot(LightVec, floatVec);
    
    // 拡散反射率
    float16_t diffuseReflectance = 1.0f / PI;
    
    // 入射角が90度の場合の拡散反射率
    float16_t energyBias = 0.5f * material[0].roughness_;
    float16_t FD90 = energyBias + 2.0f * LdotH * LdotH * material[0].roughness_;
    
    // 入っていくときの拡散反射率
    float16_t FL = SchlickFresnel(1.0f, FD90, NdotL);
    
    // 出ていくときの拡散反射率
    float16_t FV = SchlickFresnel(1.0f, FD90, NdotV);
    
    // 入って出ていくまでの拡散反射率
    float16_t energyFactor = lerp(1.0f, 1.0f / 1.51f, material[0].roughness_);
    float16_t FD = FL * FV * energyFactor;
    
    // 拡散反射項
    float16_t3 diffuseColor = diffuseReflectance * FD * BaseColor * (1.0f - material[0].metalness_);
    
    // 鏡面反射項
    float16_t3 specularColor = CookTorranceSpecular(NdotL, NdotV, NdotH, LdotH, BaseColor);
    
    return diffuseColor + specularColor;
    
}

// ライティング処理
bool Lighting(inout Payload PayloadData, float3 WorldPos, float3 NormalMap, Vertex Vtx, float4 TexColor)
{
    
    // Payload一時受け取り用変数。
    Payload payloadBuff = PayloadData;
    
    // 乱数の種となる値を取得。
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
            
    // 太陽の位置とベクトル
    float3 sunPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;
    float3 sunDir = normalize(sunPos - WorldPos);
    
    // ディレクショナルライトの方向にレイを飛ばす。
    float dirLightVisibility = ShootDirShadow(Vtx, 10000.0f);
    
    // 大気散乱の色
    float16_t3 skydomeColor;
    float16_t3 mieColor = float3(1, 1, 1);
    
    // ディレクショナルライトの明るさが一定以上だったら
    if (0.0f < dirLightVisibility)
    {
    
        const float SKYDOME_RADIUS = 15000.0f;
        const float16_t SAMPLING_POS_Y = 0.0f;
        
        // 天球の色をサンプリング
        float3 samplingVec = normalize(-gSceneParam.light.dirLight.lightDir) * SKYDOME_RADIUS;
        
        // サンプリングするベクトル
        samplingVec.y = SAMPLING_POS_Y;
        samplingVec = normalize(samplingVec);
        
        // サンプリングする座標
        float3 samplingPos;
        samplingPos = samplingVec * SKYDOME_RADIUS;
        
        // 大気散乱を計算。
        skydomeColor = AtmosphericScattering(samplingPos, mieColor);
        payloadBuff.light_ += BRDF(-gSceneParam.light.dirLight.lightDir, -WorldRayDirection(), NormalMap, float3(1, 1, 1)) * dirLightVisibility * PayloadData.impactAmount_;
        //payloadBuff.color_ = float3(1, 1, 1);
        
        
    }
    else
    {
        //payloadBuff.light_ += float3(1, 0, 0);
    }
        
    
    // AOの計算。 一定以上の距離の場合はAOの計算を行わない。
    float offsetAO = 0.2f;
    uint instanceID = InstanceID();
    if (500.0f < RayTCurrent() || payloadBuff.rayID_ == CHS_IDENTIFICATION_RAYID_RECLECTION)
    {
        payloadBuff.light_ += mieColor * offsetAO * payloadBuff.impactAmount_;
    }
    else if (instanceID == CHS_IDENTIFICATION_INSTANCE_DEF_TIREMASK_AO || instanceID == CHS_IDENTIFICATION_INSTANCE_DEF_AO)
    {
        
        float aoLightVisibilityBuff = ShootAOShadowRay(WorldPos, normalize(mul(Vtx.Normal, (float3x3) ObjectToWorld4x3())), 50, gRtScene);
        
        // 各光源の明るさ情報
        float aoLightVisibility = 0;
        aoLightVisibility += aoLightVisibilityBuff;
        aoLightVisibility = clamp(aoLightVisibility, offsetAO, 1.0f);
    
        // ライトの総合隠蔽度を求める。
        float aoVisibility = aoLightVisibility;
    
        // 各色を設定。
        payloadBuff.light_ += mieColor * offsetAO * payloadBuff.impactAmount_;
        payloadBuff.light_ -= (1.0f - aoVisibility) * offsetAO * payloadBuff.impactAmount_;
        
    }
    else
    {

        payloadBuff.light_ += mieColor * offsetAO * payloadBuff.impactAmount_;
        
    }
    
    
    PayloadData = payloadBuff;
    
    return false;
    
}

// ライティング後処理
void ProccessingAfterLighting(inout Payload PayloadData, Vertex Vtx, float3 WorldPos, float3 NormalMap, float4 DefTexColor, inout float4 TexColor, uint InstanceID)
{
    
    // Payload一時受け取り用変数。
    Payload payloadBuff = PayloadData;
    
    
    // エミッシブだったらエミッシブマップに書き込む。
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_TEXCOLOR_REFLECTION_EMISSIVE)
    {
        payloadBuff.emissive_ += TexColor * payloadBuff.impactAmount_;
        payloadBuff.light_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
    }
        
    // 金属度
    float metalness = 1.0f - material[0].metalness_;
    
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
        
            if (payloadBuff.impactAmount_ < alpha * TexColor.w)
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w * payloadBuff.impactAmount_;
                payloadBuff.light_ += float3(1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_);
                payloadBuff.impactAmount_ = 0.0f;

            }
            else
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w * alpha;
                payloadBuff.light_ += float3(1 * alpha * TexColor.w, 1 * alpha * TexColor.w, 1 * alpha * TexColor.w);
                payloadBuff.impactAmount_ -= alpha * TexColor.w;
            }
            
        }
        // 加算合成だったら
        else if (InstanceID == CHS_IDENTIFICATION_INSTANCE_ADD)
        {
            
            payloadBuff.light_ = float3(TexColor.w, TexColor.w, TexColor.w);
            
            if (payloadBuff.impactAmount_ < alpha * TexColor.w)
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w;
                payloadBuff.light_ += float3(1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_);
                payloadBuff.impactAmount_ = 0.0f;

            }
            else
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w;
                payloadBuff.light_ += float3(1 * alpha * TexColor.w, 1 * alpha * TexColor.w, 1 * alpha * TexColor.w);
                payloadBuff.impactAmount_ -= alpha * TexColor.w;
            }
        }
        
        // アルファが一定以下だったら。
        if (alpha < 0.5f)
        {
            ++payloadBuff.alphaCounter_;
            if (3 <= payloadBuff.alphaCounter_)
            {
                payloadBuff.isCullingAlpha_ = true;
            }
        }
            
        if (0.0f < payloadBuff.impactAmount_)
        {
                
            // 反射レイを飛ばす。
            ShootRay(CHS_IDENTIFICATION_RAYID_DEF, WorldPos, WorldRayDirection(), payloadBuff, gRtScene);
            
        }
        
    }
    // 当たったオブジェクトのInstanceIDが屈折だったら
    else if (InstanceID == CHS_IDENTIFICATION_INSTANCE_REFRACTION)
    {
        
        if (payloadBuff.impactAmount_ < metalness)
        {
            payloadBuff.color_.xyz += (float3) TexColor * payloadBuff.impactAmount_;
            payloadBuff.impactAmount_ = 0.0f;

        }
        else
        {
            payloadBuff.color_.xyz += (float3) TexColor * metalness;
            payloadBuff.impactAmount_ -= metalness;
        }

        float refractVal = 1.4f;
        float3 rayDir = float3(0, 0, 0);

        float nr = dot(NormalMap, WorldRayDirection());
        if (nr < 0)
        {

            // 空気中->オブジェクト
            float eta = 1.0f / refractVal;
            rayDir = refract(WorldRayDirection(), NormalMap, eta);

        }
        else
        {

            // オブジェクト->空気中
            float eta = refractVal / 1.0f;
            rayDir = refract(WorldRayDirection(), -NormalMap, eta);
      
        }
            
        if (0.0f < payloadBuff.impactAmount_)
        {
                
            ShootRay(CHS_IDENTIFICATION_RAYID_REFRACTION, WorldPos, rayDir, payloadBuff, gRtScene);
            
        }

    }
    // 一定以上離れていると反射を行わない。
    else if (5000.0f < RayTCurrent())
    {
        
        payloadBuff.color_ = TexColor;
        payloadBuff.impactAmount_ = 0.0f;
        
    }
    // 反射の処理
    else
    {
        
        float metal = metalness;
        float rougness = material[0].roughness_;
        payloadBuff.roughnessOffset_ = rougness * 100.0f;
        
        // metalnessマップの取得。
        if (material[0].mapParam_ == MAP_SPECULAR)
        {
            // metalnessマップの色を取得。
            float3 metalnessMapColor = (float3) mapTexture.SampleLevel(smp, Vtx.uv, 0.0f);
        }
        
        if (payloadBuff.impactAmount_ < metal)
        {
            payloadBuff.color_.xyz += (float3) TexColor * payloadBuff.impactAmount_;
            payloadBuff.impactAmount_ = 0.0f;
        }
        else
        {
            payloadBuff.color_.xyz += (float3) TexColor * metal;
            payloadBuff.impactAmount_ -= metal;
            
            if (0.0f < payloadBuff.impactAmount_)
            {
                
                // 反射レイを飛ばす。
                ShootRay(CHS_IDENTIFICATION_RAYID_RECLECTION, WorldPos, reflect(WorldRayDirection(), NormalMap), payloadBuff, gRtScene);
                
            }
        
        }
        
    }
    
    PayloadData = payloadBuff;
    
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
        
        // アルファのオブジェクトだったら
        if (InstanceID() == CHS_IDENTIFICATION_INSTANCE_ALPHA)
        {
            
            // テクスチャの色を取得。
            Vertex meshInfo[3];
            Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer, meshInfo);
            float4 texColor = (float4) texture.SampleLevel(smp, vtx.uv, 0.0f);
        
            payload.impactAmount_ = texColor.a;
            
            return;
            
        }
        
        payload.impactAmount_ = 0.0f;
        
        return;
    }
    // AO影用レイだったら。
    if (payload.rayID_ == CHS_IDENTIFICATION_RAYID_AO_SHADOW)
    {
        
        // AOレイの最大長
        const float AO_LENGTH = 50.0f;
        float rayLength = RayTCurrent();
        
        if (rayLength <= AO_LENGTH)
        {
            payload.impactAmount_ = rayLength / AO_LENGTH;
        }
        else
        {
            payload.impactAmount_ = 0.0f;
        }
        
        
        return;
        
    }

    // 呼び出し回数が制限を超えないようにする。
    ++payload.recursive_;
    if (5 < payload.recursive_ || payload.impactAmount_ <= 0 || payload.isCullingAlpha_)
    {
        
        return;
    }
    
    Vertex meshInfo[3];
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer, meshInfo);
    uint instanceID = InstanceID();
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldNormal = normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3()));
    float3 normalMap = worldNormal;
    
    // MipLevel計算処理
    float2 ddxUV;
    float2 ddyUV;
    if (payload.rayID_ != CHS_IDENTIFICATION_RAYID_DEF)
    {
        
        // レイの発射ベクトルを求めるのに必要な変数たち
        matrix mtxViewInv = gSceneParam.camera.mtxViewInv;
        matrix mtxProjInv = gSceneParam.camera.mtxProjInv;
        float2 dims = float2(DispatchRaysDimensions().xy);
        float aspect = dims.x / dims.y;
        
        // 現在のレイからX+方向の発射ベクトル
        uint2 launchIndex = DispatchRaysIndex().xy + uint2(1, 0);
        float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
        float4 target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
        float3 rayDir = WorldRayDirection();
        float rotationAmountR = 0.0174533f / 30.0f;
        float3 rayDirX = normalize(mul(rayDir, float3x3(1, 0, 0,
                                               0, cos(rotationAmountR), -sin(rotationAmountR),
                                               0, sin(rotationAmountR), cos(rotationAmountR))));
        
        // 現在のレイからY+方向の発射ベクトル
        launchIndex -= uint2(1, -1);
        d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
        target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
        float3 rayDirY = normalize(mul(rayDir, float3x3(cos(rotationAmountR), 0, sin(rotationAmountR),
                                               0, 1, 0,
                                               -sin(rotationAmountR), 0, cos(rotationAmountR))));
        
        // レイの射出地点。
        float3 worldRayOrigin = WorldRayOrigin() + (RayTMin() * rayDir);
        
        // ベクトルXが平面に当たるまでの長さと衝突地点を求める。
        float lengthX = dot(-worldNormal, worldRayOrigin - worldPos) / dot(worldNormal, rayDirX);
        float3 impPosX = rayDirX * lengthX + worldRayOrigin;
        
        // ベクトルYが平面に当たるまでの長さと衝突地点を求める。
        float lengthY = dot(-worldNormal, worldRayOrigin - worldPos) / dot(worldNormal, rayDirY);
        float3 impPosY = rayDirY * lengthY + worldRayOrigin;
        
        // XYの重心座標を求める。
        float3 baryX = CalcVertexBarys(impPosX, meshInfo[0].Position, meshInfo[1].Position, meshInfo[2].Position);
        float3 baryY = CalcVertexBarys(impPosY, meshInfo[0].Position, meshInfo[1].Position, meshInfo[2].Position);
        
        // uvを求めて、その差分を取得する。
        float2 uvX = baryX.x * meshInfo[0].uv + baryX.y * meshInfo[1].uv + baryX.z * meshInfo[2].uv;
        float2 uvY = baryY.x * meshInfo[0].uv + baryY.y * meshInfo[1].uv + baryY.z * meshInfo[2].uv;
        ddxUV = abs(uvX - vtx.uv);
        ddyUV = abs(uvY - vtx.uv);
        
        
    }
    else
    {
        // レイの発射ベクトルを求めるのに必要な変数たち
        matrix mtxViewInv = gSceneParam.camera.mtxViewInv;
        matrix mtxProjInv = gSceneParam.camera.mtxProjInv;
        float2 dims = float2(DispatchRaysDimensions().xy);
        float aspect = dims.x / dims.y;
        
        // 現在のレイからX+方向の発射ベクトル
        uint2 launchIndex = DispatchRaysIndex().xy + uint2(1, 0);
        float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
        float4 target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
        float3 rayDirX = normalize(mul(mtxViewInv, float4(target.xyz, 0)).xyz);
        
        // 現在のレイからY+方向の発射ベクトル
        launchIndex -= uint2(1, -1);
        d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
        target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
        float3 rayDirY = normalize(mul(mtxViewInv, float4(target.xyz, 0)).xyz);
        
        // レイの射出地点。
        float3 worldRayOrigin = WorldRayOrigin() + (RayTMin() * WorldRayDirection());
        
        // ベクトルXが平面に当たるまでの長さと衝突地点を求める。
        float lengthX = dot(-worldNormal, worldRayOrigin - worldPos) / dot(worldNormal, rayDirX);
        float3 impPosX = rayDirX * lengthX + worldRayOrigin;
        
        // ベクトルYが平面に当たるまでの長さと衝突地点を求める。
        float lengthY = dot(-worldNormal, worldRayOrigin - worldPos) / dot(worldNormal, rayDirY);
        float3 impPosY = rayDirY * lengthY + worldRayOrigin;
        
        // XYの重心座標を求める。
        float3 baryX = CalcVertexBarys(impPosX, meshInfo[0].Position, meshInfo[1].Position, meshInfo[2].Position);
        float3 baryY = CalcVertexBarys(impPosY, meshInfo[0].Position, meshInfo[1].Position, meshInfo[2].Position);
        
        // uvを求めて、その差分を取得する。
        float2 uvX = baryX.x * meshInfo[0].uv + baryX.y * meshInfo[1].uv + baryX.z * meshInfo[2].uv;
        float2 uvY = baryX.x * meshInfo[0].uv + baryX.y * meshInfo[1].uv + baryX.z * meshInfo[2].uv;
        ddxUV = abs(uvX - vtx.uv);
        ddyUV = abs(uvY - vtx.uv);
        
    }

    // テクスチャの色を取得。
    float4 texColor = (float4) texture.SampleGrad(smp, vtx.uv, ddxUV * payload.roughnessOffset_, ddyUV * payload.roughnessOffset_);
    float4 defTexColor = texColor;
    
    // 追加のマップがAO用だったら
    if (material[0].mapParam_ == MAP_AO)
    {
        float mapColor = (float4) mapTexture.SampleGrad(smp, vtx.uv, ddxUV * payload.roughnessOffset_, ddyUV * payload.roughnessOffset_);
        texColor *= mapColor;
    }
    // 追加のマップが法線用だったら
    if (material[0].mapParam_ == MAP_NORMAL)
    {
        float3 normalColor = (float4) mapTexture.SampleGrad(smp, vtx.uv, ddxUV * payload.roughnessOffset_, ddyUV * payload.roughnessOffset_);
        
        normalMap = normalColor;
        
        // 接空間変換用
        float3 tan;
        float3 bnorm;
        CalcTangentAndBinormal(meshInfo[0].Position, meshInfo[1].Position, meshInfo[2].Position, meshInfo[0].uv, meshInfo[1].uv, meshInfo[2].uv, tan, bnorm);
        
        // 説空間行列を求める。
        float3x3 mat =
        {
            float3(tan),
            float3(bnorm),
            float3(vtx.Normal)
        };
        
        normalMap = mul(normalMap, mat);
        
    }

    // メッシュのデバッグ
    float2 uv = DispatchRaysIndex().xy / float2(1280.0f, 720.0f);
    uv.x += gSceneParam.debug_.debugMesnInfoX_ / 1280.0f;
    float3 debugColor = (float3) debugMaskTexture.SampleLevel(smp, uv, 1);
    if (gSceneParam.debug_.isDebugMeshInfo_ == 1)
    {

        // 白…UI
        if (debugColor.x == 1 && debugColor.y == 1 && debugColor.z == 1)
        {
            payload.color_ = debugColor;
            payload.light_ = float3(1, 1, 1);
        }
        // 黃…メッシュ
        else if (debugColor.x == 1 && debugColor.y == 1)
        {
            payload.color_ = float3(attrib.barys, 1);
            payload.light_ = float3(1, 1, 1);
            return;
        }
        // 紫…法線
        else if (debugColor.x == 1 && debugColor.z == 1)
        {
            payload.color_ = normalMap;
            payload.light_ = float3(1, 1, 1);
            return;
        }
        // 赤…通常
        else if (debugColor.x == 1)
        {
        }
        // 緑…アルベド
        else if (debugColor.y == 1)
        {
            payload.color_ = texColor;
            payload.light_ = float3(1, 1, 1);
            return;
        }
        // 青…メタルネス
        else if (debugColor.z == 1)
        {
            payload.color_ = float3(material[0].metalness_, material[0].metalness_, material[0].metalness_);
            payload.light_ = float3(1, 1, 1);
            return;
        }
        // 黒…ラフネス
        else
        {
            payload.color_ = float3(material[0].roughness_, material[0].roughness_, material[0].roughness_);
            payload.light_ = float3(1, 1, 1);
            return;
        }

    }
    
    // ライティング前の処理を実行。----- 全反射オブジェクトやテクスチャの色をそのまま使うオブジェクトの色取得処理。
    if (ProcessingBeforeLighting(payload, vtx, attrib, worldPos, worldNormal, normalMap, texColor, instanceID))
    {
        return;
    }
    

    // ライティングの処理
    if (instanceID != CHS_IDENTIFICATION_INSTANCE_ALPHA)
    {
        if (Lighting(payload, worldPos, normalMap, vtx, texColor))
        {
            return;
        }
    }
    
    // ライティング後の処理を実行。 ----- ライティングの結果を合成する処理や反射や屈折等の再びレイを飛ばす必要があるオブジェクトの処理。
    ProccessingAfterLighting(payload, vtx, worldPos, normalMap, defTexColor, texColor, instanceID);
    
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
        
    Vertex meshInfo[3];
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer, meshInfo);
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
    
}