
// ���_���
struct Vertex
{
    float3 Position;
    float3 Normal;
    float4 Color;
};

// �����
struct SceneCB
{
    matrix mtxView; // �r���[�s��.
    matrix mtxProj; // �v���W�F�N�V�����s��.
    matrix mtxViewInv; // �r���[�t�s��.
    matrix mtxProjInv; // �v���W�F�N�V�����t�s��.
    float4 lightDirection; // ���s�����̌���.
    float4 lightColor; // ���s�����F.
    float4 ambientColor; // ����.
};

// �O���[�o�����[�g�V�O�l�`��
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneCB> gSceneParam : register(b0);

// �q�b�g�O���[�v�̃��[�J�����[�g�V�O�l�`��
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);

// RayGeneration�V�F�[�_�[�̃��[�J�����[�g�V�O�l�`��
RWTexture2D<float4> gOutput : register(u0);

// �y�C���[�h �F�����擾���邽�߂̍\����
struct Payload
{
    float3 color;
};
// �A�g���r���[�g ���������ʒu���擾���邽�߂̍\����
struct MyAttribute
{
    float2 barys;
};

// ���������ʒu���v�Z����֐�
inline float3 CalcBarycentrics(float2 barys)
{
    return float3(1.0 - barys.x - barys.y, barys.x, barys.y);
}

// ���������ʒu�̏����擾����֐�
Vertex GetHitVertex(MyAttribute attrib)
{
    Vertex v = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attrib.barys);
    uint vertexId = PrimitiveIndex() * 3; // Triangle List �̂���.
    float weights[3] =
    {
        barycentrics.x, barycentrics.y, barycentrics.z
    };
    for (int i = 0; i < 3; ++i)
    {
        uint index = indexBuffer[vertexId + i];
        float w = weights[i];
        v.Position += vertexBuffer[index].Position * w;
        v.Normal += vertexBuffer[index].Normal * w;
        v.Color += vertexBuffer[index].Color * w;
    }
    v.Normal = normalize(v.Normal);
    return v;
}

// RayGeneration�V�F�[�_�[
[shader("raygeneration")]
void mainRayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);

    float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
    float aspect = dims.x / dims.y;

    matrix mtxViewInv = gSceneParam.mtxViewInv;
    matrix mtxProjInv = gSceneParam.mtxProjInv;

    // ���C�̐ݒ�
    RayDesc rayDesc;
    rayDesc.Origin = mul(mtxViewInv, float4(0, 0, 0, 1)).xyz;

    float4 target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
    float3 dir = mul(mtxViewInv, float4(target.xyz, 0)).xyz;

    rayDesc.Direction = normalize(dir);
    rayDesc.TMin = 0;
    rayDesc.TMax = 100000;

    // �y�C���[�h�̐ݒ�
    Payload payload;
    payload.color = float3(0, 0, 0);

    // TransRay�ɕK�v�Ȑݒ���쐬
    uint rayMask = 0xFF;

    // ���C�𔭎�
    TraceRay(
    gRtScene, // TLAS
    0, // �Փ˔��萧�������t���O
    rayMask, // �Փ˔���Ώۂ̃}�X�N�l
    0, // ray index
    1, // MultiplierForGeometryContrib
    0, // miss index
    rayDesc,
    payload);

    // ���C�𔭎˂������ʂ̐F���擾
    float3 col = payload.color;

    // ���ʊi�[
    gOutput[launchIndex.xy] = float4(col, 1);

}

// miss�V�F�[�_�[ ���C���q�b�g���Ȃ��������ɌĂ΂��V�F�[�_�[
[shader("miss")]
void mainMS(inout Payload payload)
{

    // �P�F��Ԃ��悤�ɂ���B
    payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);

}

// closesthit�V�F�[�_�[ ���C���q�b�g�������ɌĂ΂��V�F�[�_�[
[shader("closesthit")]
void mainCHS(inout Payload payload, MyAttribute attrib)
{

    Vertex vtx = GetHitVertex(attrib);
    // Lambert ���C�e�B���O���s��.
    float3 lightDir = -normalize(gSceneParam.lightDirection.xyz);
    float nl = saturate(dot(vtx.Normal, lightDir));
    float3 lightColor = gSceneParam.lightColor.xyz;
    float3 ambientColor = gSceneParam.ambientColor.xyz;
    float3 color = 0;
    color += lightColor * vtx.Color.xyz * nl;
    color += ambientColor * vtx.Color.xyz;
    payload.color = color;

}
