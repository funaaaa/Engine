
// 頂点情報
struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 uv;
};

// 環境情報
struct SceneCB
{
    matrix mtxView; // ビュー行列.
    matrix mtxProj; // プロジェクション行列.
    matrix mtxViewInv; // ビュー逆行列.
    matrix mtxProjInv; // プロジェクション逆行列.
    float4 lightDirection; // 平行光源の向き.
    float4 lightColor; // 平行光源色.
    float4 ambientColor; // 環境光.
    int seed;
    int counter;
};


// ペイロード 色情報を取得するための構造体
struct Payload
{
    float3 color;
    uint recursive;
};
// ペイロード 影情報を取得するための構造体
struct ShadowPayload
{
    bool isShadow;
};
// アトリビュート 当たった位置を取得するための構造体
struct MyAttribute
{
    float2 barys;
};

// 当たった位置を計算する関数
inline float3 CalcBarycentrics(float2 barys)
{
    return float3(1.0 - barys.x - barys.y, barys.x, barys.y);
}

// 当たった位置を特定
inline float2 CalcHitAttribute2(float2 vertexAttribute[3], float2 barycentrics)
{
    float2 ret;
    ret = vertexAttribute[0];
    ret += barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]);
    ret += barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
    return ret;
}
inline float3 CalcHitAttribute3(float3 vertexAttribute[3], float2 barycentrics)
{
    float3 ret;
    ret = vertexAttribute[0];
    ret += barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]);
    ret += barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
    return ret;
}


// 制限以上トレースしないようにする
inline bool checkRecursiveLimit(inout Payload payload)
{
    payload.recursive++;
    if (payload.recursive >= 29)
    {
        payload.color = float3(0, 0, 0);
        return true;
    }
    return false;
}

// 乱数生成
uint randomU(float2 uv)
{
    float r = dot(uv, float2(127.1, 311.7));
    return uint(12345 * frac(sin(r) * 43758.5453123));
}
float nextRand(inout uint s)
{
    s = (1664525u * s + 1013904223u);
    return float(s & 0x00FFFFFF) / float(0x01000000);
}

float3x3 angleAxis3x3(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return float3x3(
        t * x * x + c, t * x * y - s * z, t * x * z + s * y,
        t * x * y + s * z, t * y * y + c, t * y * z - s * x,
        t * x * z - s * y, t * y * z + s * x, t * z * z + c
        );
}

// ソフトシャドウ用に臨時で持ってきた処理。
float3 getConeSample(inout uint randSeed, float3 direction, float coneAngle)
{
    float cosAngle = cos(coneAngle);
    const float PI = 3.1415926535;
    // Generate points on the spherical cap around the north pole [1].
    // [1] See https://math.stackexchange.com/a/205589/81266
    float z = nextRand(randSeed) * (1.0f - cosAngle) + cosAngle;
    float phi = nextRand(randSeed) * 2.0f * PI;
    float x = sqrt(1.0f - z * z) * cos(phi);
    float y = sqrt(1.0f - z * z) * sin(phi);
    float3 north = float3(0.f, 0.f, 1.f);
    // Find the rotation axis `u` and rotation angle `rot` [1]
    float3 axis = normalize(cross(north, normalize(direction)));
    float angle = acos(dot(normalize(direction), north));
    // Convert rotation axis and angle to 3x3 rotation matrix [2]
    float3x3 R = angleAxis3x3(angle, axis);
    return mul(R, float3(x, y, z));
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

float3 GetPerpendicularVector(float3 u)
{
    float3 a = abs(u);
    uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
    uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
    uint zm = 1 ^ (xm | ym);
    return cross(u, float3(xm, ym, zm));
}

// 法線基準で半球状のランダムなベクトルを生成。
float3 GetCosHemisphereSample(uint randSeed, float3 hitNorm)
{
    // 2つのランダムな数値を取得
    float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));
    // 法線に垂直なベクトルを取る（最後に利用する）
    float3 bitangent = GetPerpendicularVector(hitNorm);
    float3 tangent = cross(bitangent, hitNorm);
    // ディスク上に一様にサンプリング
    float r = sqrt(randVal.x);
    float phi = 2.0f * 3.14f * randVal.y;
    // 半球に射影する
    float x = r * cos(phi);
    float z = r * sin(phi);
    float y = sqrt(1.0 - randVal.x); // 1- r2
    // 法線ベクトルの座標系に射影
    return x * tangent + y * hitNorm.xyz + z * bitangent;
}

float3 GetUniformHemisphereSample(uint randSeed, float3 hitNorm)
{
    // 2つのランダムな数値を取得
    float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));
    // 法線に垂直なベクトルを取る（最後に利用する）
    float3 bitangent = GetPerpendicularVector(hitNorm);
    float3 tangent = cross(bitangent, hitNorm);
    // θ、Φを決める
    float cosTheta = randVal.x;
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    float phi = 2.0f * 3.14f * randVal.y;
    // 半球に射影する
    float x = sinTheta * cos(phi);
    float z = sinTheta * sin(phi);
    float y = cosTheta;
    // 法線ベクトルの座標系に射影
    return x * tangent + y * hitNorm.xyz + z * bitangent;
}