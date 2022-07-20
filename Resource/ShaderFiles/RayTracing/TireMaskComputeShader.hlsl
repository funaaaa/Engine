// 入力UAV
RWTexture2D<float4> InputImg : register(u0);

// 出力先UAV
RWTexture2D<float4> OutputImg : register(u1);

// 書き換える場所指定定数バッファ
cbuffer WriteUV : register(b0)
{
    float2 uv_[4];
};

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 index = uint2(DTid.x, DTid.y);
    float2 uv = float2(index.x / 2048.0f, index.y / 2048.0f);
    
    if (abs(uv.x - uv_[0].x) < 0.01f && abs(uv.y - uv_[0].y) < 0.01f)
    {
        OutputImg[index] = float4(1, 1, 1, 1);
    }
    
}