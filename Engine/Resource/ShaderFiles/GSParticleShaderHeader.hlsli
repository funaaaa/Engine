/*-----定数1(変換行列と視点座標)-----*/
cbuffer cabuff0 : register(b0)
{
    matrix world; //ワールド変換行列
    matrix viewproj; //ビュープロジェクション行列
    float4 color; //色
    float3 eye;     	//視点座標
}

//頂点シェーダーから返されるデータ構造体
struct VSOutput
{
    float4 pos : POSITION; //システム用頂点座標
    //matrix scale;
};

//ジオメトリシェーダーから返されるデータ構造体
struct GSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //uv値
};