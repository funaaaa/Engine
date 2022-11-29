
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

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // �o�C���j�A�t�B���^��������
    uint2 basepos = uint2(DTid.x / 2, DTid.y / 2);
    float2 texSize = float2(1280 / 2.0f, 720 / 2.0f);
    float4 color = GetPixelColor(basepos.x, basepos.y, texSize);
    color += GetPixelColor(basepos.x, basepos.y, texSize);
    color += GetPixelColor(basepos.x, basepos.y, texSize);
    color += GetPixelColor(basepos.x, basepos.y, texSize);

    // ���d���ς����
    color /= 4.0f;
    OutputImg[DTid.xy] = color;
    
}