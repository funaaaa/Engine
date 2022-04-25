
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
    float shadowRate;
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