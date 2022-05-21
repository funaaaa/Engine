
struct constBuffData
{
    float r;
    float g;
    float b;
    float a;
};

// 入出力先UAV  
RWTexture2D<float4> inOutImg : register(u0);

[numthreads(4, 4, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basePos = uint2(DTid.x, DTid.y * 2);
    
    float4 color;
    
    constBuffData gaussianWeight[2];
    gaussianWeight[0].r = 2;
    gaussianWeight[0].g = 3;
    gaussianWeight[0].b = 4;
    gaussianWeight[0].a = 5;
    gaussianWeight[1].r = 2;
    gaussianWeight[1].g = 3;
    gaussianWeight[1].b = 4;
    gaussianWeight[1].a = 5;
    
    //基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    color = gaussianWeight[0].r * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    color += gaussianWeight[0].g * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 1, 0, 720))];
    color += gaussianWeight[0].b * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 2, 0, 720))];
    color += gaussianWeight[0].a * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 3, 0, 720))];
    color += gaussianWeight[1].r * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 4, 0, 720))];
    color += gaussianWeight[1].g * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 5, 0, 720))];
    color += gaussianWeight[1].b * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 6, 0, 720))];
    color += gaussianWeight[1].a * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 7, 0, 720))];
    
    //基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    color += gaussianWeight[0].r * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    color += gaussianWeight[0].g * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 1, 0, 720))];
    color += gaussianWeight[0].b * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 2, 0, 720))];
    color += gaussianWeight[0].a * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 3, 0, 720))];
    color += gaussianWeight[1].r * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 4, 0, 720))];
    color += gaussianWeight[1].g * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 5, 0, 720))];
    color += gaussianWeight[1].b * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 6, 0, 720))];
    color += gaussianWeight[1].a * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 7, 0, 720))];
    
    inOutImg[DTid.xy] = color;
    
}