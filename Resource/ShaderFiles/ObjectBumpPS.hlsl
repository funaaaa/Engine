#include "ObjectShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> normalMap : register(t1); //1版スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(1,1,1,1);

	//ノーマルマップから法線を抜き取る
    input.normal = normalMap.Sample(smp, input.uv);
    //input.normal = normalize(input.normal);

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
            //shadeColor.rgb += outputColor;
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
   // }
    //丸影

   // for (int i = 0; i < CIRCLESHADOW_NUM; ++i)
   // {
   //     if (circleShadow[i].active == 1)
   //     {
			////オブジェクト表面からCASTERへのベクトル
   //         float3 casterv = circleShadow[i].casterpos - input.worldpos.xyz;
   //         //投影方向での距離
   //         float d = dot(casterv, circleShadow[i].dir);
   //         //距離減衰係数
   //         float atten = saturate(1.0f / (circleShadow[i].atten.x +
   //         circleShadow[i].atten.y * d +
   //         circleShadow[i].atten.z * d * d));
   //         //距離がマイナスなら0にする
   //         atten *= step(0, d);
   //         //仮想ライトの座標
   //         float3 lightpos = circleShadow[i].casterpos + circleShadow[i].dir * circleShadow[i].distanceCasterLight;
   //         //オブジェクト表面からライトへのベクトル(単位ベクトル)
   //         float3 lightv = normalize(lightpos - input.worldpos.xyz);
   //         //角度減衰
   //         float cos = dot(lightv, circleShadow[i].dir);
   //         //減衰開始角度から減衰終了角度にかけて減衰
   //         //減衰開始角度の内側は1倍 減衰終了角度の外側は0度の輝度
   //         float angleatten = smoothstep(circleShadow[i].factorAngleCos.y, circleShadow[i].factorAngleCos.x, cos);
   //         //角度減衰を乗算
   //         atten *= angleatten;
   //         //すべてを減産する
   //         shadeColor.rgb -= atten;
   //     }
   // }

	//テクスチャの色
    float4 texcolor = tex.Sample(smp, input.uv);
    return (shadeColor * texcolor) * color;

}