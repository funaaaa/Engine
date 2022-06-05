
// “ü—Íî•ñ
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputLuminance : register(u1);
RWTexture2D<float4> InputLightLuminance : register(u2);

// o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u3);

[numthreads(4, 4, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    float4 luminance = InputLuminance[DTid.xy];
    
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    float4 color = InputColor[DTid.xy];
    
    OutputImg[DTid.xy] = (luminance + lightLuminance) * color;
    
}