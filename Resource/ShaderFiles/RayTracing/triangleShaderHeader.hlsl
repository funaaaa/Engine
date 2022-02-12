
// TLASのバッファを指定。
RaytracingAccelerationStructure gRtScene : register(t0);
// レイトレーシング結果バッファを指定。
RWTexture2D<float4> gOutput : register(u0);

// ペイロード 色情報を取得するための構造体
struct Payload
{
    float3 color;
};
// アトリビュート 当たった位置を取得するための構造体
struct MyAttribute
{
    float2 barys;
};

// RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;

    // レイの設定
    RayDesc rayDesc;
    rayDesc.Origin = float3(d.x, -d.y, 1);
    rayDesc.Direction = float3(0, 0, -1);
    rayDesc.TMin = 0;
    rayDesc.TMax = 100000;

    // ペイロードの設定
    Payload payload;
    payload.color = float3(0, 0, 0);

    // TransRayに必要な設定を作成
    uint rayMask = 0xFF;

    // レイを発射
    TraceRay(
    gRtScene, // TLAS
    0, // 衝突判定制御をするフラグ
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

}

// missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload payload)
{

    // 単色を返すようにする。
    payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);

}

// closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]
void mainCHS(inout Payload payload, MyAttribute attrib)
{

    float3 col = 0;

    // レイがヒットした場所の色を取得する。
    col.xy = attrib.barys;
    col.z = 1.0 - col.x - col.y;

    // 色を返す。
    payload.color = col;

}
