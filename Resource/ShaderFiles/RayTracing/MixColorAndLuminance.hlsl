
// ??????
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputAOLuminance : register(u1);
RWTexture2D<float4> InputLightLuminance : register(u2);
RWTexture2D<float4> InputReflection : register(u3);

// ?o???UAV  
RWTexture2D<float4> OutputImg : register(u4);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // AO?????
    float4 aoLuminance = InputAOLuminance[DTid.xy];
    
    // ???C?e?B???O?????
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    // ?e?N?X?`????F???
    float4 color = InputColor[DTid.xy];
    
    // ??I?I??S???e?N?X?`??????????????B
    OutputImg[DTid.xy] = (aoLuminance + lightLuminance) * (color + InputReflection[DTid.xy]);
    OutputImg[DTid.xy] = (color + InputReflection[DTid.xy]) * float4(1, 1, 1, 1);
    
}