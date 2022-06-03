
// “ü—Íî•ñ
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputLuminance : register(u1);

// o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u2);

[numthreads(4, 4, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    float4 luminance = InputLuminance[DTid.xy];
    
    float4 color = InputColor[DTid.xy];
    
    OutputImg[DTid.xy] = luminance * 3.0f * color;
    
}