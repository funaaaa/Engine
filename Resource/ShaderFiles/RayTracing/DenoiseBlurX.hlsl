
// 入力情報
RWTexture2D<float4> InputImg : register(u1);
RWTexture2D<float4> InputMaskImg : register(u2);

// 出力先UAV  
RWTexture2D<float4> OutputImg : register(u3);

// 重みテーブル
cbuffer GaussianWeight : register(b0)
{
    float4 weights[2];
};

float4 GetPixelColor(int x, int y)
{
    float2 texSize = float2(1280, 720);
    
    x = clamp(0, texSize.x, x);
    y = clamp(0, texSize.y, y);

    return InputImg[uint2(x, y)];
}
float4 GetMaskColor(int x, int y)
{
    float2 texSize = float2(1280, 720);
    
    x = clamp(0, texSize.x, x);
    y = clamp(0, texSize.y, y);

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

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x * 1, DTid.y);
    
    float4 color;
    
    ////基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    //color = gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x + 1, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x + 2, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x + 3, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x + 4, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x + 5, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x + 6, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x + 7, 0, 1280), clamp(basePos.y, 0, 720))];
    
    ////基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    //color += gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x - 1, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x - 2, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x - 3, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x - 4, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x - 5, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x - 6, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x - 7, 0, 1280), clamp(basePos.y, 0, 720))];
    
    // 基盤の色
    float4 baseColor = GetPixelColor(basepos.x, basepos.y);
    float4 baseMaskColor = GetMaskColor(basepos.x, basepos.y);
    
    // 基盤の色から違う色がでた回数。
    int differenColorCount = 0;
    bool isDrawBaseColor = false;
    
    color = LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x, basepos.y), baseColor, GetPixelColor(basepos.x, basepos.y), weights[0].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 1, basepos.y), baseColor, GetPixelColor(basepos.x + 1, basepos.y), weights[0].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 2, basepos.y), baseColor, GetPixelColor(basepos.x + 2, basepos.y), weights[0].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 3, basepos.y), baseColor, GetPixelColor(basepos.x + 3, basepos.y), weights[0].w);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 4, basepos.y), baseColor, GetPixelColor(basepos.x + 4, basepos.y), weights[1].x);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 5, basepos.y), baseColor, GetPixelColor(basepos.x + 5, basepos.y), weights[1].y);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 6, basepos.y), baseColor, GetPixelColor(basepos.x + 6, basepos.y), weights[1].z);
    color += LightLeakageCountermeasures(baseMaskColor, GetMaskColor(basepos.x + 7, basepos.y), baseColor, GetPixelColor(basepos.x + 7, basepos.y), weights[1].w);
    
    // 変数を初期化。
    differenColorCount = 0;
    isDrawBaseColor = false;

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