#include "GSParticleShaderHeader.hlsli"

//頂点数
static const uint vnum = 24;

//真ん中からのオフセット
static const float4 offsetArray[vnum] =
{
    float4(-1.0f, -1.0f, -1.0f, 0), //前面左下
	float4(-1.0f, 1.0f, -1.0f, 0), //前面左上
	float4(1.0f, -1.0f, -1.0f, 0), //前面右下
	float4(1.0f, 1.0f, -1.0f, 0), //前面右上

	float4(1.0f, -1.0f, 1.0f, 0), //奥面右下
	float4(1.0f, 1.0f, 1.0f, 0), //奥面右上
	float4(-1.0f, -1.0f, 1.0f, 0), //奥面左下
	float4(-1.0f, 1.0f, 1.0f, 0), //奥面左上

	float4(1.0f, -1.0f, -1.0f, 0), //右面左下
	float4(1.0f, 1.0f, -1.0f, 0), //右面左上
	float4(1.0f, -1.0f, 1.0f, 0), //右面右下
	float4(1.0f, 1.0f, 1.0f, 0), //右面右上

	float4(-1.0f, -1.0f, 1.0f, 0), //左面左下
	float4(-1.0f, 1.0f, 1.0f, 0), //左面左上
	float4(-1.0f, -1.0f, -1.0f, 0), //左面右下
	float4(-1.0f, 1.0f, -1.0f, 0), //左面右上

	float4(-1.0f, 1.0f, -1.0f, 0), //上面左下
	float4(-1.0f, 1.0f, 1.0f, 0), //上面左上
	float4(1.0f, 1.0f, -1.0f, 0), //上面右下
	float4(1.0f, 1.0f, 1.0f, 0), //上面右上

	float4(-1.0f, -1.0f, 1.0f, 0), //下面左下
	float4(-1.0f, -1.0f, -1.0f, 0), //下面左上
	float4(1.0f, -1.0f, 1.0f, 0), //下面右下
	float4(1.0f, -1.0f, -1.0f, 0), //下面右上
};

//uvのオフセット
static const float2 offsetUVArray[vnum] =
{
    float2(0, 1), //前面左下
	float2(0, 0), //前面左上
	float2(1, 1), //前面右下
	float2(1, 0), //前面右上

	float2(0, 1), //奥面左下
	float2(0, 0), //奥面左上
	float2(1, 1), //奥面右下
	float2(1, 0), //奥面右上

	float2(0, 1), //右面左下
	float2(0, 0), //右面左上
	float2(1, 1), //右面右下
	float2(1, 0), //右面右上

	float2(0, 1), //左面左下
	float2(0, 0), //左面左上
	float2(1, 1), //左面右下
	float2(1, 0), //左面右上

	float2(0, 1), //上面左下
	float2(0, 0), //上面左上
	float2(1, 1), //上面右下
	float2(1, 0), //上面右上

	float2(0, 1), //下面左下
	float2(0, 0), //下面左上
	float2(1, 1), //下面右下
	float2(1, 0), //下面右上
};

[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element;
	//全頂点分まわす
    for (uint i = 0; i < 4; i++)
    {
        GSOutput element;
        element.svpos = input[0].pos + offsetArray[i];

        element.svpos = mul(world, element.svpos);

        //element.uv = float2(noise(0.5f), noise(0.5f));
        //element.uv = float2(noise(i / 2.0f / vnum), noise(i / 2.0f / vnum));
        //element.uv = float2(0.5f, 0.5f);
        element.uv = offsetUVArray[i];

        output.Append(element);
    }
    output.RestartStrip();
    for (i = 4; i < 8; i++)
    {
        GSOutput element;
        element.svpos = input[0].pos + offsetArray[i];

        element.svpos = mul(world, element.svpos);

        //element.uv = float2(noise(0.5f), noise(0.5f));
        //element.uv = float2(noise(i / 2.0f / vnum), noise(i / 2.0f / vnum));
        //element.uv = float2(0.5f, 0.5f);
        element.uv = offsetUVArray[i];

        output.Append(element);
    }
    output.RestartStrip();
    for (i = 8; i < 12; i++)
    {
        GSOutput element;
        element.svpos = input[0].pos + offsetArray[i];

        element.svpos = mul(world, element.svpos);

        //element.uv = float2(noise(0.5f), noise(0.5f));
        //element.uv = float2(noise(i / 2.0f / vnum), noise(i / 2.0f / vnum));
        //element.uv = float2(0.5f, 0.5f);
        element.uv = offsetUVArray[i];

        output.Append(element);
    }
    output.RestartStrip();
    for (i = 12; i < 16; i++)
    {
        GSOutput element;
        element.svpos = input[0].pos + offsetArray[i];

        element.svpos = mul(world, element.svpos);

        //element.uv = float2(noise(0.5f), noise(0.5f));
        //element.uv = float2(noise(i / 2.0f / vnum), noise(i / 2.0f / vnum));
        //element.uv = float2(0.5f, 0.5f);
        element.uv = offsetUVArray[i];

        output.Append(element);
    }
    output.RestartStrip();
    for (i = 16; i < 20; i++)
    {
        GSOutput element;
        element.svpos = input[0].pos + offsetArray[i];

        element.svpos = mul(world, element.svpos);

        //element.uv = float2(noise(0.5f), noise(0.5f));
        //element.uv = float2(noise(i / 2.0f / vnum), noise(i / 2.0f / vnum));
        //element.uv = float2(0.5f, 0.5f);
        element.uv = offsetUVArray[i];

        output.Append(element);
    }
    output.RestartStrip();
    for (i = 20; i < 24; i++)
    {
        GSOutput element;
        element.svpos = input[0].pos + offsetArray[i];

        element.svpos = mul(world, element.svpos);

        //element.uv = float2(noise(0.5f), noise(0.5f));
        //element.uv = float2(noise(i / 2.0f / vnum), noise(i / 2.0f / vnum));
        //element.uv = float2(0.5f, 0.5f);
        element.uv = offsetUVArray[i];

        output.Append(element);
    }
    output.RestartStrip();

}



//全頂点を指定して描画してみる