
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
    
    uint2 basepos = uint2(DTid.x * 2, DTid.y);
    
    float4 color;
    
    float4 weights[2];
    weights[0].r = 0.1f;
    weights[0].g = 0.1f;
    weights[0].b = 0.1f;
    weights[0].a = 0.1f;
    weights[1].r = 0.1f;
    weights[1].g = 0.1f;
    weights[1].b = 0.1f;
    weights[1].a = 0.1f;
    
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
    
    color = GetPixelColor(basepos.x, basepos.y, float2(1280, 720)) * weights[0].x;
    color += GetPixelColor(basepos.x + 1, basepos.y, float2(1280, 720)) * weights[0].y;
    color += GetPixelColor(basepos.x + 2, basepos.y, float2(1280, 720)) * weights[0].z;
    color += GetPixelColor(basepos.x + 3, basepos.y, float2(1280, 720)) * weights[0].w;
    color += GetPixelColor(basepos.x + 4, basepos.y, float2(1280, 720)) * weights[1].x;
    color += GetPixelColor(basepos.x + 5, basepos.y, float2(1280, 720)) * weights[1].y;
    color += GetPixelColor(basepos.x + 6, basepos.y, float2(1280, 720)) * weights[1].z;
    color += GetPixelColor(basepos.x + 7, basepos.y, float2(1280, 720)) * weights[1].w;

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