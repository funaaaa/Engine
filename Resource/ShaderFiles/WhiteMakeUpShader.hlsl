// o—ÍæUAV
RWTexture2D<float4> OutputImg : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x * 1, DTid.y);
    
    OutputImg[basepos] = float4(1, 1, 1, 1);
    
}