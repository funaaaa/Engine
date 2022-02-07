/*-----定数1(変換行列と視点座標)-----*/
cbuffer cabuff0 : register(b0)
{
    matrix world; //ワールド変換行列
    matrix viewproj; //ビュープロジェクション行列
    float4 color; //色
    float3 eye; //視点座標
}

/*-----定数2(マテリアル)-----*/
cbuffer cbuff1 : register(b1)
{
    float3 mAmbient : packoffset(c0); //アンビエント係数
    float3 mDiffuse : packoffset(c1); //ディフーズ係数
    float3 mSpecular : packoffset(c2); //スペキュラー係数
    float mAlpha : packoffset(c2.w); //アルファ
}

/*-----定数3(ライト関係)-----*/
//点光源の数
#define POINTLIGHT_NUM 120
//面光源の数
#define DIRECTIONLIGHT_NUM 1
//スポットライトの数
#define SPOTLIGHT_NUM 1
//丸影の数
#define CIRCLESHADOW_NUM 120
//点光源のデータ
struct PointLight
{
    float3 lightpos; //ライト座標
    float3 lightcolor; //ライトの色
    float3 lightatten; //ライト距離減衰係数
    uint active; //アクティブフラグ
};
//面光源のデータ
struct DirLight
{
    float3 lightv; //光線ベクトル
    float3 lightcolor; //ライトの色
    uint active; //アクティブフラグ
};
//スポットライトのデータ
struct SpotLight
{
    float3 lightv; //ライトの方向ベクトル
    float3 lightpos; //ライト座標
    float3 lightcolor; //ライトの色
    float3 lightatten; //ライト距離減衰係数
    float2 lightfactoranglecos; //ライト角度減衰係数
    uint active; //アクティブフラグ
};
//丸影のデータ
struct CircleShadow
{
    float3 dir; //投影方向の逆ベクトル(単位ベクトル)
    float3 casterpos; //CASTER座標
    float distanceCasterLight; //キャスターとライトの距離
    float3 atten; //距離減衰係数
    float2 factorAngleCos; //減衰角度のコサイン
    uint active; //アクティブフラグ
};
cbuffer cbuff2 : register(b2)
{
    PointLight pointLights[POINTLIGHT_NUM]; //点光源
    DirLight dirLights[DIRECTIONLIGHT_NUM]; //面光源
    SpotLight spotlights[SPOTLIGHT_NUM]; //スポットライト
    CircleShadow circleShadow[CIRCLESHADOW_NUM]; //丸影
}

//頂点シェーダーから返されるデータ構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; //uv値
    float1 depth : TEXCOORD2; //Z値
};

//ピクセルシェーダーから返されるデータ構造体
struct PSOutput
{
    float4 color : SV_Target0; //レンダーターゲット0に書き込む
    float4 depth : SV_Target1; //レンダーターゲット1に書き込む
};