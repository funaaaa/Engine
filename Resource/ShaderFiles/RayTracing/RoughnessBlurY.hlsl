
// 反射色
RWTexture2D<float4> InputReflectionImg : register(u0);

// ラフネスマップ
RWTexture2D<float4> InputRoughnessImg : register(u1);

// デノイズをかける際にライトリーク等の対策をするためのマスク用テクスチャ
RWTexture2D<float4> InputMaskImg : register(u2);

// 出力先UAV
RWTexture2D<float4> OutputImg : register(u3);

// ガウシアンブラーの重み
cbuffer GaussianWeight : register(b0)
{
    float4 weights[2];
};

// テクスチャの色を取得
float4 GetPixelColor(int x, int y)
{
    uint2 texSize = uint2(1280 / 1, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputReflectionImg[uint2(x, y)];
}
float4 GetRoughnessColor(int x, int y)
{
    uint2 texSize = uint2(1280, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputRoughnessImg[uint2(x, y)];
}
// マスクテクスチャの色を取得
float4 GetMaskColor(int x, int y)
{
    uint2 texSize = uint2(1280 / 1, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputMaskImg[uint2(x, y)];
}

// ライトリーク対策用のサンプリング関数
float4 LightLeakageCountermeasures(float4 baseMaskColor, float4 targetMaskColor, float4 baseColor, float4 targetColor, float weight)
{
    
    // ある程度ならマスクの色の違いを許容するようにする。
    float subR = abs(baseMaskColor.x - targetMaskColor.x);
    float subG = abs(baseMaskColor.y - targetMaskColor.y);
    float subB = abs(baseMaskColor.z - targetMaskColor.z);
    const float SUB_NEAR = 0.1f;
    if (subR < SUB_NEAR && subG < SUB_NEAR && subB < SUB_NEAR)
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
    
    // ガウシアンブラーをかける際の基準の色
    float4 baseColor = GetPixelColor(basepos.x, basepos.y);
    float4 baseMaskColor = GetMaskColor(basepos.x, basepos.y);
    
    color = LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y), baseColor, GetPixelColor(basepos.x, basepos.y), weights[0].x * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 1), baseColor, GetPixelColor(basepos.x, basepos.y + 1), weights[0].y * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 2), baseColor, GetPixelColor(basepos.x, basepos.y + 2), weights[0].z * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 3), baseColor, GetPixelColor(basepos.x, basepos.y + 3), weights[0].w * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 4), baseColor, GetPixelColor(basepos.x, basepos.y + 4), weights[1].x * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 5), baseColor, GetPixelColor(basepos.x, basepos.y + 5), weights[1].y * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 6), baseColor, GetPixelColor(basepos.x, basepos.y + 6), weights[1].z * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y + 7), baseColor, GetPixelColor(basepos.x, basepos.y + 7), weights[1].w * GetRoughnessColor(basepos.x, basepos.y).x);

    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 1), baseColor, GetPixelColor(basepos.x, basepos.y - 1), weights[0].y * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 2), baseColor, GetPixelColor(basepos.x, basepos.y - 2), weights[0].z * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 3), baseColor, GetPixelColor(basepos.x, basepos.y - 3), weights[0].w * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 4), baseColor, GetPixelColor(basepos.x, basepos.y - 4), weights[1].x * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 5), baseColor, GetPixelColor(basepos.x, basepos.y - 5), weights[1].y * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 6), baseColor, GetPixelColor(basepos.x, basepos.y - 6), weights[1].z * GetRoughnessColor(basepos.x, basepos.y).x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y - 7), baseColor, GetPixelColor(basepos.x, basepos.y - 7), weights[1].w * GetRoughnessColor(basepos.x, basepos.y).x);
    
    
    color = saturate(color);
    
    OutputImg[DTid.xy] = color;
    
}