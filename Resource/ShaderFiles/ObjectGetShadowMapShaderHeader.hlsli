/*-----定数1(変換行列と視点座標)-----*/
cbuffer cabuff0 : register(b0)
{
    matrix world; //ワールド変換行列
    matrix viewproj; //ビュープロジェクション行列
    float4 color; //色
    float3 eye; //視点座標
}

/*-----定数3(カメラライト行列)-----*/
cbuffer cbuff1 : register(b1)
{
    matrix lightViewProj; //ライトのビュープロジェクション行列
}

//頂点シェーダーから返されるデータ構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; //uv値
};