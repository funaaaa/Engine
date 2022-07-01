
// “ü—Íî•ñ
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputLuminance : register(u1);
RWTexture2D<float4> InputLightLuminance : register(u2);
RWTexture2D<float4> InputGI : register(u3);

// o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u4);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    float4 luminance = InputLuminance[DTid.xy];
    
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    float4 color = InputColor[DTid.xy];
    
    float4 gi = InputGI[DTid.xy];
    gi.x *= saturate(luminance.x + lightLuminance.x);
    gi.y *= saturate(luminance.y + lightLuminance.y);
    gi.z *= saturate(luminance.z + lightLuminance.z);
    
    OutputImg[DTid.xy] = (luminance + lightLuminance) * (color + gi);
    
}