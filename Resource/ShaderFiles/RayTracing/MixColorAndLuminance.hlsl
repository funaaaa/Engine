
// ??????
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputLightLuminance : register(u1);
RWTexture2D<float4> InputEmissive : register(u2);
RWTexture2D<float4> InputFog : register(u3);

// ?o???UAV  
RWTexture2D<float4> OutputImg : register(u4);

float Luminance(float3 v)
{
    return dot(v, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 ReinhardExtended(float3 v, float max_white)
{
    float maxColor = max_white * max_white;
    float3 numerator = v * (1.0f + (v / float3(maxColor, maxColor, maxColor)));
    return numerator / (1.0f + v);
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // AO?????
    float4 aoLuminance = InputColor[DTid.xy].w;
    
    // ???C?e?B???O?????
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    float4 emissive = InputEmissive[DTid.xy];
    
    float4 fog = InputFog[DTid.xy];
    
    // ?e?N?X?`????F???
    float4 color = InputColor[DTid.xy];
    color.w = 1.0f;
    

    
    // ??I?I??S???e?N?X?`??????????????B
    float3 resultColor = (aoLuminance.xyz + lightLuminance.xyz) * (color.xyz) + emissive.xyz + fog.xyz;
    
    OutputImg[DTid.xy] = float4(ReinhardExtended(resultColor, 1), 1.0f);
    OutputImg[DTid.xy] *= 0.9f;
    
}