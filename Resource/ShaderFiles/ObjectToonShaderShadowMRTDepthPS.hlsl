#include "ObjectMRTShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されてテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

PSOutput main(VSOutput input) : SV_TARGET
{
	//return float4(1,1,1,1);

    float4 shadeColor = float4(0, 0, 0, 1); //最終的に描画する影の色

	//平行光源
    for (int i = 0; i < DIRECTIONLIGHT_NUM; ++i)
    {
        if (dirLights[i].active == 1)
        {
			//環境反射光の計算
            float3 ambient = mAmbient;
			//拡散反射光の計算
            float3 diffuse = dot(-dirLights[i].lightv, input.normal) * mDiffuse;
			//光沢度
            const float shininess = 4.0f;
			//頂点から視点への方向ベクトル
            float3 eyedir = normalize(eye - input.worldpos.xyz);
			//反射光ベクトル
            float3 reflect = normalize(dirLights[i].lightv + 2 * dot(-dirLights[i].lightv, input.normal) * input.normal);
			//鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//すべて加算した色
            float3 color = (ambient + diffuse + specular) * dirLights[i].lightcolor;
            float3 outputColor = color;
            shadeColor.rgb += outputColor;
        }
    }
    
	//点光源
    for (int i = 0; i < POINTLIGHT_NUM; ++i)
    {
        if (pointLights[i].active == 1)
        {
			//ライトのベクトル
            float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			//ベクトルの長さ
            float d = length(lightv);
			//正規化して単位ベクトルにする
            lightv = normalize(lightv);
			//距離減衰係数
            float atten = 1.0f / (pointLights[i].lightatten.x +
				pointLights[i].lightatten.y * d +
				pointLights[i].lightatten.z * (d * d));
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
            float3 eyedir = normalize(eye - input.worldpos.xyz);
			//鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//すべて加算した色
            float3 outputColor = saturate(atten * (diffuse + specular) * pointLights[i].lightcolor);
            shadeColor.rgb += outputColor;
        }
    }
    
	//スポットライト
   // for (int i = 0; i < SPOTLIGHT_NUM; ++i)
   // {
   //     if (spotlights[i].active == 1)
   //     {
			////ライトへの方向ベクトル
   //         float3 lightv = spotlights[i].lightpos - input.worldpos.xyz;
   //         float d = length(lightv);
   //         lightv = normalize(lightv);
			////距離減衰係数
   //         float atten = saturate(1.0f / (spotlights[i].lightatten.x +
			//	spotlights[i].lightatten.y * d +
			//	spotlights[i].lightatten.z * d * d));
			////角度減衰
   //         float cos = dot(lightv, spotlights[i].lightv);
			////減衰開始角度から、減衰終了角度にかけて減衰
			////減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
   //         float angleatten = smoothstep(spotlights[i].lightfactoranglecos.y, spotlights[i].lightfactoranglecos.x, -cos);
			////角度減衰を乗算
   //         atten *= angleatten;
			////ライトに向かうベクトルと法線の内積
   //         float3 dotlightnormal = dot(lightv, input.normal);
			////頂点から視点への方向ベクトル
   //         float3 eyedir = normalize(eye - input.worldpos.xyz);
			////光沢度
   //         const float shininess = 4.0f;
			////反射光ベクトル
   //         float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			////拡散反射光
   //         float3 diffuse = dotlightnormal * mDiffuse;
			////鏡面反射光
   //         float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			////すべて加算
   //         float3 outputColor = atten * (diffuse + specular) * spotlights[i].lightcolor;
   //         shadeColor.rgb += outputColor;
   //     }
    //}
    
    //丸影の処理
    for (int i = 0; i < CIRCLESHADOW_NUM; ++i)
    {
        if (circleShadow[i].active == 1)
        {
    	    //オブジェクト表面からCASTERへのベクトル
            //float3 casterv = circleShadow[i].casterpos - input.worldpos.xyz;
            ////投影方向での距離
            //float d = dot(casterv, circleShadow[i].dir);
            ////距離減衰係数
            //float atten = saturate(1.0f / (circleShadow[i].atten.x +
            //circleShadow[i].atten.y * d +
            //circleShadow[i].atten.z * (d * d)));
            ////距離がマイナスなら0にする
            //atten *= step(0, d);
            ////仮想ライトの座標
            //float3 lightpos = circleShadow[i].casterpos + circleShadow[i].dir * circleShadow[i].distanceCasterLight;
            ////オブジェクト表面からライトへのベクトル(単位ベクトル)
            //float3 lightv = normalize(lightpos - input.worldpos.xyz);
            ////角度減衰
            //float cos = dot(lightv, circleShadow[i].dir);
            ////減衰開始角度から減衰終了角度にかけて減衰
            ////減衰開始角度の内側は1倍 減衰終了角度の外側は0度の輝度
            //float angleatten = smoothstep(circleShadow[i].factorAngleCos.y, circleShadow[i].factorAngleCos.x, cos);
            ////角度減衰を乗算
            //atten *= angleatten;
            ////すべてを減算する
            //shadeColor.rgb -= atten;
            
            float dis = distance(circleShadow[i].casterpos, input.worldpos.xyz);
            
            if (dis <= circleShadow[i].dir.x)
            {
                shadeColor.rgb = float3(0, 0, 0);
                
            }
            
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
    
    PSOutput output;
    output.color = (shadeColor * texcolor) * color;
    output.depth = float4(input.depth, input.depth, input.depth, 1);
    
    return output;

}