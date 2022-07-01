
// 入力情報
RWTexture2D<float4> InputColor : register(u0);
RWTexture2D<float4> InputAOLuminance : register(u1);
RWTexture2D<float4> InputLightLuminance : register(u2);
RWTexture2D<float4> InputGI : register(u3);

// 出力先UAV  
RWTexture2D<float4> OutputImg : register(u4);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // AOの明るさ
    float4 aoLuminance = InputAOLuminance[DTid.xy];
    
    // ライティングの明るさ
    float4 lightLuminance = InputLightLuminance[DTid.xy];
    
    // テクスチャの色情報
    float4 color = InputColor[DTid.xy];
    
    // GIの情報
    float4 gi = InputGI[DTid.xy];
    // ライトの明るさによってGIを弱める。
    gi.x *= saturate(aoLuminance.x + lightLuminance.x);
    gi.y *= saturate(aoLuminance.y + lightLuminance.y);
    gi.z *= saturate(aoLuminance.z + lightLuminance.z);
    
    // 最終的に全てのテクスチャを混ぜ合わせる。
    OutputImg[DTid.xy] = (aoLuminance + lightLuminance) * (color + gi);
    
}