
// 円周率
static const float PI = 3.141592653589f;

// CHSでの識別用変数
static const int CHS_IDENTIFICATION_INSTANCE_DEF = 0; // InstanceID 通常のオブジェクト
static const int CHS_IDENTIFICATION_INSTNACE_AS = 1; // InstanceID 大気散乱用のオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_TEXCOLOR = 2; // InstanceID テクスチャの色をそのまま返すオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_LIGHT = 5; // instanceID ライト用オブジェクト テクスチャの色をそのまま返す。MissShaderで当たり判定を棄却する為にも使用する。
static const int CHS_IDENTIFICATION_INSTANCE_REFRACTION = 6; // instanceID 屈折の処理
static const int CHS_IDENTIFICATION_INSTANCE_INVISIBILITY = 7; // instanceID ライティングも描画も行わないオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_DEF_GI = 8; // instanceID 通常の処理 + GIも行う。
static const int CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK = 9; // instanceID 通常の処理 + GI + タイヤ痕
static const int CHS_IDENTIFICATION_INSTANCE_ALPHA = 10; // instanceID 半透明
static const int CHS_IDENTIFICATION_INSTANCE_ADD = 11; // instanceID 加算合成

static const int CHS_IDENTIFICATION_RAYID_DEF = 100; // デフォルトのレイ
static const int CHS_IDENTIFICATION_RAYID_GI = 101; // GI用のレイ
static const int CHS_IDENTIFICATION_RAYID_RECLECTION = 102; // 反射用のレイ
static const int CHS_IDENTIFICATION_RAYID_COMPLETE_RECLECTION = 103; // 完全反射のレイ
static const int CHS_IDENTIFICATION_RAYID_REFRACTION = 104; // 屈折のレイ
static const int CHS_IDENTIFICATION_RAYID_SHADOW = 105; // 影用のレイ

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
static const int POINT_LIGHT_COUNT = 30;
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
    PointLightData pointLight[POINT_LIGHT_COUNT];
};
// アルファ値転送用の定数バッファ
struct AlphaData
{
    int instanceIndex_;
    float alpha_;
};
#define ALPHA_DATA_COUNT 120
struct AlphaConstBufferData
{
    AlphaData alphaData_[ALPHA_DATA_COUNT];
};

// 定数バッファ
struct ConstBufferData
{
    CameraConstBufferData camera;
    LightConstBufferData light;
    AlphaConstBufferData alphaData_;
};

// マテリアル情報
struct Material
{
    float3 baseColor_;
    float metalness_;
    float specular_;
    float roughness_;
    float2 pad_;
};

// 頂点情報
struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 uv;
    float2 subUV;
};

// ペイロード
struct Payload
{
    uint recursive_; // 反復回数
    uint rayID_; // レイのID
    float impactAmount_; // 合計影響度
    float ao_; // AOの色
    float3 color_; // 色情報
    float3 gi_; // GI情報
    float3 light_; // ライティングの色情報
    float3 denoiseMask_; // デノイズのマスクの色情報
    uint alphaCounter_; // 薄いアルファのオブジェクトに当たった数
    uint isCullingAlpha_; // 薄いアルファのオブジェクトに一定以上当たったら次からアルファを無効化するフラグ。
    float2 pad_;
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

// 乱数の種を更新
float NextRand(inout uint S)
{
    S = (1664525u * S + 1013904223u);
    return float(S & 0x00FFFFFF) / float(0x01000000);
}

float3x3 AngleAxis3x3(float Angle, float3 Axis)
{
    float c, s;
    sincos(Angle, s, c);

    float t = 1 - c;
    float x = Axis.x;
    float y = Axis.y;
    float z = Axis.z;

    return float3x3(
        t * x * x + c, t * x * y - s * z, t * x * z + s * y,
        t * x * y + s * z, t * y * y + c, t * y * z - s * x,
        t * x * z - s * y, t * y * z + s * x, t * z * z + c
        );
}

// 円錐状にベクトルをサンプリング
float3 GetConeSample(inout uint randSeed, float3 direction, float coneAngle)
{
    float cosAngle = cos(coneAngle);
    const float PI = 3.1415926535;
    
    float z = NextRand(randSeed) * (1.0f - cosAngle) + cosAngle;
    float phi = NextRand(randSeed) * 2.0f * PI;

    float x = sqrt(1.0f - z * z) * cos(phi);
    float y = sqrt(1.0f - z * z) * sin(phi);
    float3 north = float3(0.f, 0.f, 1.f);
    
    float3 axis = normalize(cross(north, normalize(direction)));
    float angle = acos(dot(normalize(direction), north));
    
    float3x3 R = AngleAxis3x3(angle, axis);

    return mul(R, float3(x, y, z));
}

float3 GetPerpendicularVector(float3 U)
{
    float3 a = abs(U);
    uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
    uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
    uint zm = 1 ^ (xm | ym);
    return cross(U, float3(xm, ym, zm));
}

float3 GetUniformHemisphereSample(inout uint RandSeed, float3 HitNorm)
{
    float2 randVal = float2(NextRand(RandSeed), NextRand(RandSeed));
    float3 bitangent = GetPerpendicularVector(HitNorm);
    float3 tangent = cross(bitangent, HitNorm);
    float cosTheta = randVal.x;
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    float phi = 2.0f * 3.14f * randVal.y;
    float x = sinTheta * cos(phi);
    float z = sinTheta * sin(phi);
    float y = cosTheta;
    return x * tangent + y * HitNorm.xyz + z * bitangent;
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
    payloadData.gi_ = float3(0, 0, 0);
    payloadData.light_ = float3(0, 0, 0);
    payloadData.isCullingAlpha_ = false;
    payloadData.alphaCounter_ = 0;

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
bool ShootShadowRayNoAH(float3 Origin, float3 Direction, float TMax, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = TMax;

    Payload payload;
    payload.impactAmount_ = 1;
    payload.rayID_ = CHS_IDENTIFICATION_RAYID_SHADOW;
    payload.recursive_ = 0;
    payload.ao_ = 0;
    payload.color_ = float3(0, 0, 0);
    payload.denoiseMask_ = float3(0, 0, 0);
    payload.gi_ = float3(0, 0, 0);
    payload.light_ = float3(0, 0, 0);
    payload.isCullingAlpha_ = false;
    payload.alphaCounter_ = 0;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    //flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderをスキップ
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
bool ShootAOShadowRay(float3 Origin, float3 Direction, float TMax, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = TMax;

    Payload payload;
    payload.impactAmount_ = 0;
    payload.rayID_ = CHS_IDENTIFICATION_RAYID_SHADOW;
    payload.recursive_ = 0;
    payload.ao_ = 0;
    payload.color_ = float3(0, 0, 0);
    payload.denoiseMask_ = float3(0, 0, 0);
    payload.gi_ = float3(0, 0, 0);
    payload.light_ = float3(0, 0, 0);
    payload.isCullingAlpha_ = false;
    payload.alphaCounter_ = 0;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    //flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShaderをスキップ
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

uint InitRand(uint Val0, uint Val1, uint Backoff = 16)
{
    uint v0 = Val0, v1 = Val1, s0 = 0;

	[unroll]
    for (uint n = 0; n < Backoff; n++)
    {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }
    return v0;
}



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

    for (int index = 0; index < 3; ++index)
    {
        uint index = indexBuffer[vertexId + index];
        float w = weights[index];
        v.Position += vertexBuffer[index].Position * w;
        v.Normal += vertexBuffer[index].Normal * w;
        v.uv += vertexBuffer[index].uv * w;
        v.subUV += vertexBuffer[index].subUV * w;
    }
    
    v.uv.x -= (int) v.uv.x;
    v.uv.y -= (int) v.uv.y;
    
    v.subUV.x -= (int) v.subUV.x;
    v.subUV.y -= (int) v.subUV.y;

    return v;
}

// 指定の頂点の衝突したメッシュ上での重心座標を求める。
float3 CalcVertexBarys(float3 VertexPos, StructuredBuffer<Vertex> VertexBuffer, StructuredBuffer<uint> IndexBuffer)
{
    
    // 衝突したBLASのIndexを取得。
    uint vertexId = PrimitiveIndex() * 3; // Triangle List のため.
    
    // 衝突した三角形を取得。
    float3 hitVertex[3];
    for (int index = 0; index < 3; ++index)
    {
        uint index = IndexBuffer[vertexId + index];
        hitVertex[index] = VertexBuffer[index].Position;

    }
    
    // 取得した三角形の面積を求める。
    float area = length(cross(hitVertex[2] - hitVertex[0], hitVertex[1] - hitVertex[0])) / 2.0f;

	// 重心座標を求める。
    float3 bary;
    bary.x = length(cross(hitVertex[0] - VertexPos, hitVertex[1] - VertexPos)) / 2.0f / area;
    bary.y = length(cross(hitVertex[1] - VertexPos, hitVertex[2] - VertexPos)) / 2.0f / area;
    bary.z = length(cross(hitVertex[2] - VertexPos, hitVertex[0] - VertexPos)) / 2.0f / area;

    return bary;
    
}

// 重心座標を指定して衝突したメッシュでのUVを求める。
float2 CalcUVByBary(float3 Bary, StructuredBuffer<Vertex> VertexBuffer, StructuredBuffer<uint> IndexBuffer)
{
    
    // 衝突したBLASのIndexを取得。
    uint vertexId = PrimitiveIndex() * 3; // Triangle List のため.
    
    // 指定された重心座標を配列に直す。
    float weights[3] =
    {
        Bary.x, Bary.y, Bary.z
    };
    
    // UV
    float2 uv = float2(0, 0);
    for (int index = 0; index < 3; ++index)
    {
        uint index = IndexBuffer[vertexId + index];
        float w = weights[index];
        uv += VertexBuffer[index].uv * w;
    }
    
    return uv;
    
}

void GetHitMeshInfo(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer, inout Vertex meshInfo[3])
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
        
        meshInfo[i].Position = vertexBuffer[index].Position;
        meshInfo[i].Normal = vertexBuffer[index].Normal;
        meshInfo[i].uv = vertexBuffer[index].uv;
       
    }
    
}




// 資料から持ってきた関数。
float Scale(float FCos)
{
    float x = 1.0 - FCos;
    return 0.25f * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

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