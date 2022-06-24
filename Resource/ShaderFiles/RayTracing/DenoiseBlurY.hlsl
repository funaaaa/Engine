
// 入力情報
RWTexture2D<float4> InputImg : register(u1);

// 出力先UAV  
RWTexture2D<float4> OutputImg : register(u2);

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

// ライトリーク対策
float4 LightLeakageCountermeasures(float4 baseColor, float4 targetColor, float weight, inout int differenColorCount, inout bool isDrawBaseColor)
{
    return targetColor * weight;
    
    // チェックする。
    const int COLOR_LENGTH = 0.5f;
    const int DIRREFECT_COLOR_COUNT = 2;
    if (COLOR_LENGTH <= abs(baseColor.x - targetColor.x))
    {
        
        // 色が一定以上離れている。
        ++differenColorCount;
        
        //return baseColor * weight;
        
    }
    
    // 色が規定以上離れていたら、基盤の色をそのまま返すようにする。
    if (DIRREFECT_COLOR_COUNT < differenColorCount)
    {
        
        isDrawBaseColor = true;
        
    }
    
    // 基盤の色を返すモードだったら。
    if (isDrawBaseColor)
    {
        
        return baseColor * weight;
        
    }
    else
    {
        
        return targetColor * weight;
        
    }
    
    
}

[numthreads(4, 4, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x, DTid.y * 2);
    
    float4 color;
    
    ////基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    //color = gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 1, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 2, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 3, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 4, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 5, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 6, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 7, 0, 720))];
    
    ////基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    //color += gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 1, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 2, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 3, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 4, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 5, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 6, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 7, 0, 720))];
    
    float4 baseColor = GetPixelColor(basepos.x, basepos.y, float2(1280, 720));
    
    // 基盤の色から違う色がでた回数。
    int differenColorCount = 0;
    bool isDrawBaseColor = false;
    
    color = LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y, float2(1280 / 2.0f, 720)), weights[0].x, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y + 1, float2(1280 / 2.0f, 720)), weights[0].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y + 2, float2(1280 / 2.0f, 720)), weights[0].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y + 3, float2(1280 / 2.0f, 720)), weights[0].w, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y + 4, float2(1280 / 2.0f, 720)), weights[1].x, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y + 5, float2(1280 / 2.0f, 720)), weights[1].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y + 6, float2(1280 / 2.0f, 720)), weights[1].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y + 7, float2(1280 / 2.0f, 720)), weights[1].w, differenColorCount, isDrawBaseColor);

    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y - 1, float2(1280 / 2.0f, 720)), weights[0].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y - 2, float2(1280 / 2.0f, 720)), weights[0].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y - 3, float2(1280 / 2.0f, 720)), weights[0].w, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y - 4, float2(1280 / 2.0f, 720)), weights[1].x, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y - 5, float2(1280 / 2.0f, 720)), weights[1].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y - 6, float2(1280 / 2.0f, 720)), weights[1].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y - 7, float2(1280 / 2.0f, 720)), weights[1].w, differenColorCount, isDrawBaseColor);
    
    
    color = saturate(color);
    
    OutputImg[DTid.xy] = color;
    
}