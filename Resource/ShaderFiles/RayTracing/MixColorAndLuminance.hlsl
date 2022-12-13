
// ??????
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputLightLuminance : register(u1);
RWTexture2D<float4> InputEmissive : register(u2);

// ?o???UAV  
RWTexture2D<float4> OutputImg : register(u3);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // AO?????
    float4 aoLuminance = InputColor[DTid.xy].w;
    
    // ???C?e?B???O?????
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    float4 emissive = InputEmissive[DTid.xy];
    
    // ?e?N?X?`????F???
    float4 color = InputColor[DTid.xy];
    color.w = 1.0f;
    
    // ??I?I??S???e?N?X?`??????????????B
    OutputImg[DTid.xy] = (aoLuminance + lightLuminance) * (color);
    OutputImg[DTid.xy] += emissive;
    
}