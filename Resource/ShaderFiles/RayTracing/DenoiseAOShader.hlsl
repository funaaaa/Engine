
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// �O���[�o�����[�g�V�O�l�`��
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneCB> gSceneParam : register(b0);

// �q�b�g�O���[�v�̃��[�J�����[�g�V�O�l�`��
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> texture : register(t2, space1);
// �T���v���[
SamplerState smp : register(s0, space1);

// RayGeneration�V�F�[�_�[�̃��[�J�����[�g�V�O�l�`��
RWTexture2D<float4> aoOutput : register(u0);
RWTexture2D<float4> lightingOutput : register(u1);
RWTexture2D<float4> colorOutput : register(u2);
RWTexture2D<float4> giOutput : register(u3);

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

// �\�t�g�V���h�E�ˏo�֐�
float SoftShadow(Vertex vtx, float lightSize, float length)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // �����ւ̒��S�x�N�g��
    float3 pointLightPosition = gSceneParam.pointLight.lightPos;
    float3 lightDir = normalize(pointLightPosition - worldPosition);
    
    // ���C�g�x�N�g���Ɛ����ȃx�N�g�������߂�B
    float3 perpL = cross(lightDir, float3(0, 1, 0));
    if (all(perpL == 0.0f))
    {
        perpL.x = 1.0f;
    }
    
    // �����̒[�����߂�B
    float3 toLightEdge = (pointLightPosition + perpL * lightSize) - worldPosition;
    toLightEdge = normalize(toLightEdge);
    
    // �p�x�����߂�B
    float coneAngle = acos(dot(lightDir, toLightEdge)) * 2.0f;
    
    // �����̎�����߂�B
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    int randSeed = initRand(DispatchRaysIndex().x + (vtx.Position.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);
    
    float3 shadowRayDir = GetConeSample(randSeed, lightDir, coneAngle);
    return ShootShadowRay(worldPosition, shadowRayDir, length, gRtScene);
    
}

// �\�t�g�V���h�E�ˏo�֐�
float ShootDirShadow(Vertex vtx, float length)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // ���C�g�x�N�g���Ɛ����ȃx�N�g�������߂�B
    float3 perpL = cross(-gSceneParam.dirLight.lightDir, float3(0, 1, 0));
    if (all(perpL == 0.0f))
    {
        perpL.x = 1.0f;
    }
    
    // �����̒[�����߂�B
    float3 toLightEdge = ((vtx.Position + -gSceneParam.dirLight.lightDir * 1000.0f) + perpL * 10) - worldPosition;
    toLightEdge = normalize(toLightEdge);
    
    // �p�x�����߂�B
    float coneAngle = acos(dot(-gSceneParam.dirLight.lightDir, toLightEdge)) * 2.0f;
    
    // �����̎�����߂�B
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    int randSeed = initRand(DispatchRaysIndex().x + (vtx.Position.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);
    
    float3 shadowRayDir = GetConeSample(randSeed, -gSceneParam.dirLight.lightDir, coneAngle);
    return ShootShadowRayNoAH(worldPosition, shadowRayDir, length, gRtScene);
    
}

// GI
float3 ShootGIRay(Vertex vtx, float length)
{
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldNormal = mul(vtx.Normal, (float3x3) ObjectToWorld4x3());
    float3 worldRayDir = WorldRayDirection();
    float3 reflectDir = reflect(worldRayDir, worldNormal);
    
    // ���C�̃t���O��ݒ�B
    RAY_FLAG flag = RAY_FLAG_NONE;
    //flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�𖳎��B
    
    // ���C�̃}�X�N
    uint rayMask = 0xFF;

    // ���C�̃p�����[�^�[��ݒ�B
    RayDesc rayDesc;
    rayDesc.Origin = worldPos;
    rayDesc.Direction = reflectDir;
    rayDesc.TMin = 0.01;
    rayDesc.TMax = length;

    // �y�C���[�h���������B
    DenoisePayload reflectPayload;
    reflectPayload.color = float3(0, 0, 0);
    reflectPayload.aoLuminance = float3(0, 0, 0);
    reflectPayload.lightLuminance = float3(0, 0, 0);
    reflectPayload.giColor = float3(0, 0, 0);
    reflectPayload.recursive = 100000;
    
    // ���C�𔭎ˁB
    TraceRay(
        gRtScene,
        flag,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        reflectPayload);
    return reflectPayload.giColor;
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
    rayDesc.TMax = 10000;

    // �y�C���[�h�̐ݒ�
    DenoisePayload payload;
    payload.color = float3(0, 0, 0);
    payload.aoLuminance = float3(0, 0, 0);
    payload.lightLuminance = float3(0, 0, 0);
    payload.giColor = float3(0, 0, 0);
    payload.recursive = 0;

    // TransRay�ɕK�v�Ȑݒ���쐬
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
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

    // ���ʊi�[
    lightingOutput[launchIndex.xy] = float4(payload.aoLuminance, 1);
    aoOutput[launchIndex.xy] = float4(payload.lightLuminance, 1);
    colorOutput[launchIndex.xy] = float4(payload.color, 1);
    giOutput[launchIndex.xy] = float4(payload.giColor, 1);

}

// miss�V�F�[�_�[ ���C���q�b�g���Ȃ��������ɌĂ΂��V�F�[�_�[
[shader("miss")]
void mainMS(inout DenoisePayload payload)
{

    // �P�F��Ԃ��悤�ɂ���B
    payload.color = float3(0, 0, 0);
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
void mainCHS(inout DenoisePayload payload, MyAttribute attrib)
{
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();

    // �e�N�X�`���̐F��ۑ��B
    float3 texColor = texture.SampleLevel(smp, vtx.uv, 0.0f);
    
    // ���ˉ񐔂�100000�񂾂�����GI�p�̃��C�Ȃ̂Ńe�N�X�`���̐F��Ԃ��B
    if (payload.recursive == 100000)
    {
        // ���C�̒���
        float rayLength = length(WorldRayOrigin() - vtx.Position);
        
        // ���C�̒����̍ő�l
        const float MAX_RAY = 500.0f;
        
        // ���������߂�B
        float rate = rayLength / MAX_RAY;
        rate = 1.0f - saturate(rate);
        
        payload.giColor = texColor * rate;
        return;
    }
    
    // Instance����1��������e�N�X�`���̐F�����̂܂ܕԂ��B
    if (instanceID == 1)
    {
        payload.lightLuminance = float3(1, 1, 1);
        payload.color = texColor;
        payload.aoLuminance = float3(1, 1, 1);
        payload.giColor = float3(1, 1, 1);
        return;
    }
    
    // �|���S���̕`�悷��t���O�������Ă�����B
    if (gSceneParam.isMeshScene)
    {
        payload.lightLuminance = CalcBarycentrics(attrib.barys);
        return;
    }

    // �@���}�b�v�����݂��Ă�����@���}�b�v����@�����𒊏o�B
    //vtx.Normal = normalMap.SampleLevel(smp, vtx.uv, 0.0f);
    
    // �@����`�悷��t���O�������Ă�����B
    if (gSceneParam.isNormalScene)
    {
        payload.lightLuminance = vtx.Normal;
        return;
    }
    
    // �Ăяo���񐔂������𒴂��Ȃ��悤�ɂ���B
    ++payload.recursive;
    if (1 < payload.recursive)
    {
        return;
    }

    // �ʏ탉�C�e�B���O
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    
    // ���C�e�B���O�̌��ʂ̖��邳
    float pointLightVisibility = 0;
    float dirLightVisibility = 0;
    float aoLightVisibility = 0;
    
    // �|�C���g���C�g�̃��C�e�B���O���ʂ̐F
    float3 pointLightColor = float3(0, 0, 0);
    
    // ���s�����̃��C�e�B���O���ʂ̐F
    float3 dirLightColor = float3(0, 0, 0);
    
    // ���C�g�܂ł̋���
    float lightLength = length(gSceneParam.pointLight.lightPos - vtx.Position);
    
    // �_�����փV���h�E���C���΂��B
    if (lightLength < gSceneParam.pointLight.lightPower && gSceneParam.pointLight.isActive)
    {
        
        pointLightVisibility = SoftShadow(vtx, gSceneParam.pointLight.lightSize, length(gSceneParam.pointLight.lightPos - vtx.Position));
        
        // �e��������
        if (0 <= pointLightVisibility)
        {
            
            // ���邳������������B
            float rate = lightLength / gSceneParam.pointLight.lightPower;
            rate = pow(rate, 5);
            rate = 1.0f - rate;
                
            // �����o�[�g�̔��˗��Ɩ��邳��������B
            pointLightVisibility *= rate;
                
            pointLightColor += gSceneParam.pointLight.lightColor * pointLightVisibility;
                
            
        }
        
    }
    
    // ���s�����ɃV���h�E���C���΂��B
    if (gSceneParam.dirLight.isActive)
    {
        
        dirLightVisibility = ShootDirShadow(vtx, 3000.0f);
        
        dirLightColor = gSceneParam.dirLight.lightColor * dirLightVisibility;
        
    }
    
        
    // ��΂����C�̉�
    for (int index = 0; index < 1; ++index)
    {
        // �A���r�G���g�I�N�����[�W�������s��Ȃ��t���O�������Ă����珈�����΂��B
        if (gSceneParam.isNoAO)
        {
            break;
        }
            
       
        int seed = initRand(DispatchRaysIndex().x + (vtx.Position.x / 1000.0f) + index + DispatchRaysIndex().y * numPix.x, 100);
        float3 sampleDir = GetUniformHemisphereSample(seed, vtx.Normal);
        
        // �V���h�E���C���΂��B
        float aoLightVisibilityBuff = ShootAOShadowRay(vtx.Position, sampleDir, 100, gRtScene);
        
        // �B���x���� += �T���v�����O�����l * �R�T�C���� / �m�����x�֐�
        float NoL = saturate(dot(vtx.Normal, sampleDir));
        float pdf = 1.0 / (2.0 * PI);
        aoLightVisibility += aoLightVisibilityBuff * NoL / pdf;
        
            
    }
    // ���ς����B
    aoLightVisibility = (1.0f / PI) * (1.0f / float(1)) * aoLightVisibility;
        
    
    
    // �����ւ̃��C�e�B���O�����Z����B
    float lightVisibility = 0;
    lightVisibility += pointLightVisibility + dirLightVisibility;
    float aoVisibility = aoLightVisibility;
    
    // �ŏI���ʂ̐F��ۑ��B
    payload.color.xyz = texColor;
    payload.lightLuminance = lightVisibility + (pointLightColor + dirLightColor) / PI;
    payload.aoLuminance = aoVisibility;
    
    // GI�̐F���擾����B
    if (instanceID == 10 && !gSceneParam.isNoGI)
    {
        payload.giColor = ShootGIRay(vtx, 500) * 1.0f;
    }
    else
    {
        payload.giColor = float3(0, 0, 0);
    }
    
    // GI�݂̂�`�悷��t���O�������Ă�����GI�ȊO�̐F�𖳌�������B
    if (gSceneParam.isGIOnlyScene)
    {
        
        payload.lightLuminance = float3(1, 1, 1);
        payload.color = float3(0, 0, 0);
        payload.aoLuminance = float3(0, 0, 0);
        
    }
    
    // ���C�g�ɓ��������ʂ����\������t���O�������Ă�����B
    if (gSceneParam.isLightHitScene)
    {
        
        // ���ɂ������Ă�����B
        if (0.0f < pointLightVisibility + dirLightVisibility)
        {
            payload.color = float3(1, 1, 1);
        }
        else
        {
            payload.color = float3(0, 0, 0);
        }
        
        return;
        
    }

}

// closesthit�V�F�[�_�[ �V���h�E�p
[shader("closesthit")]
void shadowCHS(inout ShadowPayload payload, MyAttribute attrib)
{
}

// �A���t�@����AnyHitShader
[shader("anyhit")]
void mainAnyHit(inout DenoisePayload payload, MyAttribute attrib)
{
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    float4 diffuse = texture.SampleLevel(smp, vtx.uv, 0);
    if (diffuse.w < 0.5f)
    {
        IgnoreHit();

    }
    
    int instanceID = InstanceID();
    
    // �C���X�^���XID��1(���C�g)�Ȃ瓖���蔻������p����B
    if (instanceID == 1)
    {
        IgnoreHit();

    }
    
}
