
// デノイズをかける対象のテクスチャ
RWTexture2D<float4> InputImg : register(u0);

// 出力先UAV
RWTexture2D<float4> OutputImg : register(u1);

// ガウシアンブラーの重み
cbuffer GaussianWeight : register(b0)
{
    float4 weight;
};

// テクスチャの色を取得
float4 GetPixelColor(int2 Pos)
{
    uint2 texSize = uint2(1280, 720);
    
    Pos.x = clamp(Pos.x, 0, texSize.x);
    Pos.y = clamp(Pos.y, 0, texSize.y);

    return InputImg[uint2(Pos.x, Pos.y)];
}


[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // 画面中央。
    float2 WIN_CENTER = float2(1280.0f / 2.0f, 720.0f / 2.0f);
    
    // 距離の最大量。
    float MAX_LENGTH = length(WIN_CENTER) / 2.0f;
    
    // 最終的な色。
    float4 color = float4(0, 0, 0, 0);
    
    // 現在のピクセルの位置。
    float2 basepos = float2(DTid.x, DTid.y);
    
    // 画面中央までのベクトル。
    float2 blurDir = normalize(WIN_CENTER - basepos);
    
    // 中心までの距離。
    float centerLength = length(WIN_CENTER - basepos);
    
    // 割合。
    float blurRate = saturate(centerLength / MAX_LENGTH);
    
    // ブラーの回数。
    int blurCount = (32.0f * blurRate) * weight.x + 1.0f;
    
    // ブラーをかける。
    if ((uint) DTid.y < (uint) WIN_CENTER.y)
    {
        for (int index = 0; index < blurCount; ++index)
        {
        
            color += GetPixelColor(basepos + ceil(blurDir * index));
        
        }
    }
    else
    {
        for (int index = 0; index < blurCount; ++index)
        {
        
            color += GetPixelColor(basepos + floor(blurDir * index));
        
        }
    }
    
    // ブラーの回数で割る。
    color /= blurCount;
    
    // 色を保存。
    OutputImg[DTid.xy] = color;
    
}