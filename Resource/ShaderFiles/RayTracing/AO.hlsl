
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// �O���[�o�����[�g�V�O�l�`��
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneAOCB> gSceneParam : register(b0);

// �q�b�g�O���[�v�̃��[�J�����[�g�V�O�l�`��
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> texture : register(t2, space1);
// �T���v���[
SamplerState smp : register(s0, space1);

// RayGeneration�V�F�[�_�[�̃��[�J�����[�g�V�O�l�`��
RWTexture2D<float4> gOutput : register(u0);

struct AOPayload
{
    float3 color;
    float3 light;
    uint recursive;
};

float GetRandomNumber(float2 texCoord, int Seed)
{
    return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

// ����
void Reflection(inout AOPayload payload, Vertex vtx)
{
    // �x�N�g���������_���ŋ��߂Ĕ��˂�����B
    float3 random = float3(GetRandomNumber(float2(vtx.Position.x, vtx.Position.y), int(gSceneParam.seed.x + gSceneParam.seed.y)) * 2.0f - 1.0f,
    GetRandomNumber(float2(vtx.Position.x, vtx.Position.y), int(gSceneParam.seed.x + gSceneParam.seed.y)) * 2.0f - 1.0f,
    GetRandomNumber(float2(vtx.Position.x, vtx.Position.y), int(gSceneParam.seed.x + gSceneParam.seed.y)) * 2.0f - 1.0f);
    
    // -1 ~ 1 �̗��������߂�ꂽ�̂ŁA�l������������B
    random /= 10.0f;
    
    float3 worldRayDir = WorldRayDirection();
    float3 worldNormal = mul(vtx.Normal, (float3x3) ObjectToWorld4x3());
    float3 reflectDir = reflect(worldRayDir, worldNormal);
    reflectDir += random;
    reflectDir = normalize(reflectDir);
    
    // ���C�𔭎ˁB
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    uint rayMask = 0xFF;

    RayDesc rayDesc;
    rayDesc.Origin = worldPos;
    rayDesc.Direction = reflectDir;
    rayDesc.TMin = 0.01;
    rayDesc.TMax = 100000;

    TraceRay(
        gRtScene,
        0,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        payload);
}

// RayGeneration�V�F�[�_�[
[shader("raygeneration")]
void mainAORayGen()
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
    AOPayload payload;
    payload.color = float3(0, 0, 0);
    payload.light = float3(0, 0, 0);
    payload.recursive = 0;

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
    
    // �擾�����F�����s�񐔂Ŋ���B
    payload.color /= payload.recursive;

    // ���C�𔭎˂������ʂ̐F���擾
    float3 col = payload.color;

    // ���ʊi�[
    gOutput[launchIndex.xy] = float4(col, 1);

}

// miss�V�F�[�_�[ ���C���q�b�g���Ȃ��������ɌĂ΂��V�F�[�_�[
[shader("miss")]
void mainAOMS(inout AOPayload payload)
{

    // �P�F��Ԃ��悤�ɂ���B
    //payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);
    //payload.color = float3(0x32 / 255.0f, 0x90 / 255.0f, 0xD0 / 255.0f); // ��̐F
    //payload.color = float3(0x01 / 255.0f, 0x01 / 255.0f, 0x01 / 255.0f);

}

// closesthit�V�F�[�_�[ ���C���q�b�g�������ɌĂ΂��V�F�[�_�[
[shader("closesthit")]
void mainAOCHS(inout AOPayload payload, MyAttribute attrib)
{

    // ���C�̔��ː�
    const int RECURSICE_RAY_COUNT = 3;

    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();

    // �Ăяo���񐔂������𒴂��Ȃ��悤�ɂ���B
    if (RECURSICE_RAY_COUNT < payload.recursive)
    {
        return; // �Ăяo���񐔂��z�����瑦���^�[��.
    }
    
    // ���������̂�0(���C�g��������)
    if (instanceID == 0)
    {
        payload.light = 1.0f;
        return;
    }
    
    // ���C�̊J�n�n�_
    float3 rayStart = WorldRayOrigin() + (WorldRayDirection() * RayTMin());
    // ���C�̏I���n�_
    float3 rayEnd = WorldRayOrigin() + (WorldRayDirection() * RayTCurrent());
    // ����
    float rayLength = length(rayStart - rayEnd);
    // ���C�̒����̊���l
    const float RAY_LENGTH = 1000;
    // ���C�̒������K��l�ȉ��������烊�^�[��
    if (rayLength < RAY_LENGTH)
    {
       // return;
    }
    
    // �Ăяo���񐔂��X�V�B
    ++payload.recursive;

    // lambert ���C�e�B���O���s��.
    float3 lightdir = -normalize(gSceneParam.lightDirection.xyz);

    float nl = saturate(dot(vtx.Normal, lightdir));

    float3 lightcolor = gSceneParam.lightColor.xyz;
    float3 ambientcolor = gSceneParam.ambientColor.xyz;
    float4 materialcolor = texture.SampleLevel(smp, vtx.uv, 0.0f);
    float3 color = 0;
    color += lightcolor * materialcolor.xyz * nl;
    color += ambientcolor * materialcolor.xyz;
    
    // ���C�e�B���O�̌��ʂ̐F��ۑ��B
    payload.color += color;
    
    Reflection(payload, vtx);

}