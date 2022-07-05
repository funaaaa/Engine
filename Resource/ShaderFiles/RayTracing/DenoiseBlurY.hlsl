
// ??????
RWTexture2D<float4> InputImg : register(u0);
RWTexture2D<float4> InputMaskImg : register(u1);

// ?o???UAV  
RWTexture2D<float4> OutputImg : register(u2);

// ?d??e?[?u??
cbuffer GaussianWeight : register(b0)
{
    float4 weights[2];
};

// ?f?m?C?Y????e?N?X?`?????T???v?????O
float4 GetPixelColor(int x, int y)
{
    uint2 texSize = uint2(1280 / 1, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputImg[uint2(x, y)];
}
// ?}?X?N??e?N?X?`?????T???v?????O
float4 GetMaskColor(int x, int y)
{
    uint2 texSize = uint2(1280 / 1, 720);
    
    x = clamp(x, 0, texSize.x);
    y = clamp(y, 0, texSize.y);

    return InputMaskImg[uint2(x, y)];
}

// ???C?g???[?N???
float4 LightLeakageCountermeasures(float4 baseMaskColor, float4 targetMaskColor, float4 baseColor, float4 targetColor, float weight)
{

    
    // ?}?X?N??F?????????????úb??^?[?Q?b?g??F?????B
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
    
    // ????F
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