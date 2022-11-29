
// 入力情報
RWTexture2D<float4> InputImg : register(u0);

// 出力先UAV  
RWTexture2D<float4> OutputImg : register(u1);

// 重みテーブル
cbuffer GaussianWeight : register(b0)
{
    float4 weights[2];
};

float4 GetPixelColor(int x, int y, int2 texSize)
{
    x = clamp(0, texSize.x, x);
    y = clamp(0, texSize.y, y);

    return InputImg[uint2(x, y)];
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x * 2, DTid.y);
    
    float4 color;
    
    color = GetPixelColor(basepos.x, basepos.y, float2(1280, 720)) * weights[0].x;
    color += GetPixelColor(basepos.x + 1, basepos.y, float2(1280, 720)) * weights[0].y;
    color += GetPixelColor(basepos.x + 2, basepos.y, float2(1280, 720)) * weights[0].z;
    color += GetPixelColor(basepos.x + 3, basepos.y, float2(1280, 720)) * weights[0].w;
    color += GetPixelColor(basepos.x + 4, basepos.y, float2(1280, 720)) * weights[1].x;
    color += GetPixelColor(basepos.x + 5, basepos.y, float2(1280, 720)) * weights[1].y;
    color += GetPixelColor(basepos.x + 6, basepos.y, float2(1280, 720)) * weights[1].z;
    color += GetPixelColor(basepos.x + 7, basepos.y, float2(1280, 720)) * weights[1].w;
    
    color += GetPixelColor(basepos.x, basepos.y, float2(1280, 720)) * weights[0].x;
    color += GetPixelColor(basepos.x - 1, basepos.y, float2(1280, 720)) * weights[0].y;
    color += GetPixelColor(basepos.x - 2, basepos.y, float2(1280, 720)) * weights[0].z;
    color += GetPixelColor(basepos.x - 3, basepos.y, float2(1280, 720)) * weights[0].w;
    color += GetPixelColor(basepos.x - 4, basepos.y, float2(1280, 720)) * weights[1].x;
    color += GetPixelColor(basepos.x - 5, basepos.y, float2(1280, 720)) * weights[1].y;
    color += GetPixelColor(basepos.x - 6, basepos.y, float2(1280, 720)) * weights[1].z;
    color += GetPixelColor(basepos.x - 7, basepos.y, float2(1280, 720)) * weights[1].w;
    
    //OutputImg[DTid.xy] = float4(1, 0, 0, 1);
    //OutputImg[DTid.xy] = InputImg[DTid.xy];
    color = saturate(color);
    OutputImg[DTid.xy] = color;
    
}