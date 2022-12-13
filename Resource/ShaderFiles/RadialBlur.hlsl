
// �f�m�C�Y��������Ώۂ̃e�N�X�`��
RWTexture2D<float4> InputImg : register(u0);

// �o�͐�UAV
RWTexture2D<float4> OutputImg : register(u1);

// �K�E�V�A���u���[�̏d��
cbuffer GaussianWeight : register(b0)
{
    float4 weight;
};

// �e�N�X�`���̐F���擾
float4 GetPixelColor(int2 Pos)
{
    uint2 texSize = uint2(1280, 720);
    
    Pos.x = clamp(Pos.x, 0, texSize.x);
    Pos.y = clamp(Pos.y, 0, texSize.y);

    return InputImg[uint2(Pos.x, Pos.y)];
}


[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // ��ʒ����B
    float2 WIN_CENTER = float2(1280.0f / 2.0f, 720.0f / 2.0f);
    
    // �����̍ő�ʁB
    float MAX_LENGTH = length(WIN_CENTER) / 2.0f;
    
    // �ŏI�I�ȐF�B
    float4 color = float4(0, 0, 0, 0);
    
    // ���݂̃s�N�Z���̈ʒu�B
    float2 basepos = float2(DTid.x, DTid.y);
    
    // ��ʒ����܂ł̃x�N�g���B
    float2 blurDir = normalize(WIN_CENTER - basepos);
    
    // ���S�܂ł̋����B
    float centerLength = length(WIN_CENTER - basepos);
    
    // �����B
    float blurRate = saturate(centerLength / MAX_LENGTH);
    
    // �u���[�̉񐔁B
    int blurCount = (32.0f * blurRate) * weight.x + 1.0f;
    
    // �u���[��������B
    if ((uint) DTid.y < (uint) WIN_CENTER.y)
    {
        for (int index = 0; index < blurCount; ++index)
        {
        
            color += GetPixelColor(basepos + ceil(blurDir * index));
        
        }
    }
    else
    {
        for (int index = 0; index < blurCount; ++index)
        {
        
            color += GetPixelColor(basepos + floor(blurDir * index));
        
        }
    }
    
    // �u���[�̉񐔂Ŋ���B
    color /= blurCount;
    
    // �F��ۑ��B
    OutputImg[DTid.xy] = color;
    
}