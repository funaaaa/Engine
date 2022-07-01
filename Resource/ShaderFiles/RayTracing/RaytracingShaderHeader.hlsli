﻿
// 円周率
static const float PI = 3.141592653589f;

// CHSでの識別用変数
static const int CHS_IDENTIFICATION_INSTANCE_DEF = 0; // InstanceID 通常のオブジェクト
static const int CHS_IDENTIFICATION_INSTNACE_AS = 1; // InstanceID 大気散乱用のオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_TEXCOLOR = 2; // InstanceID テクスチャの色をそのまま返すオブジェクト
static const int CHS_IDENTIFICATION_ISNTANCE_REFLECTION = 3; // InstanceID 反射のオブジェクト
static const int CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION = 4; // InstanceID 完全反射のオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_LIGHT = 5; // instanceID ライト用オブジェクト テクスチャの色をそのまま返す。MissShaderで当たり判定を棄却する為にも使用する。
static const int CHS_IDENTIFICATION_INSTANCE_REFRACTION = 6; // instanceID 屈折の処理
static const int CHS_IDENTIFICATION_INSTANCE_INVISIBILITY = 7; // instanceID ライティングも描画も行わないオブジェクト
static const int CHS_IDENTIFICATION_INSTANCE_DEF_GI = 8; // instanceID 通常の処理 + GIも行う。

static const int CHS_IDENTIFICATION_RAYID_DEF = 100; // デフォルトのレイ
static const int CHS_IDENTIFICATION_RAYID_GI = 101; // GI用のレイ
static const int CHS_IDENTIFICATION_RAYID_RECLECTION = 102; // 反射用のレイ
static const int CHS_IDENTIFICATION_RAYID_COMPLETE_RECLECTION = 103; // 完全反射のレイ
static const int CHS_IDENTIFICATION_RAYID_REFRACTION = 104; // 屈折のレイ

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
    float pad;
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

// 大気散乱用定数バッファ
struct ASConstBufferData
{
    float kr; // レイリー散乱定数
    float km; // ミー散乱定数
    float samples; // サンプル数
    float outerRadius; // 大気の外周
    float innerRadius; // 地上の外周
    float eSun; // 太陽光線の強さ
    float g; // 散乱定数
    float aveHeight; // 平均大気密度を取得する高さ
};
// デバッグ用のパラメーター定数バッファ
struct DebugConstBufferData
{
    int seed;
    int isNoiseScene; // ノイズのみのシーンを描画するかのフラグ
    int isLightHitScene; // ライトに当たった面のみを描画するフラグ
    int isNormalScene; // 法線情報を描画するフラグ
    int isMeshScene; // メッシュ情報を描画するフラグ
    int isNoAO;
    int isNoGI; // GIの処理を行わないフラグ
    int isGIOnlyScene;
};

// 定数バッファ
struct ConstBufferData
{
    CameraConstBufferData camera;
    LightConstBufferData light;
    ASConstBufferData as;
    DebugConstBufferData debug;
};

// マテリアル情報
struct Material
{
    float3 ambient;
    float pad1;
    float3 diffuse;
    float pad2;
    float3 specular;
    float alpha;
};

// 頂点情報
struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 uv;
};

// ペイロード
struct DenoisePayload
{
    float3 color;
    float3 aoLuminance;
    float3 lightLuminance;
    float3 giColor;
    float3 denoiseMask;
    uint recursive;
    uint rayID;
};
// 影取得用ペイロード
struct ShadowPayload
{
    bool isShadow;
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

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    //flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShader�X�L�b�v���Ȃ�
    
    // ���C�g�͏��O�B
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

    return payload.isShadow;
}
bool ShootShadowRayNoAH(float3 Origin, float3 Direction, float TMax, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = TMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    //flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShader�X�L�b�v
    
    // ���C�g�͏��O�B
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

    return payload.isShadow;
}

// AO用の影レイを射出
bool ShootAOShadowRay(float3 Origin, float3 Direction, float TMax, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = TMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�X�L�b�v
    
    // ���C�g�͏��O�B
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

    return payload.isShadow;
}

// 値を設定してレイを発射。
void ShootRay(uint RayID, float3 Origin, float3 Direction, inout DenoisePayload Payload, RaytracingAccelerationStructure GRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = Origin;
    rayDesc.Direction = Direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = 300000.0f;

    // ���C��ID�𔽎˗p���C�ɐݒ�B
    Payload.rayID = RayID;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�X�L�b�v
    
    // ���C�g�͏��O�B
    uint rayMask = ~(0x08);

    TraceRay(
    GRtScene,
    flags,
    rayMask,
    0,
    1,
    0, // Missシェーダーのインデックスを指定する。
    rayDesc,
    Payload);
    
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