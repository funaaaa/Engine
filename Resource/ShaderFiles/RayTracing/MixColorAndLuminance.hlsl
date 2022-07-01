
// ���͏��
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputAOLuminance : register(u1);
RWTexture2D<float4> InputLightLuminance : register(u2);
RWTexture2D<float4> InputGI : register(u3);

// �o�͐�UAV  
RWTexture2D<float4> OutputImg : register(u4);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // AO�̖��邳
    float4 aoLuminance = InputAOLuminance[DTid.xy];
    
    // ���C�e�B���O�̖��邳
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    // �e�N�X�`���̐F���
    float4 color = InputColor[DTid.xy];
    
    // GI�̏��
    float4 gi = InputGI[DTid.xy];
    // ���C�g�̖��邳�ɂ����GI����߂�B
    gi.x *= saturate(aoLuminance.x + lightLuminance.x);
    gi.y *= saturate(aoLuminance.y + lightLuminance.y);
    gi.z *= saturate(aoLuminance.z + lightLuminance.z);
    
    // �ŏI�I�ɑS�Ẵe�N�X�`�����������킹��B
    OutputImg[DTid.xy] = (aoLuminance + lightLuminance) * (color + gi);
    
}