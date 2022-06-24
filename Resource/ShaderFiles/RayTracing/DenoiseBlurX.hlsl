
// ���͏��
RWTexture2D<float4> InputImg : register(u1);

// �o�͐�UAV  
RWTexture2D<float4> OutputImg : register(u2);

// �d�݃e�[�u��
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

// ���C�g���[�N�΍�
float4 LightLeakageCountermeasures(float4 baseColor, float4 targetColor, float weight, inout int differenColorCount, inout bool isDrawBaseColor)
{
    return targetColor * weight;
    
    // �`�F�b�N����B
    const int COLOR_LENGTH = 0.5f;
    const int DIRREFECT_COLOR_COUNT = 2;
    if (COLOR_LENGTH <= abs(baseColor.x - targetColor.x))
    {
        
        // �F�����ȏ㗣��Ă���B
        ++differenColorCount;
        
        //return baseColor * weight;
        
    }
    
    // �F���K��ȏ㗣��Ă�����A��Ղ̐F�����̂܂ܕԂ��悤�ɂ���B
    if (DIRREFECT_COLOR_COUNT < differenColorCount)
    {
        
        isDrawBaseColor = true;
        
    }
    
    // ��Ղ̐F��Ԃ����[�h��������B
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
    
    uint2 basepos = uint2(DTid.x * 2, DTid.y);
    
    float4 color;
    
    ////��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //color = gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x + 1, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x + 2, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x + 3, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x + 4, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x + 5, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x + 6, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x + 7, 0, 1280), clamp(basePos.y, 0, 720))];
    
    ////��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    //color += gaussianWeight[0].r * InputImg[uint2(clamp(basePos.x, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].g * InputImg[uint2(clamp(basePos.x - 1, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].b * InputImg[uint2(clamp(basePos.x - 2, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[0].a * InputImg[uint2(clamp(basePos.x - 3, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].r * InputImg[uint2(clamp(basePos.x - 4, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].g * InputImg[uint2(clamp(basePos.x - 5, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].b * InputImg[uint2(clamp(basePos.x - 6, 0, 1280), clamp(basePos.y, 0, 720))];
    //color += gaussianWeight[1].a * InputImg[uint2(clamp(basePos.x - 7, 0, 1280), clamp(basePos.y, 0, 720))];
    
    // ��Ղ̐F
    float4 baseColor = GetPixelColor(basepos.x, basepos.y, float2(1280, 720));
    
    // ��Ղ̐F����Ⴄ�F���ł��񐔁B
    int differenColorCount = 0;
    bool isDrawBaseColor = false;
    
    color = LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x, basepos.y, float2(1280, 720)), weights[0].x, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x + 1, basepos.y, float2(1280, 720)), weights[0].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x + 2, basepos.y, float2(1280, 720)), weights[0].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x + 3, basepos.y, float2(1280, 720)), weights[0].w, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x + 4, basepos.y, float2(1280, 720)), weights[1].x, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x + 5, basepos.y, float2(1280, 720)), weights[1].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x + 6, basepos.y, float2(1280, 720)), weights[1].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x + 7, basepos.y, float2(1280, 720)), weights[1].w, differenColorCount, isDrawBaseColor);
    
    // �ϐ����������B
    differenColorCount = 0;
    isDrawBaseColor = false;

    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x - 1, basepos.y, float2(1280, 720)), weights[0].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x - 2, basepos.y, float2(1280, 720)), weights[0].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x - 3, basepos.y, float2(1280, 720)), weights[0].w, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x - 4, basepos.y, float2(1280, 720)), weights[1].x, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x - 5, basepos.y, float2(1280, 720)), weights[1].y, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x - 6, basepos.y, float2(1280, 720)), weights[1].z, differenColorCount, isDrawBaseColor);
    color += LightLeakageCountermeasures(baseColor, GetPixelColor(basepos.x - 7, basepos.y, float2(1280, 720)), weights[1].w, differenColorCount, isDrawBaseColor);
    
    //OutputImg[DTid.xy] = float4(1, 0, 0, 1);
    //OutputImg[DTid.xy] = InputImg[DTid.xy];
    color = saturate(color);
    OutputImg[DTid.xy] = color;
    
}