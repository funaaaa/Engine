
// デノイズをかける対象のテクスチャ
RWTexture2D<float4> InputImg : register(u0);

// デノイズをかける際にライトリーク等の対策をするためのマスク用テクスチャ
RWTexture2D<float4> InputMaskImg : register(u1);

// 出力先UAV
RWTexture2D<float4> OutputImg : register(u2);

// ガウシアンブラーの重み
cbuffer GaussianWeight : register(b0)
{
    float4 weights[2];
};

// テクスチャの色を取得
float4 GetPixelColor(int x, int y)
{
    uint2 texSize = uint2(1280, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputImg[uint2(x, y)];
}
// マスクテクスチャの色を取得
float4 GetMaskColor(int x, int y)
{
    uint2 texSize = uint2(1280, 720);
    
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
    if (subR + subG + subB < SUB_NEAR * 3.0f)
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
    
    uint2 basepos = uint2(DTid.x * 1, DTid.y);
    
    float4 color = float4(0, 0, 0, 0);
    
    // ガウシアンブラーをかける際の基準の色
    float4 baseColor = GetPixelColor(basepos.x, basepos.y);
    float4 baseMaskColor = GetMaskColor(basepos.x, basepos.y);
    
    color = LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y), baseColor, GetPixelColor(basepos.x, basepos.y), weights[0].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 1, basepos.y), baseColor, GetPixelColor(basepos.x + 1, basepos.y), weights[0].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 2, basepos.y), baseColor, GetPixelColor(basepos.x + 2, basepos.y), weights[0].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 3, basepos.y), baseColor, GetPixelColor(basepos.x + 3, basepos.y), weights[0].w);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 4, basepos.y), baseColor, GetPixelColor(basepos.x + 4, basepos.y), weights[1].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 5, basepos.y), baseColor, GetPixelColor(basepos.x + 5, basepos.y), weights[1].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 6, basepos.y), baseColor, GetPixelColor(basepos.x + 6, basepos.y), weights[1].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 7, basepos.y), baseColor, GetPixelColor(basepos.x + 7, basepos.y), weights[1].w);


    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x - 1, basepos.y), baseColor, GetPixelColor(basepos.x - 1, basepos.y), weights[0].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x - 2, basepos.y), baseColor, GetPixelColor(basepos.x - 2, basepos.y), weights[0].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x - 3, basepos.y), baseColor, GetPixelColor(basepos.x - 3, basepos.y), weights[0].w);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x - 4, basepos.y), baseColor, GetPixelColor(basepos.x - 4, basepos.y), weights[1].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x - 5, basepos.y), baseColor, GetPixelColor(basepos.x - 5, basepos.y), weights[1].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x - 6, basepos.y), baseColor, GetPixelColor(basepos.x - 6, basepos.y), weights[1].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x - 7, basepos.y), baseColor, GetPixelColor(basepos.x - 7, basepos.y), weights[1].w);
    
    //OutputImg[DTid.xy] = float4(1, 0, 0, 1);
    //OutputImg[DTid.xy] = InputImg[DTid.xy];
    color = saturate(color);
    OutputImg[DTid.xy] = color;
    
}