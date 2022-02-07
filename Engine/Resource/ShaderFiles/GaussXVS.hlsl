#include "MultiPathShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VSOutput output;
    output.svpos = mul(mul(viewproj, world), pos); //座標に行列を乗算
    
    float4 inormal = float4(normal, 0);
	//output.normal = inormal;
    output.normal = normalize(mul(world, inormal).rgb); //法線にもワールド変換を行う
    
    //テクスチャのサイズを取得
    float2 texSize = 0;
    float level = 0;
    tex.GetDimensions(0, texSize.x, texSize.y, level);
    
    //基準テクセルのuv座標を計算
    float2 standardTex = uv;
    
    //基準テクセルからV座標を+1テクセルずらすためのオフセットを計算する
    output.uv.tex0.xy = float2(1.0f / texSize.x, 0.0f);
    
    //基準テクセルからV座標を+3テクセルずらすためのオフセットを計算する
    output.uv.tex1.xy = float2(5.0f / texSize.x, 0.0f);
    
    //基準テクセルからV座標を+5テクセルずらすためのオフセットを計算する
    output.uv.tex2.xy = float2(9.0f / texSize.x, 0.0f);
    
    //基準テクセルからV座標を+7テクセルずらすためのオフセットを計算する
    output.uv.tex3.xy = float2(13.0f / texSize.x, 0.0f);
    
    //基準テクセルからV座標を+9テクセルずらすためのオフセットを計算する
    output.uv.tex4.xy = float2(17.0f / texSize.x, 0.0f);
    
    //基準テクセルからV座標を+11テクセルずらすためのオフセットを計算する
    output.uv.tex5.xy = float2(21.0f / texSize.x, 0.0f);
    
    //基準テクセルからV座標を+13テクセルずらすためのオフセットを計算する
    output.uv.tex6.xy = float2(25.0f / texSize.x, 0.0f);
    
    //基準テクセルからV座標を+15テクセルずらすためのオフセットを計算する
    output.uv.tex7.xy = float2(29.0f / texSize.x, 0.0f);
    
    //オフセットに-1を掛けてマイナス方向のオフセットも計算する
    output.uv.tex0.zw = output.uv.tex0.xy * -1.0f;
    output.uv.tex1.zw = output.uv.tex1.xy * -1.0f;
    output.uv.tex2.zw = output.uv.tex2.xy * -1.0f;
    output.uv.tex3.zw = output.uv.tex3.xy * -1.0f;
    output.uv.tex4.zw = output.uv.tex4.xy * -1.0f;
    output.uv.tex5.zw = output.uv.tex5.xy * -1.0f;
    output.uv.tex6.zw = output.uv.tex6.xy * -1.0f;
    output.uv.tex7.zw = output.uv.tex7.xy * -1.0f;
    
    //オフセットに基準テクセルのuv座標を足して、実際にサンプリングするuv座標に変換する
    output.uv.tex0 += float4(standardTex, standardTex);
    output.uv.tex1 += float4(standardTex, standardTex);
    output.uv.tex2 += float4(standardTex, standardTex);
    output.uv.tex3 += float4(standardTex, standardTex);
    output.uv.tex4 += float4(standardTex, standardTex);
    output.uv.tex5 += float4(standardTex, standardTex);
    output.uv.tex6 += float4(standardTex, standardTex);
    output.uv.tex7 += float4(standardTex, standardTex);
    
    return output;
}