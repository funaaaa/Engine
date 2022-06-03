
// 入力情報
RWTexture2D<float4> InputImg : register(u0);

// 出力先UAV  
RWTexture2D<float4> OutputImg : register(u1);

float4 GetPixelColor(int x, int y, int2 texSize)
{
    x = clamp(0, texSize.x, x);
    y = clamp(0, texSize.y, y);

    return InputImg[uint2(x, y)];
}

[numthreads(4, 4, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // バイリニアフィルタをかける
    uint2 basepos = uint2(DTid.x / 1, DTid.y / 1);
    float2 texSize = float2(1280 / 1.0f, 720 / 1.0f);
    float offset = 1;
    float4 color = GetPixelColor(basepos.x, basepos.y, texSize);
    color += GetPixelColor(basepos.x, basepos.y + offset, texSize);
    color += GetPixelColor(basepos.x + offset, basepos.y, texSize);
    color += GetPixelColor(basepos.x + offset, basepos.y + offset, texSize);

    // 加重平均を取る
    color /= 4.0f;
    OutputImg[DTid.xy] = color;
    
}