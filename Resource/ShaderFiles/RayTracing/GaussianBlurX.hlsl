
// ���o�͐�UAV  
RWTexture2D<float4> inOutImg : register(u0);

[numthreads(4, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //uint2 basePos = uint2(DTid.x * 2, DTid.y);
    
    //float4 color;
    
    //float4 gaussianWeight[2];
    //gaussianWeight[0].r = 2;
    //gaussianWeight[0].g = 3;
    //gaussianWeight[0].b = 4;
    //gaussianWeight[0].a = 5;
    //gaussianWeight[1].r = 2;
    //gaussianWeight[1].g = 3;
    //gaussianWeight[1].b = 4;
    //gaussianWeight[1].a = 5;
    
    ////��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //color = gaussianWeight[0].r * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * inOutImg[uint2(clamp(basePos.x + 1, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].b * inOutImg[uint2(clamp(basePos.x + 2, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].a * inOutImg[uint2(clamp(basePos.x + 3, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].r * inOutImg[uint2(clamp(basePos.x + 4, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].g * inOutImg[uint2(clamp(basePos.x + 5, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].b * inOutImg[uint2(clamp(basePos.x + 6, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].a * inOutImg[uint2(clamp(basePos.x + 7, 0, 1280), clamp(basePos.y, 0, 720))];
    
    ////��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //color += gaussianWeight[0].r * inOutImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * inOutImg[uint2(clamp(basePos.x - 1, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].b * inOutImg[uint2(clamp(basePos.x - 2, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].a * inOutImg[uint2(clamp(basePos.x - 3, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].r * inOutImg[uint2(clamp(basePos.x - 4, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].g * inOutImg[uint2(clamp(basePos.x - 5, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].b * inOutImg[uint2(clamp(basePos.x - 6, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].a * inOutImg[uint2(clamp(basePos.x - 7, 0, 1280), clamp(basePos.y, 0, 720))];
    
    inOutImg[DTid.xy] = 1;
    
}