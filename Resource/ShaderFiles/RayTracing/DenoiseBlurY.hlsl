
// ���͏��
RWTexture2D<float4> InputImg : register(u0);

// �o�͐�UAV  
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
    
    uint2 basepos = uint2(DTid.x, DTid.y * 2);
    
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
    
    ////��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //color = gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 1, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 2, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 3, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 4, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 5, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 6, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y + 7, 0, 720))];
    
    ////��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //color += gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 1, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 2, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 3, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 4, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 5, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 6, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y - 7, 0, 720))];
    
    color = GetPixelColor(basepos.x, basepos.y, float2(1280 / 2.0f, 720)) * weights[0].x;
    color += GetPixelColor(basepos.x, basepos.y + 1, float2(1280 / 2.0f, 720)) * weights[0].y;
    color += GetPixelColor(basepos.x, basepos.y + 2, float2(1280 / 2.0f, 720)) * weights[0].z;
    color += GetPixelColor(basepos.x, basepos.y + 3, float2(1280 / 2.0f, 720)) * weights[0].w;
    color += GetPixelColor(basepos.x, basepos.y + 4, float2(1280 / 2.0f, 720)) * weights[1].x;
    color += GetPixelColor(basepos.x, basepos.y + 5, float2(1280 / 2.0f, 720)) * weights[1].y;
    color += GetPixelColor(basepos.x, basepos.y + 6, float2(1280 / 2.0f, 720)) * weights[1].z;
    color += GetPixelColor(basepos.x, basepos.y + 7, float2(1280 / 2.0f, 720)) * weights[1].w;

    color += GetPixelColor(basepos.x, basepos.y - 1, float2(1280 / 2.0f, 720)) * weights[0].y;
    color += GetPixelColor(basepos.x, basepos.y - 2, float2(1280 / 2.0f, 720)) * weights[0].z;
    color += GetPixelColor(basepos.x, basepos.y - 3, float2(1280 / 2.0f, 720)) * weights[0].w;
    color += GetPixelColor(basepos.x, basepos.y - 4, float2(1280 / 2.0f, 720)) * weights[1].x;
    color += GetPixelColor(basepos.x, basepos.y - 5, float2(1280 / 2.0f, 720)) * weights[1].y;
    color += GetPixelColor(basepos.x, basepos.y - 6, float2(1280 / 2.0f, 720)) * weights[1].z;
    color += GetPixelColor(basepos.x, basepos.y - 7, float2(1280 / 2.0f, 720)) * weights[1].w;
    
    
    color = saturate(color);
    
    OutputImg[DTid.xy] = color;
    
}