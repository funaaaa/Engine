// ����UAV
RWTexture2D<float4> InputImg : register(u0);

// �o�͐�UAV
RWTexture2D<float4> OutputImg : register(u1);

// ����������ꏊ�w��萔�o�b�t�@
cbuffer WriteUV : register(b0)
{
    float2 uv1_;
    float2 uv2_;
    float2 uv3_;
    float2 uv4_;
    
    float2 prevuv1_;
    float2 prevuv2_;
    float2 prevuv3_;
    float2 prevuv4_;
};

// �������ފ֐�
void WriteColor(uint2 WriteUV, float4 color)
{
    // �A���t�@�l��1����Ȃ�������
    if (OutputImg[WriteUV].w != 1)
    {
        OutputImg[WriteUV] = color;
    }
}

// ��������
void WriteOutputImg(uint2 WriteUV)
{
    
    OutputImg[uint2(WriteUV.x, WriteUV.y)] = float4(1, 1, 1, 1);
    
    // �㉺���E�ɔ����o���B
    WriteColor(uint2(WriteUV.x + 1, WriteUV.y + 1), float4(0.4f, 0.4f, 0.4f, 1));
    WriteColor(uint2(WriteUV.x - 1, WriteUV.y + 1), float4(0.4f, 0.4f, 0.4f, 1));
    WriteColor(uint2(WriteUV.x - 1, WriteUV.y - 1), float4(0.4f, 0.4f, 0.4f, 1));
    WriteColor(uint2(WriteUV.x + 1, WriteUV.y - 1), float4(0.4f, 0.4f, 0.4f, 1));
    WriteColor(uint2(WriteUV.x, WriteUV.y + 1), float4(0.4f, 0.4f, 0.4f, 1));
    WriteColor(uint2(WriteUV.x, WriteUV.y - 1), float4(0.4f, 0.4f, 0.4f, 1));
    WriteColor(uint2(WriteUV.x - 1, WriteUV.y), float4(0.4f, 0.4f, 0.4f, 1));
    WriteColor(uint2(WriteUV.x + 1, WriteUV.y), float4(0.4f, 0.4f, 0.4f, 1));
    
    
};

// �O�t���[����UV�Ƃ��܃t���[����UV�̒l���Ԃ��ď������ށB
void Interpolation(uint2 WriteUV, uint2 PrevWriteUV)
{
    
    if (WriteUV.x == 0 || WriteUV.y == 0 || PrevWriteUV.x == 0 || PrevWriteUV.y == 0)
    {
        return;
    }
    
    // ��Ԃ���̒��������߂�B
    float interpolationLength = length(WriteUV - PrevWriteUV);
    
    // ��Ԃ���x�N�g�������߂�B
    float2 interpolationVec = normalize(WriteUV - PrevWriteUV);
    
    // �I�_�Ǝn�_���������ށB
    WriteOutputImg(WriteUV);
    WriteOutputImg(PrevWriteUV);
    
    // ���Ԓn�_�ɂ��������ށB
    WriteOutputImg(WriteUV * 0.1f + PrevWriteUV * 0.9f);
    WriteOutputImg(WriteUV * 0.2f + PrevWriteUV * 0.8f);
    WriteOutputImg(WriteUV * 0.3f + PrevWriteUV * 0.7f);
    WriteOutputImg(WriteUV * 0.4f + PrevWriteUV * 0.6f);
    WriteOutputImg(WriteUV * 0.5f + PrevWriteUV * 0.5f);
    WriteOutputImg(WriteUV * 0.6f + PrevWriteUV * 0.4f);
    WriteOutputImg(WriteUV * 0.7f + PrevWriteUV * 0.3f);
    WriteOutputImg(WriteUV * 0.8f + PrevWriteUV * 0.2f);
    WriteOutputImg(WriteUV * 0.9f + PrevWriteUV * 0.1f);
    
    //if (1000000 < interpolationLength)
    //{
    //    OutputImg[uint2(WriteUV.x, WriteUV.y)] = float4(1, 0, 0, 1);
    //}
    //else
    //{
    
    //    // ��Ԃ��ď������ށB
    //    for (int index = 0; index < interpolationLength; ++index)
    //    {
        
    //        WriteOutputImg(PrevWriteUV + uint2(interpolationVec * index));
        
    //    }
        
    //}
    
};

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // ���
    Interpolation(uint2(uv1_.x * 4096, uv1_.y * 4096), uint2(prevuv1_.x * 4096, prevuv1_.y * 4096));
    
    // ���
    Interpolation(uint2(uv2_.x * 4096, uv2_.y * 4096), uint2(prevuv2_.x * 4096, prevuv2_.y * 4096));
    
    //// �O��
    //OutputImg[uint2(uv_[2].x * 4096, uv_[2].y * 4096)] = float4(1, 1, 1, 1);
    
    //// �l��
    //OutputImg[uint2(uv_[3].x * 4096, uv_[3].y * 4096)] = float4(1, 1, 1, 1);
    
}