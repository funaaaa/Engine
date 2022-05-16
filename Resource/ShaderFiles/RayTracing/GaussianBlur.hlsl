
struct constBuffData
{
    float x;
    float y;
    float z;
    float w;
};

// 入出力先UAV  
RWTexture2D<float4> inOutImg : register(u0);

[numthreads(1280, 720, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float4 color;
    
    constBuffData gaussianWeight[2];
    gaussianWeight[0].x = 2;
    gaussianWeight[0].y = 3;
    gaussianWeight[0].z = 4;
    gaussianWeight[0].w = 5;
    gaussianWeight[1].x = 2;
    gaussianWeight[1].y = 3;
    gaussianWeight[1].z = 4;
    gaussianWeight[1].w = 5;
    
    //基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    color = gaussianWeight[0].x * inOutImg[uint2(clamp(DTid.x, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[0].y * inOutImg[uint2(clamp(DTid.x + 1, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[0].z * inOutImg[uint2(clamp(DTid.x + 2, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[0].w * inOutImg[uint2(clamp(DTid.x + 3, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].x * inOutImg[uint2(clamp(DTid.x + 4, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].y * inOutImg[uint2(clamp(DTid.x + 5, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].z * inOutImg[uint2(clamp(DTid.x + 6, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].w * inOutImg[uint2(clamp(DTid.x + 7, 0, 1280), clamp(DTid.y, 0, 720))];
    
    //基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    color += gaussianWeight[0].x * inOutImg[uint2(clamp(DTid.x, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[0].y * inOutImg[uint2(clamp(DTid.x - 1, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[0].z * inOutImg[uint2(clamp(DTid.x - 2, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[0].w * inOutImg[uint2(clamp(DTid.x - 3, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].x * inOutImg[uint2(clamp(DTid.x - 4, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].y * inOutImg[uint2(clamp(DTid.x - 5, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].z * inOutImg[uint2(clamp(DTid.x - 6, 0, 1280), clamp(DTid.y, 0, 720))];
    color += gaussianWeight[1].w * inOutImg[uint2(clamp(DTid.x - 7, 0, 1280), clamp(DTid.y, 0, 720))];
    
    inOutImg[DTid.xy] = color;
    
}