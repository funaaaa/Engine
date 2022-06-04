
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// �O���[�o�����[�g�V�O�l�`��
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneCB> gSceneParam : register(b0);

// �q�b�g�O���[�v�̃��[�J�����[�g�V�O�l�`��
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> debugTexImg : register(t2, space1);
Texture2D<float4> debugTexImg2 : register(t3, space1); // �g���ĂȂ�
RWTexture2D<float4> gOutput : register(u0, space1);
// �T���v���[
SamplerState smp : register(s0, space1);

// RayGeneration�V�F�[�_�[�̃��[�J�����[�g�V�O�l�`��
RWTexture2D<float4> debugTex : register(u0);

// ���������ʒu�̏����擾����֐�
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer)
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
        v.uv += vertexBuffer[index].uv * w;
    }
    v.Normal = normalize(v.Normal);

    return v;
}

// ���˂�����B
void Reflection(float3 vertexPosition, float3 vertexNormal, inout Payload payload)
{
    float3 worldPos = mul(float4(vertexPosition, 1), ObjectToWorld4x3());
    float3 worldNormal = mul(vertexNormal, (float3x3) ObjectToWorld4x3());
    float3 worldRayDir = WorldRayDirection();
    float3 reflectDir = reflect(worldRayDir, worldNormal);
    
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
    payload.recursive = 0;

    // TransRay�ɕK�v�Ȑݒ���쐬
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    //flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�𖳎��B

    // ���C�𔭎�
    TraceRay(
    gRtScene, // TLAS
    flag, // �Փ˔��萧�������t���O
    rayMask, // �Փ˔���Ώۂ̃}�X�N�l
    0, // ray index
    1, // MultiplierForGeometryContrib
    0, // miss index
    rayDesc,
    payload);
    
    
    // ���C�𔭎˂������ʂ̐F���擾
    float3 col = payload.color;

    debugTex[launchIndex.xy] = float4(col, 1);

}

// miss�V�F�[�_�[ ���C���q�b�g���Ȃ��������ɌĂ΂��V�F�[�_�[
[shader("miss")]
void mainMS(inout Payload payload)
{

    // �P�F��Ԃ��悤�ɂ���B
    //payload.color = float3(0xFF / 255.0f, 0xFF / 255.0f, 0xE5 / 255.0f);
    //payload.color = float3(0x32 / 255.0f, 0x90 / 255.0f, 0xD0 / 255.0f);

}

// �V���h�E�pmiss�V�F�[�_�[
[shader("miss")]
void shadowMS(inout ShadowPayload payload)
{
    // ���ɂ��������Ă��Ȃ��Ƃ������ƂȂ̂ŁA�e�͐������Ȃ��B
    payload.isShadow = true;
}

// closesthit�V�F�[�_�[ ���C���q�b�g�������ɌĂ΂��V�F�[�_�[
[shader("closesthit")]
void mainCHS(inout Payload payload, MyAttribute attrib)
{
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    
    // �Ăяo���񐔂������𒴂��Ȃ��悤�ɂ���B
    if (checkRecursiveLimit(payload, 1))
    {
        return; // �Ăяo���񐔂��z�����瑦���^�[��.
    }
    
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
        
        // �B���x����
    float visibility = 0.0f;
        
    const int SAMPLING_COUNT = 30;
        
        // ��΂����C�̉�
    for (int index = 0; index < SAMPLING_COUNT; ++index)
    {
                
            // �����𐶐����ă��C���΂����������߂�B
        float randSeedX = (frac(sin(dot(vtx.Position.xy + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
        float randSeedY = (frac(sin(dot(vtx.Position.xz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(78.233, 12.9898)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
        float randSeedZ = (frac(sin(dot(vtx.Position.yz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(32.9898, 48.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
        randSeedX = randSeedX * 2.0f - 1.0f;
        randSeedY = randSeedY * 2.0f - 1.0f;
        randSeedZ = randSeedZ * 2.0f - 1.0f;
        float3 sampleDir = float3(randSeedX, randSeedY, randSeedZ);
            
            // �V���h�E���C���΂��B
        float smpleVisiblity = ShootAOShadowRay(vtx.Position, sampleDir, 500, gRtScene);
            
            // �B���x���� += �T���v�����O�����l * �R�T�C���� / �m�����x�֐�
        float nol = saturate(dot(vtx.Normal, sampleDir));
        float pdf = 1.0 / (2.0 * PI);
        visibility += smpleVisiblity * nol / pdf;
                
    }
        // ���ς����B
    visibility = (1.0f / PI) * (1.0f / float(SAMPLING_COUNT)) * visibility;
        
        // �B���x���������E�𒴂��Ȃ��悤�ɂ���B
    visibility = saturate(visibility);

        // �ŏI���ʂ̐F��ۑ��B
    float3 texColor = debugTexImg.SampleLevel(smp, vtx.uv, 0.0f).xyz;
    payload.color.xyz = float3(visibility, visibility, visibility) * texColor;
    
    uint uavSizeX = 0;
    uint uavSizeY = 0;
    gOutput.GetDimensions(uavSizeX, uavSizeY);
    gOutput[uint2(uavSizeY * vtx.uv.y, uavSizeX * vtx.uv.x)] = float4(float3(visibility, visibility, visibility), 1);
        
    Reflection(vtx.Position, vtx.Normal, payload);

}

// closesthit�V�F�[�_�[ �V���h�E�p
[shader("closesthit")]
void shadowCHS(inout ShadowPayload payload, MyAttribute attrib)
{
}

// �A���t�@����AnyHitShader
[shader("anyhit")]
void mainAnyHit(inout Payload payload, MyAttribute attrib)
{
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    float4 diffuse = 0;
    if (diffuse.w < 0.5f)
    {
        IgnoreHit();

    }
    
    int instanceID = InstanceID();
    
    // �C���X�^���XID��3(���C�g)�Ȃ瓖���蔻������p����B
    if (instanceID == 3)
    {
        IgnoreHit();

    }
    
}
