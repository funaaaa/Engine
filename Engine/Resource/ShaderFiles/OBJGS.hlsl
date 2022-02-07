#include "GSParticleShaderHeader.hlsli"

[maxvertexcount(3)]
void main(
	triangle VSOutput input[3] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
	//GSOutput element;
	//for (uint i = 0; i < 3; i++)
	//{
	//	element.svpos = input[i].svpos;
	//	element.worldpos = input[i].worldpos;
	//	element.normal = input[i].normal;
	//	element.uv = input[i].uv;
	//	output.Append(element);
	//}
}