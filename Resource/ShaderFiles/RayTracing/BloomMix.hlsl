
// “ü—Íî•ñ
RWTexture2D<float4> InputImg0 : register(u0);
RWTexture2D<float4> InputImg1 : register(u1);
RWTexture2D<float4> InputImg2 : register(u2);
RWTexture2D<float4> InputImg3 : register(u3);

// o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u4);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x, DTid.y);
    
    float4 color;
    
    color = InputImg0[basepos];
    color += InputImg1[basepos];
    color += InputImg2[basepos];
    color += InputImg3[basepos];
    
    color /= 4.0f;
    
    OutputImg[DTid.xy] = color;
    
}