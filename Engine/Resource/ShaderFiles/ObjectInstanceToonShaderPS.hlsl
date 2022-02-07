#include "ObjectInstanceShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(1,1,1,1);

    float4 shadeColor = float4(0, 0, 0, 1); //最終的に描画する影の色

    //平行光源
    for (int i = 0; i < DIRECTIONLIGHT_NUM; ++i)
    {
        if (cbuffData2.dirLights[i].active == 1)
        {
			//環境反射光の計算
            float3 ambient = mAmbient;
			//拡散反射光の計算
            float3 diffuse = dot(-cbuffData2.dirLights[i].lightv, input.normal) * mDiffuse;
			//光沢度
            const float shininess = 4.0f;
			//頂点から視点への方向ベクトル
            float3 eyedir = normalize(cbuff0Data[input.instanceID].eye - input.worldpos.xyz);
			//反射光ベクトル
            float3 reflect = normalize(cbuffData2.dirLights[i].lightv + 2 * dot(-cbuffData2.dirLights[i].lightv, input.normal) * input.normal);
			//鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//すべて加算した色
            float3 color = (ambient + diffuse + specular) * cbuffData2.dirLights[i].lightcolor;
            float3 outputColor = color;
            shadeColor.rgb += outputColor;
        }
    }
	//点光源
    for (int i = 0; i < POINTLIGHT_NUM; ++i)
    {
        if (cbuffData2.pointLights[i].active == 1)
        {
			//ライトのベクトル
            float3 lightv = cbuffData2.pointLights[i].lightpos - input.worldpos.xyz;
			//ベクトルの長さ
            float d = length(lightv);
			//正規化して単位ベクトルにする
            lightv = normalize(lightv);
			//距離減衰係数
            float atten = 1.0f / (cbuffData2.pointLights[i].lightatten.x +
				cbuffData2.pointLights[i].lightatten.y * d +
				cbuffData2.pointLights[i].lightatten.z * (d * d));
            atten *= 1000;
			//ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal);
			//反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			//拡散反射光
            float3 diffuse = dotlightnormal * mDiffuse;
			//光沢度
            const float shininess = 4.0f;
			//頂点から視点への方向ベクトル
            float3 eyedir = normalize(cbuff0Data[input.instanceID].eye - input.worldpos.xyz);
			//鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//すべて加算した色
            float3 outputColor = saturate(atten * (diffuse + specular) * cbuffData2.pointLights[i].lightcolor);
            shadeColor.rgb += outputColor;
        }
    }
    //スポットライト
    for (int i = 0; i < SPOTLIGHT_NUM; ++i)
    {
        if (cbuffData2.spotlights[i].active == 1)
        {
			//ライトへの方向ベクトル
            float3 lightv = cbuffData2.spotlights[i].lightpos - input.worldpos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
			//距離減衰係数
            float atten = saturate(1.0f / (cbuffData2.spotlights[i].lightatten.x +
				cbuffData2.spotlights[i].lightatten.y * d +
				cbuffData2.spotlights[i].lightatten.z * d * d));
			//角度減衰
            float cos = dot(lightv, cbuffData2.spotlights[i].lightv);
			//減衰開始角度から、減衰終了角度にかけて減衰
			//減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(cbuffData2.spotlights[i].lightfactoranglecos.y, cbuffData2.spotlights[i].lightfactoranglecos.x, -cos);
			//角度減衰を乗算
            atten *= angleatten;
			//ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal);
			//頂点から視点への方向ベクトル
            float3 eyedir = normalize(cbuff0Data[input.instanceID].eye - input.worldpos.xyz);
			//光沢度
            const float shininess = 4.0f;
			//反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			//拡散反射光
            float3 diffuse = dotlightnormal * mDiffuse;
			//鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//すべて加算
            float3 outputColor = atten * (diffuse + specular) * cbuffData2.spotlights[i].lightcolor;
            shadeColor.rgb += outputColor;
        }
    }
    
    //赤成分
    float buff = 0.35f;
    float buff2 = 0.65f;
    float buff3 = 1.0f;
    if (shadeColor.r < buff)
    {
        shadeColor.r = buff;
    }
    else if (shadeColor.r < buff2)
    {
        shadeColor.r = buff2;
    }
    else
    {
        shadeColor.r = buff3;
    }
    
    //緑成分
    if (shadeColor.g < buff)
    {
        shadeColor.g = buff;
    }
    else if (shadeColor.g < buff2)
    {
        shadeColor.g = buff2;
    }
    else
    {
        shadeColor.g = buff3;
    }
    
    //青成分
    if (shadeColor.b < buff)
    {
        shadeColor.b = buff;
    }
    else if (shadeColor.b < buff2)
    {
        shadeColor.b = buff2;
    }
    else
    {
        shadeColor.b = buff3;
    }

	//テクスチャの色
    float4 texcolor = tex.Sample(smp, input.uv);
    return (shadeColor * texcolor) * cbuff0Data[input.instanceID].color;

}