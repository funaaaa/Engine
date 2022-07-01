
// 入力情報
RWTexture2D<float4> InputImg : register(u0);
RWTexture2D<float4> InputMaskImg : register(u1);

// 出力先UAV  
RWTexture2D<float4> OutputImg : register(u2);

// 重みテーブル
cbuffer GaussianWeight : register(b0)
{
    float4 weights[2];
};

// デノイズ対象のテクスチャをサンプリング
float4 GetPixelColor(int x, int y)
{
    uint2 texSize = uint2(1280 / 1, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputImg[uint2(x, y)];
}
// マスクのテクスチャをサンプリング
float4 GetMaskColor(int x, int y)
{
    uint2 texSize = uint2(1280 / 1, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputMaskImg[uint2(x, y)];
}

// ライトリーク対策
float4 LightLeakageCountermeasures(float4 baseMaskColor, float4 targetMaskColor, float4 baseColor, float4 targetColor, float weight)
{

    
    // マスクの色が同じだったらそのターゲットの色を返す。
    if (baseMaskColor.x == targetMaskColor.x && baseMaskColor.y == targetMaskColor.y && baseMaskColor.z == targetMaskColor.z)
    {
        return targetColor * weight;
    }
    else
    {
        return baseColor * weight;
    }
    
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x, DTid.y * 1);
    
    float4 color = float4(0, 0, 0, 0);
    
    // 基盤の色
    float4 baseColor = GetPixelColor(basepos.x, basepos.y);
    float4 baseMaskColor = GetMaskColor(basepos.x, basepos.y);
    
    color = LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y), baseColor, GetPixelColor(basepos.x, basepos.y), weights[0].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 1), baseColor, GetPixelColor(basepos.x, basepos.y + 1), weights[0].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 2), baseColor, GetPixelColor(basepos.x, basepos.y + 2), weights[0].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 3), baseColor, GetPixelColor(basepos.x, basepos.y + 3), weights[0].w);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 4), baseColor, GetPixelColor(basepos.x, basepos.y + 4), weights[1].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 5), baseColor, GetPixelColor(basepos.x, basepos.y + 5), weights[1].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 6), baseColor, GetPixelColor(basepos.x, basepos.y + 6), weights[1].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 7), baseColor, GetPixelColor(basepos.x, basepos.y + 7), weights[1].w);

    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 1), baseColor, GetPixelColor(basepos.x, basepos.y - 1), weights[0].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 2), baseColor, GetPixelColor(basepos.x, basepos.y - 2), weights[0].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 3), baseColor, GetPixelColor(basepos.x, basepos.y - 3), weights[0].w);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 4), baseColor, GetPixelColor(basepos.x, basepos.y - 4), weights[1].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 5), baseColor, GetPixelColor(basepos.x, basepos.y - 5), weights[1].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 6), baseColor, GetPixelColor(basepos.x, basepos.y - 6), weights[1].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 7), baseColor, GetPixelColor(basepos.x, basepos.y - 7), weights[1].w);
    
    
    color = saturate(color);
    
    OutputImg[DTid.xy] = color;
    
}