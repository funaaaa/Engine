
// �o�͐�UAV
RWTexture3D<float4> OutputImg : register(u0);

[numthreads(8, 8, 4)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // �F��ۑ��B
    OutputImg[DTid.xyz] = float4(1,1,1,1);
    
}