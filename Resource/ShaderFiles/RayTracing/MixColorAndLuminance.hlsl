
// ??????
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputAOLuminance : register(u1);
RWTexture2D<float4> InputLightLuminance : register(u2);
RWTexture2D<float4> InputGI : register(u3);

// ?o???UAV  
RWTexture2D<float4> OutputImg : register(u4);

float Luminance(float3 V)
{
    return dot(V, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 ChangeLumiance(float3 ColorIn, float LightOut)
{
    float l_in = Luminance(ColorIn);
    return ColorIn * (LightOut / l_in);
}

float3 ReinhardTonemap(float3 V, float MapWhite)
{
    float l_old = Luminance(V);
    float numerator = l_old * (1.0f + (l_old / (MapWhite * MapWhite)));
    float l_new = numerator / (1.0f + l_old);
    return ChangeLumiance(V, l_new);
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // AO?????
    float4 aoLuminance = InputAOLuminance[DTid.xy];
    
    // ???C?e?B???O?????
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    // ?e?N?X?`????F???
    float4 color = InputColor[DTid.xy];
    
    // GI????
    float4 gi = InputGI[DTid.xy];
    // ???C?g???????????GI??????B
    gi.x *= saturate(aoLuminance.x + lightLuminance.x);
    gi.y *= saturate(aoLuminance.y + lightLuminance.y);
    gi.z *= saturate(aoLuminance.z + lightLuminance.z);
    
    // ??I?I??S???e?N?X?`??????????????B
    float3 resultColor = (aoLuminance + lightLuminance + gi) * (color);
    resultColor = ReinhardTonemap(resultColor, 1.0f);
    OutputImg[DTid.xy] = float4(resultColor, 1);
    
}