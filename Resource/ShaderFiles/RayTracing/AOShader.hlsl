
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// �O���[�o�����[�g�V�O�l�`��
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<SceneCB> gSceneParam : register(b0);

// �q�b�g�O���[�v�̃��[�J�����[�g�V�O�l�`��
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> texture : register(t2, space1);
Texture2D<float4> normalMap : register(t3, space1);
// �T���v���[
SamplerState smp : register(s0, space1);

// RayGeneration�V�F�[�_�[�̃��[�J�����[�g�V�O�l�`��
RWTexture2D<float4> gOutput : register(u0);
RWTexture2D<float4> gOutputBuff : register(u1);

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

// ���˃��C�g���[�V���O
float3 Reflection(float3 vertexPosition, float3 vertexNormal, int recursive)
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

    Payload reflectPayload;
    reflectPayload.color = float3(0, 0, 0);
    reflectPayload.recursive = recursive;
    TraceRay(
        gRtScene,
        0,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        reflectPayload);
    return reflectPayload.color;
}

// ���܃��C�g���[�V���O
float3 Refraction(float3 vertexPosition, float3 vertexNormal, int recursive)
{
    float4x3 mtx = ObjectToWorld4x3();
    float3 worldPos = mul(float4(vertexPosition, 1), mtx);
    float3 worldNormal = mul(vertexNormal, (float3x3) mtx);
    float3 worldRayDir = normalize(WorldRayDirection());
    worldNormal = normalize(worldNormal);

    const float refractVal = 1.4;
    float nr = dot(worldNormal, worldRayDir);
    float3 refracted;
    if (nr < 0)
    {
        // �\��. ��C�� -> ���ܔ}��.
        float eta = 1.0 / refractVal;
        refracted = refract(worldRayDir, worldNormal, eta);
    }
    else
    {
        // ����. ���ܔ}�� -> ��C��.
        float eta = refractVal / 1.0;
        refracted = refract(worldRayDir, -worldNormal, eta);
    }

    if (length(refracted) < 0.01)
    {
        return Reflection(vertexPosition, vertexNormal, recursive);
    }
    else
    {
        uint rayMask = 0xFF;

        RayDesc rayDesc;
        rayDesc.Origin = worldPos;
        rayDesc.Direction = refracted;
        rayDesc.TMin = 0.001;
        rayDesc.TMax = 100000;

        Payload refractPayload;
        refractPayload.color = float3(0, 1, 0);
        refractPayload.recursive = recursive;
        TraceRay(
            gRtScene,
            0,
            rayMask,
            0, // ray index
            1, // MultiplierForGeometryContrib
            0, // miss index
            rayDesc,
            refractPayload);
        return refractPayload.color;
    }
}

// ���˃��C���ˏo
void ShootReflectionRay(inout Payload payload, float3 vertexPosition, float3 vertexNormal)
{
    
    float3 worldPos = mul(float4(vertexPosition, 1), ObjectToWorld4x3());
    float3 worldNormal = mul(vertexNormal, (float3x3) ObjectToWorld4x3());
    float3 worldRayDir = WorldRayDirection();
    float3 reflectDir = reflect(worldRayDir, worldNormal);
    
    uint rayMask = 0xFF;

    RayDesc rayDesc;
    rayDesc.Origin = worldPos;
    rayDesc.Direction = reflectDir;
    rayDesc.TMin = 0.1f;
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

// �V���h�E���C����
bool ShootShadowRay(float3 origin, float3 direction, float tMax)
{
    RayDesc rayDesc;
    rayDesc.Origin = origin;
    rayDesc.Direction = direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = tMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShader�X�L�b�v
    
    // ���C�g�͏��O�B
    uint rayMask = ~(0x08);

    TraceRay(
    gRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // MISS�V�F�[�_�[�̃C���f�b�N�X
    rayDesc,
    payload);

    return payload.isShadow;
}

// �V���h�E���C����
bool ShootAOShadowRay(float3 origin, float3 direction, float tMax)
{
    RayDesc rayDesc;
    rayDesc.Origin = origin;
    rayDesc.Direction = direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = tMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�X�L�b�v
    
    // ���C�g�͏��O�B
    uint rayMask = ~(0x08);

    TraceRay(
    gRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // MISS�V�F�[�_�[�̃C���f�b�N�X
    rayDesc,
    payload);

    return payload.isShadow;
}

// �\�t�g�V���h�E�ˏo�֐�
float SoftShadow(Vertex vtx)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // �����ւ̒��S�x�N�g��
    float3 pointLightPosition = gSceneParam.lightPos;
    float3 lightDir = normalize(pointLightPosition - worldPosition);
    
    // ���C�g�x�N�g���Ɛ����ȃx�N�g�������߂�B
    float3 perpL = cross(lightDir, float3(0, 1, 0));
    if (all(perpL == 0.0f))
    {
        perpL.x = 1.0f;
    }
    
    // �����̒[�����߂�B
    float3 toLightEdge = (pointLightPosition + perpL * gSceneParam.lightSize) - worldPosition;
    toLightEdge = normalize(toLightEdge);
    
    // �p�x�����߂�B
    float coneAngle = acos(dot(lightDir, toLightEdge)) * 2.0f;
    
    // �����̎�����߂�B
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    int randSeed = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
    
    float3 shadowRayDir = GetConeSample(randSeed, lightDir, coneAngle);
    
    
    
    //float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    //// �����ւ̒��S�x�N�g��
    //float3 pointLightPosition = gSceneParam.lightPos;
    //float3 lightDir = pointLightPosition - worldPosition;
    
    //// �����̎�����߂�B
    //uint2 pixldx = DispatchRaysIndex().xy;
    //uint2 numPix = DispatchRaysDimensions().xy;
    //float randSeed = frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453);
    
    //// ������������̔��a�����ɐL�т�x�N�g�������߂�B
    //float3 lightScaleVec = float3(randSeed * gSceneParam.lightSize, randSeed * gSceneParam.lightSize, randSeed * gSceneParam.lightSize);
    
    //// �����x�N�g���Ɣ��a�x�N�g���𑫂��Đ��K�������l���V���h�E���C�Ƃ���B
    //float3 shadowRayDir = normalize(lightDir + lightScaleVec);
    
    return ShootShadowRay(worldPosition, shadowRayDir, length(vtx.Position - gSceneParam.lightPos));
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
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�𖳎��B

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
    if (gSceneParam.counter == 0)
    {
        
        gOutputBuff[launchIndex.xy] = float4(col, 1);
        gOutput[launchIndex.xy] = float4(col, 1);

    }
    else if (gSceneParam.counter < 256)
    {
        gOutput[launchIndex.xy] = gOutputBuff[launchIndex.xy] / float(gSceneParam.counter);
        gOutputBuff[launchIndex.xy] += float4(col, 1);
    }
    else
    {
        gOutput[launchIndex.xy] = gOutputBuff[launchIndex.xy] / 256.0f;
    }
    
    // �f�o�b�O�p�Ńm�C�Y��ʂ��o���t���O�������Ă�����B
    if (gSceneParam.isNoiseScene)
    {
        gOutput[launchIndex.xy] = float4(col, 1);
    }
    
    gOutput[launchIndex.xy] = pow(gOutput[launchIndex.xy], 1 / 1.4);

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
    
    // �|���S���̕`�悷��t���O�������Ă�����B
    if (gSceneParam.isMeshScene)
    {
        payload.color = CalcBarycentrics(attrib.barys);
        return;
    }

    // �@���}�b�v�����݂��Ă�����@���}�b�v����@�����𒊏o�B
    vtx.Normal = normalMap.SampleLevel(smp, vtx.uv, 0.0f);
    
    // �@����`�悷��t���O�������Ă�����B
    if (gSceneParam.isNormalScene)
    {
        payload.color = vtx.Normal;
        return;
    }
    
    // �Ăяo���񐔂������𒴂��Ȃ��悤�ɂ���B
    if (checkRecursiveLimit(payload))
    {
        return; // �Ăяo���񐔂��z�����瑦���^�[��.
    }

    // ���S����
    if (instanceID == 0)
    {
        float3 reflectionColor = Reflection(vtx.Position, vtx.Normal, payload.recursive);
        payload.color = reflectionColor;
    }
    // ����
    if (instanceID == 1)
    {
        payload.color = Refraction(vtx.Position, vtx.Normal, payload.recursive);
    }
    // �ʏ탉�C�e�B���O
    else if (instanceID == 2)
    {
        // lambert ���C�e�B���O���s��.
        float3 lightdir = -normalize(gSceneParam.lightPos.xyz);

        float nl = saturate(dot(vtx.Normal, lightdir));
        
        // ���K�������o�[�g�H
        nl /= PI;

        float3 lightcolor = gSceneParam.lightColor.xyz;
        float3 ambientcolor = gSceneParam.ambientColor.xyz;
        float4 materialcolor = texture.SampleLevel(smp, vtx.uv, 0.0f);
        float3 color = 0;
        color += lightcolor * materialcolor.xyz * nl;
        color += ambientcolor * materialcolor.xyz;

        // ���C�e�B���O�̌��ʂ̐F��ۑ��B
        float3 resultColor = color;
        
        uint2 pixldx = DispatchRaysIndex().xy;
        uint2 numPix = DispatchRaysDimensions().xy;
        // �����_���ȃV�[�h���v�Z�B
        //uint randSeed = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
        
        // �B���x����
        float visibility = 0.0f;
        
        // �����փV���h�E���C���΂��B
        float smpleVisiblity = SoftShadow(vtx);
        
        // �B���x���� += �T���v�����O�����l * �R�T�C���� * �m�����x�֐�
        float nol = saturate(dot(vtx.Normal, normalize(gSceneParam.lightPos.xyz)));
        float pdf = 1.0 / (2.0 * 3.14f);
        float lightVisibility = 0;
        lightVisibility += smpleVisiblity;
            
        // ���C�e�B���O�̌��ʖ��邩�����珈�����΂��B
        if (lightVisibility != 1.0f)
        {
            
            // ��΂����C�̉�
            for (int index = 0; index < gSceneParam.aoSampleCount; ++index)
            {
                // �A���r�G���g�I�N�����[�W�������s��Ȃ��t���O�������Ă����珈�����΂��B
                if (gSceneParam.isNoAO)
                {
                    break;
                }
            
                // �@���𒆐S�Ƃ���������̃����_���ȃx�N�g���̃T���v�����O(�R�T�C���d�ݕ��z�t��)
                //float3 sampleDir = GetCosHemisphereSample(randSeed, vtx.Normal);
                
                // �����𐶐����ă��C���΂����������߂�B
                float randSeedX = (frac(sin(dot(vtx.Position.xy + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
                float randSeedY = (frac(sin(dot(vtx.Position.xz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(78.233, 12.9898)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
                float randSeedZ = (frac(sin(dot(vtx.Position.yz + pixldx * (index + 1.0f) + numPix * (index + 1.0f), float2(32.9898, 48.233)) + gSceneParam.seed) * 43758.5453 * (index + 1.0f)));
                randSeedX = randSeedX * 2.0f - 1.0f;
                randSeedY = randSeedY * 2.0f - 1.0f;
                randSeedZ = randSeedZ * 2.0f - 1.0f;
                float3 sampleDir = float3(randSeedX, randSeedY, randSeedZ);
            
                // �V���h�E���C���΂��B
                float smpleVisiblity = ShootAOShadowRay(vtx.Position, sampleDir, 300);
            
                // �B���x���� += �T���v�����O�����l * �R�T�C���� / �m�����x�֐�
                float nol = saturate(dot(vtx.Normal, sampleDir));
                float pdf = 1.0 / (2.0 * PI);
                visibility += smpleVisiblity * nol / pdf;
               
            }
            // ���ς����B
            visibility = (1.0f / PI) * (1.0f / float(gSceneParam.aoSampleCount)) * visibility;
            
        }
        
        
        // �����ւ̃��C�e�B���O�����Z����B
        visibility += lightVisibility;
        
        // �B���x���������E�𒴂��Ȃ��悤�ɂ���B
        visibility = saturate(visibility);
        
        // �m�C�Y�݂̂�`�悷��t���O�������Ă�����B
        if (gSceneParam.isNoiseOnlyScene)
        {
            payload.color = float3(visibility, visibility, visibility);
            return;

        }
        
        // �e�N�X�`���ƃ��C�e�B���O�̐F�ɉB������������B
        resultColor *= visibility;
        
        // �ŏI���ʂ̐F��ۑ��B
        payload.color.xyz += resultColor;
        
        // ���C�g�ɓ��������ʂ����\������t���O�������Ă�����B
        if (gSceneParam.isLightHitScene)
        {
            
            // ���ɂ������Ă�����B
            if (0.0f < lightVisibility)
            {
                payload.color = float3(1, 1, 1);
            }
            else
            {
                payload.color = float3(0, 0, 0);
            }
            
            return;
            
        }
        
        //ShootReflectionRay(payload, vtx.Position, vtx.Normal);

    }
    else if (instanceID == 3)
    {
        payload.color = float3(1, 1, 1);
    }

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
    float4 diffuse = texture.SampleLevel(smp, vtx.uv, 0);
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
