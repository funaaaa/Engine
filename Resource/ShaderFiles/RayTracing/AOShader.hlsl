
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
RWTexture2D<float4> aoBakeTex : register(u0, space1);
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
    int randSeed = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
    
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
    float3 toLightEdge = ((vtx.Position + -gSceneParam.dirLight.lightDir * 1000.0f) + perpL * 20) - worldPosition;
    toLightEdge = normalize(toLightEdge);
    
    // �p�x�����߂�B
    float coneAngle = acos(dot(-gSceneParam.dirLight.lightDir, toLightEdge)) * 2.0f;
    
    // �����̎�����߂�B
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    int randSeed = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
    
    float3 shadowRayDir = GetConeSample(randSeed, -gSceneParam.dirLight.lightDir, coneAngle);
    return ShootShadowRayNoAH(worldPosition, shadowRayDir, length, gRtScene);
    
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
    if (gSceneParam.counter == 0)
    {
        
        gOutputBuff[launchIndex.xy] = float4(col, 1);
        gOutput[launchIndex.xy] = float4(col, 1);

    }
    else if (gSceneParam.counter < 512)
    {
        gOutput[launchIndex.xy] = gOutputBuff[launchIndex.xy] / float(gSceneParam.counter);
        gOutputBuff[launchIndex.xy] += float4(col, 1);
    }
    else
    {
        gOutput[launchIndex.xy] = gOutputBuff[launchIndex.xy] / 512.0f;
    }
    
    // �f�o�b�O�p�Ńm�C�Y��ʂ��o���t���O�������Ă�����B
    if (gSceneParam.isNoiseScene)
    {
        gOutput[launchIndex.xy] = float4(col, 1);
    }
    
    gOutput[launchIndex.xy] = pow(gOutput[launchIndex.xy], 1.0f / 1.5f);

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
    //vtx.Normal = normalMap.SampleLevel(smp, vtx.uv, 0.0f);
    
    // �@����`�悷��t���O�������Ă�����B
    if (gSceneParam.isNormalScene)
    {
        payload.color = vtx.Normal;
        return;
    }
    
    // �Ăяo���񐔂������𒴂��Ȃ��悤�ɂ���B
    if (checkRecursiveLimit(payload, 1))
    {
        return; // �Ăяo���񐔂��z�����瑦���^�[��.
    }

    // �ʏ탉�C�e�B���O
    if (instanceID == 2)
    {

        // ���C�e�B���O�̌��ʂ̐F��ۑ��B
        float3 texColor = texture.SampleLevel(smp, vtx.uv, 0.0f);
        
        uint2 pixldx = DispatchRaysIndex().xy;
        uint2 numPix = DispatchRaysDimensions().xy;
        
        // �B���x����
        float visibility = 0.0f;
        
        // ���C�e�B���O�̌��ʂ̖��邳
        float lightVisibility = 0;
        
        // �|�C���g���C�g�̃��C�e�B���O���ʂ̐F
        float3 pointLightColor = float3(0, 0, 0);
        
        // ���s�����̃��C�e�B���O���ʂ̐F
        float3 dirLightColor = float3(0, 0, 0);
        
        // �_�����ƕ��s�����̂ǂ���Ƀ��C���΂����𒲂ׂ�B
        int lightingRandom = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
        lightingRandom = lightingRandom % 3;
        
        // �ړ_�Ɠ_�����̋���
        float lightDistance = length(vtx.Position - gSceneParam.pointLight.lightPos);
        
        // �_�������I�΂�Ă��āA���C�g�Ƃ̋������K��l��藣��Ă����瑼��2��I�ԁB
        if (lightingRandom == 0)
        {
            if (gSceneParam.pointLight.lightPower < lightDistance)
            {
                lightingRandom = (frac(sin(dot(vtx.Position.xy + pixldx + numPix, float2(12.9898 + numPix.x, 78.233)) + gSceneParam.seed) * 43758.5453)) * 100000;
                lightingRandom = lightingRandom % 2 + 1;
            }

        }
        
        // �_�����ȊO���I�΂�Ă��āA�_�������K��l���߂�������_������I�ԁB
        if (lightingRandom != 0)
        {
            if (lightDistance < gSceneParam.pointLight.lightPower)
            {
                lightingRandom = 0;
            }
        }
        
        // �ǂ̃��C�e�B���O�����邩�ɂ���ď�����ς���B
        switch (lightingRandom)
        {
            
            case 0:
                {
            
                    // ���C�g�܂ł̋���
                    float lightLength = length(gSceneParam.pointLight.lightPos - vtx.Position);
        
                    // �_�����փV���h�E���C���΂��B
                    if (lightLength < gSceneParam.pointLight.lightPower && gSceneParam.pointLight.isActive)
                    {
            
                        lightVisibility = SoftShadow(vtx, gSceneParam.pointLight.lightSize, length(gSceneParam.pointLight.lightPos - vtx.Position));
            
                        // �e��������
                        if (0 <= lightVisibility)
                        {
                
                           // ���邳������������B
                            float rate = lightLength / gSceneParam.pointLight.lightPower;
                            rate = pow(rate, 5);
                            rate = 1.0f - rate;
                    
                            // �����o�[�g�̔��˗��Ɩ��邳��������B
                            lightVisibility *= rate;
                    
                            pointLightColor += gSceneParam.pointLight.lightColor * lightVisibility;
                    
                
                        }
            
                    }
                }
            
                break;
            
            
            case 1:
            {
            
                    float dirLightVisibility = 0;
        
                    // ���s�����ɃV���h�E���C���΂��B
                    if (gSceneParam.dirLight.isActive)
                    {
            
                        dirLightVisibility = ShootDirShadow(vtx, 3000.0f);
                        lightVisibility += dirLightVisibility;
            
                        dirLightColor = gSceneParam.dirLight.lightColor * dirLightVisibility;
            
                    }
                }
            
                break;
            
            case 2:
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
                        float smpleVisiblity = ShootAOShadowRay(vtx.Position, sampleDir, 500, gRtScene);
            
                    // �B���x���� += �T���v�����O�����l * �R�T�C���� / �m�����x�֐�
                        float nol = saturate(dot(vtx.Normal, sampleDir));
                        float pdf = 1.0 / (2.0 * PI);
                        visibility += smpleVisiblity * nol / pdf;
                
                    }
                // ���ς����B
                    visibility = (1.0f / PI) * (1.0f / float(gSceneParam.aoSampleCount)) * visibility;
            
                }
            
                break;
            
        }
        
        // �����ւ̃��C�e�B���O�����Z����B
        visibility += lightVisibility;
        
        // �B���x���������E�𒴂��Ȃ��悤�ɂ���B
        //visibility = saturate(visibility);
        
        // �m�C�Y�݂̂�`�悷��t���O�������Ă�����B
        if (gSceneParam.isNoiseOnlyScene)
        {
            payload.color = float3(visibility, visibility, visibility);
            return;

        }
        
        // �e�N�X�`���ƃ��C�e�B���O�̐F�ɉB������������B
        texColor *= visibility;
        
        // �ŏI���ʂ̐F��ۑ��B
        payload.color.xyz += texColor + (pointLightColor + dirLightColor) / PI;
        
        payload.color.xyz = saturate(payload.color.xyz);
        
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
