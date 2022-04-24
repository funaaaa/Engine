
// 頂点データ
static const int MAX_BONE = 16;

struct VertexPosNormalUvSkin
{
    float3 pos : POSITION; // 座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // UV
    uint4 boneIndex : BONEINDICES; // ボーンの番号
    float4 boneWeight : BONEWEIGHTS; // ボーンの重み
};

struct SkinData
{
    matrix bones[MAX_BONE];
};

// 入力データ構造体
struct InputData
{
    VertexPosNormalUvSkin vertex;
    SkinData skinData;
};

//出力データ構造体
struct OutputData
{
    float3 pos;
    float3 normal;
    float2 uv;
};

//入力データにアクセスするための変数
StructuredBuffer<InputData> input : register(t0);

//出力データにアクセスするための変数
RWStructuredBuffer<OutputData> output : register(u0);

//エントリーポイント
[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

    // ゼロクリア
    OutputData outputData = (OutputData) 0;

    uint iBone; // 計算するボーンの番号
    float weight; // ボーンウェイト(重み)
    matrix m; // スキニング行列

    // ボーン0
    iBone = input[DTid.x].vertex.boneIndex.x;
    weight = input[DTid.x].vertex.boneWeight.x;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    // ボーン1
    iBone = input[DTid.x].vertex.boneIndex.y;
    weight = input[DTid.x].vertex.boneWeight.y;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    // ボーン2
    iBone = input[DTid.x].vertex.boneIndex.z;
    weight = input[DTid.x].vertex.boneWeight.z;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    // ボーン3
    iBone = input[DTid.x].vertex.boneIndex.w;
    weight = input[DTid.x].vertex.boneWeight.w;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    output[DTid.x] = outputData;

}

