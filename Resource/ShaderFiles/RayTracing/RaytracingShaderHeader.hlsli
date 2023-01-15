
// 円周率
static const float PI = 3.141592653589f;

// CHSでの識別用変数
static const int CHS_IDENTIFICATION_INSTANCE_DEF = 0; // InstanceID 通常のオブジェクト
static const int CHS_IDENTIFICATION_INSTNACE_AS = 1; // InstanceID 大気散乱用のオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_TEXCOLOR = 2; // InstanceID テクスチャの色をそのまま返すオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_DEF_EMISSIVE = 3; // InstanceID 通常反射とエミッシブ
static const int CHS_IDENTIFICATION_INSTANCE_TEXCOLOR_REFLECTION_EMISSIVE = 4; // InstanceID 通常反射とテクスチャの色そのままとエミッシブ
static const int CHS_IDENTIFICATION_INSTANCE_LIGHT = 5; // instanceID ライト用オブジェクト テクスチャの色をそのまま返す。MissShaderで当たり判定を棄却する為にも使用する。
static const int CHS_IDENTIFICATION_INSTANCE_REFRACTION = 6; // instanceID 屈折の処理
static const int CHS_IDENTIFICATION_INSTANCE_INVISIBILITY = 7; // instanceID ライティングも描画も行わないオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_DEF_GI = 8; // instanceID 通常の処理 + GIも行う。
static const int CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK = 9; // instanceID 通常の処理 + GI + タイヤ痕
static const int CHS_IDENTIFICATION_INSTANCE_ALPHA = 10; // instanceID 半透明
static const int CHS_IDENTIFICATION_INSTANCE_ADD = 11; // instanceID 加算合成
static const int CHS_IDENTIFICATION_INSTANCE_DEF_TIREMASK = 12; // instanceID 通常の処理 + タイヤ痕
static const int CHS_IDENTIFICATION_INSTANCE_DEF_TAILLAMP = 13; // instanceID 通常の処理 + エミッシブ + マテリアルアルファ
static const int CHS_IDENTIFICATION_INSTANCE_DEF_AO = 14; // instanceID 通常の処理 + AO
static const int CHS_IDENTIFICATION_INSTANCE_DEF_TIREMASK_AO = 15; // instanceID 通常の処理 + タイヤ痕 + AO

static const int CHS_IDENTIFICATION_RAYID_DEF = 100; // デフォルトのレイ
static const int CHS_IDENTIFICATION_RAYID_GI = 101; // GI用のレイ
static const int CHS_IDENTIFICATION_RAYID_RECLECTION = 102; // 反射用のレイ
static const int CHS_IDENTIFICATION_RAYID_COMPLETE_RECLECTION = 103; // 完全反射のレイ
static const int CHS_IDENTIFICATION_RAYID_REFRACTION = 104; // 屈折のレイ
static const int CHS_IDENTIFICATION_RAYID_SHADOW = 105; // 影用のレイ
static const int CHS_IDENTIFICATION_RAYID_AO_SHADOW = 106; // AO影用のレイ

// カメラ用の定数バッファ
struct CameraConstBufferData
{
    matrix mtxView; // ビュー行列
    matrix mtxProj; // プロジェクション行列
    matrix mtxViewInv; // 逆ビュー行列
    matrix mtxProjInv; // 逆プロジェクション行列
};

// ディレクショナルライト用定数バッファ
struct DirLightData
{
    float3 lightDir;
    int isActive;
    float3 lightColor;
    int seed;
};
// ポイントライト用定数バッファ
struct PointLightData
{
    float3 lightPos;
    float lightSize;
    float3 lightColor;
    float lightPower;
    int isActive;
    int isShadow;
    float2 pad;
};
// ライト用定数バッファ
struct LightConstBufferData
{
    DirLightData dirLight;
};
// アルファ値転送用の定数バッファ
struct AlphaData
{
    int instanceIndex_;
    int alpha_;
};
#define ALPHA_DATA_COUNT 60
struct AlphaConstBufferData
{
    AlphaData alphaData_[ALPHA_DATA_COUNT];
};

// デバッグ用情報
struct DebugData
{
    int isDebugMeshInfo_;
    int debugMesnInfoX_;
    float2 pad_;
};

// 定数バッファ
struct ConstBufferData
{
    CameraConstBufferData camera;
    LightConstBufferData light;
    AlphaConstBufferData alphaData_;
    DebugData debug_;
};

static int MAP_NONE = 0;
static int MAP_NORMAL = 1;
static int MAP_SPECULAR = 2;
static int MAP_AO = 3;

// マテリアル情報
struct Material
{
    float3 baseColor_;
    float metalness_;
    float specular_;
    float roughness_;
    int mapParam_; // マテリアルの追加情報。追加のテクスチャをどう使用するかを判断する。
    float alpha_;
};

// 頂点情報
struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 uv;
    float2 subUV;
    float2 pad_;
};

// ペイロード
struct Payload
{
    uint recursive_; // 反復回数
    uint rayID_; // レイのID
    uint alphaCounter_; // 薄いアルファのオブジェクトに当たった数
    uint isCullingAlpha_; // 薄いアルファのオブジェクトに一定以上当たったら次からアルファを無効化するフラグ。
    float3 color_; // 色情報
    float impactAmount_; // 合計影響度
    float3 light_; // ライティングの色情報
    float ao_; // AOの色
    float3 denoiseMask_; // デノイズのマスクの色情報
    float roughnessOffset_;
    float3 emissive_; // エミッシブ出力用
    float pad_;
};

struct MyAttribute
{
    float2 barys;
};

// barysを計算
inline float3 CalcBarycentrics(float2 Barys)
{
    return float3(1.0 - Barys.x - Barys.y, Barys.x, Barys.y);
}

// 影のレイを照射
bool ShootShadowRay(float3 Origin, float3 Direction, float TMax, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = TMax;

    Payload payloadData;
    payloadData.impactAmount_ = 1.0f;
    payloadData.rayID_ = CHS_IDENTIFICATION_RAYID_SHADOW;
    payloadData.recursive_ = 0;
    payloadData.ao_ = 0;
    payloadData.color_ = float3(0, 0, 0);
    payloadData.denoiseMask_ = float3(0, 0, 0);
    payloadData.light_ = float3(0, 0, 0);
    payloadData.isCullingAlpha_ = false;
    payloadData.alphaCounter_ = 0;
    payloadData.roughnessOffset_ = 1.0f;
    payloadData.pad_ = 0.0f;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderをスキップ
    
    // ???C?g????O?B
    uint rayMask = ~(0x08);

    TraceRay(
    GRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // Missシェーダーのインデックスを指定する。
    rayDesc,
    payloadData);

    return payloadData.impactAmount_;
}
float ShootShadowRayNoAH(float3 Origin, float3 Direction, float TMax, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = TMax;

    Payload payload;
    payload.impactAmount_ = 1.0f;
    payload.rayID_ = CHS_IDENTIFICATION_RAYID_SHADOW;
    payload.recursive_ = 0;
    payload.ao_ = 0;
    payload.color_ = float3(0, 0, 0);
    payload.denoiseMask_ = float3(0, 0, 0);
    payload.light_ = float3(0, 0, 0);
    payload.isCullingAlpha_ = false;
    payload.alphaCounter_ = 0;
    payload.roughnessOffset_ = 1.0f;
    payload.pad_ = 0.0f;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderをスキップ
    
    // ???C?g????O?B
    uint rayMask = ~(0x08);

    TraceRay(
    GRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // Missシェーダーのインデックスを指定する。
    rayDesc,
    payload);

    return payload.impactAmount_;
}

// AO用の影レイを射出
float ShootAOShadowRay(float3 Origin, float3 Direction, float TMax, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = TMax;

    Payload payload;
    payload.impactAmount_ = 1.0f;
    payload.rayID_ = CHS_IDENTIFICATION_RAYID_AO_SHADOW;
    payload.recursive_ = 0;
    payload.ao_ = 0;
    payload.color_ = float3(0, 0, 0);
    payload.denoiseMask_ = float3(0, 0, 0);
    payload.light_ = float3(0, 0, 0);
    payload.isCullingAlpha_ = false;
    payload.alphaCounter_ = 0;
    payload.roughnessOffset_ = 1.0f;
    payload.pad_ = 0.0f;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderをスキップ
    
    // ???C?g????O?B
    uint rayMask = ~(0x08);

    TraceRay(
    GRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // Missシェーダーのインデックスを指定する。
    rayDesc,
    payload);

    return payload.impactAmount_;
}

// 値を設定してレイを発射。
void ShootRay(uint RayID, float3 Origin, float3 Direction, inout Payload PayloadData, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = 300000.0f;

    // ???C??ID???p???C????B
    int rayID = PayloadData.rayID_;
    PayloadData.rayID_ = RayID;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flags |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShaderを実行
    
    // ???C?g????O?B
    uint rayMask = 0xFF;

    TraceRay(
    GRtScene,
    flags,
    rayMask,
    0,
    1,
    0, // Missシェーダーのインデックスを指定する。
    rayDesc,
    PayloadData);
    
    PayloadData.rayID_ = rayID;
    
}

// 当たった位置の情報を取得する関数
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer, inout Vertex meshInfo[3])
{
    Vertex v = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attrib.barys);
    uint vertexId = PrimitiveIndex() * 3; // Triangle List のため.

    float weights[3] =
    {
        barycentrics.x, barycentrics.y, barycentrics.z
    };

    for (int index = 0; index < 3; ++index)
    {
        uint vtxIndex = indexBuffer[vertexId + index];
        float w = weights[index];
        v.Position += vertexBuffer[vtxIndex].Position * w;
        v.Normal += vertexBuffer[vtxIndex].Normal * w;
        v.uv += vertexBuffer[vtxIndex].uv * w;
        v.subUV += vertexBuffer[vtxIndex].subUV * w;
        
        // メッシュの情報を保存。
        meshInfo[index].Position = mul(float4(vertexBuffer[vtxIndex].Position, 1), ObjectToWorld4x3());
        meshInfo[index].Normal = normalize(mul(vertexBuffer[vtxIndex].Normal, (float3x3) ObjectToWorld4x3()));
        meshInfo[index].uv = vertexBuffer[vtxIndex].uv;
    }

    return v;
}

// 指定の頂点の衝突したメッシュ上での重心座標を求める。
float3 CalcVertexBarys(float3 HitVertex, float3 VertexA, float3 VertexB, float3 VertexC)
{
    
    float3 e0 = VertexB - VertexA;
    float3 e1 = VertexC - VertexA;
    float3 e2 = HitVertex - VertexA;
    float d00 = dot(e0, e0);
    float d01 = dot(e0, e1);
    float d11 = dot(e1, e1);
    float d20 = dot(e2, e0);
    float d21 = dot(e2, e1);
    float denom = 1.0 / (d00 * d11 - d01 * d01);
    float v = (d11 * d20 - d01 * d21) * denom;
    float w = (d00 * d21 - d01 * d20) * denom;
    float u = 1.0 - v - w;
    return float3(u, v, w);
    
}

// 資料から持ってきた関数。
float Scale(float FCos)
{
    float x = 1.0 - FCos;
    return 0.25f * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}
// 資料から持ってきた関数。 天球との交点を求める。
float3 IntersectionPos(float3 Dir, float3 A, float Radius)
{
    float b = dot(A, Dir);
    float c = dot(A, A) - Radius * Radius;
    float d = max(b * b - c, 0.0);

    return A + Dir * (-b + sqrt(d));
}

// 3頂点とUV値から指定座標でのU軸（Tangent）及びV軸（Binormal）を算出
void CalcTangentAndBinormal(float3 p0, float3 p1, float3 p2, float2 uv0, float2 uv1, float2 uv2, inout float3 outTangent, inout float3 outBinormal)
{
    // 5次元→3次元頂点に
    float3 CP0[3] =
    {
        float3(p0.x, uv0.x, uv0.y),
        float3(p0.y, uv0.x, uv0.y),
        float3(p0.z, uv0.x, uv0.y),
    };
    float3 CP1[3] =
    {
        float3(p1.x, uv1.x, uv1.y),
        float3(p1.y, uv1.x, uv1.y),
        float3(p1.z, uv1.x, uv1.y),
    };
    float3 CP2[3] =
    {
        float3(p2.x, uv2.x, uv2.y),
        float3(p2.y, uv2.x, uv2.y),
        float3(p2.z, uv2.x, uv2.y),
    };

    // 平面パラメータからUV軸座標算出
    float U[3], V[3];
    for (int i = 0; i < 3; ++i)
    {
        float3 V1 = CP1[i] - CP0[i];
        float3 V2 = CP2[i] - CP1[i];
        float3 ABC;
        ABC = cross(V1, V2);

        if (ABC.x == 0.0f)
        {
            return;
        }
        U[i] = -ABC.y / ABC.x;
        V[i] = -ABC.z / ABC.x;
    }

    outTangent.x = U[0];
    outTangent.y = U[1];
    outTangent.z = U[2];

    outBinormal.x = V[0];
    outBinormal.y = V[1];
    outBinormal.z = V[2];
    
    // 正規化
    outTangent = normalize(outTangent);
    outBinormal = normalize(outBinormal);
    
}