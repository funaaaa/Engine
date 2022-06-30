
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// �O���[�o�����[�g�V�O�l�`��
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<ConstBufferData> gSceneParam : register(b0);

// �q�b�g�O���[�v�̃��[�J�����[�g�V�O�l�`��
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
StructuredBuffer<Material> material : register(t2, space1);
Texture2D<float4> texture : register(t3, space1);
Texture2D<float4> normalTexture : register(t4, space1);
// �T���v���[
SamplerState smp : register(s0, space1);

// RayGeneration�V�F�[�_�[�̃��[�J�����[�g�V�O�l�`��
RWTexture2D<float4> aoOutput : register(u0);
RWTexture2D<float4> lightingOutput : register(u1);
RWTexture2D<float4> colorOutput : register(u2);
RWTexture2D<float4> giOutput : register(u3);
RWTexture2D<float4> denoiseMaskoutput : register(u4);

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

// ��C�U��
float3 AtmosphericScattering(float3 pos, inout float3 mieColor, inout bool isUnderGround)
{
    
    // �n�����ȉ��͐^�����ɂ���B
    if (pos.y < -10)
    {
        isUnderGround = true;
        return float3(0.1f, 0.1f, 0.1f);
    }
    
    // ���C���[�U���萔
    float kr = gSceneParam.as.kr;
    // �~�[�U���萔
    float km = gSceneParam.as.km;
 
    // ��C���̐������T���v�����O���鐔�B
    float fSamples = gSceneParam.as.samples;
 
    // ��̐F �F�I�ɂ͔��߂̒��F
    float3 three_primary_colors = float3(0.68f, 0.55f, 0.44f);
    // ���̔g���H
    float3 v3InvWaveLength = 1.0f / pow(three_primary_colors, 4.0f);
 
    // ��C���̈�ԏ�̍����B
    float fOuterRadius = gSceneParam.as.outerRadius;
    // �n���S�̂̒n��̍����B
    float fInnerRadius = gSceneParam.as.innerRadius;
 
    // ���z���̋����H
    float fESun = gSceneParam.as.eSun;
    // ���z���̋����Ƀ��C���[�U���萔�������ă��C���[�U���̋��������߂Ă���B
    float fKrESun = kr * fESun;
    // ���z���̋����Ƀ~�[�U���萔�������ă��C���[�U���̋��������߂Ă���B
    float fKmESun = km * fESun;

    // ���C���[�U���萔�ɉ~�����������Ă���̂����A����Ȃ�0�ɋ߂��l�B
    float fKr4PI = kr * 4.0f * PI;
    // �~�[�U���萔�ɉ~�����������Ă���̂����A�~�[�U���萔��0�Ȃ̂ł���̒l��0�B
    float fKm4PI = km * 4.0f * PI;
 
    // �n���S�̂ł̑�C�̊����B
    float fScale = 1.0f / (fOuterRadius - fInnerRadius);
    // ���ϑ�C���x�����߂鍂���B
    float fScaleDepth = gSceneParam.as.aveHeight;
    // �n���S�̂ł̑�C�̊����𕽋ϑ�C���x�Ŋ������l�B
    float fScaleOverScaleDepth = fScale / fScaleDepth;
 
    // �U���萔�����߂�ۂɎg�p����l�B
    float g = gSceneParam.as.g;
    // �U���萔�����߂�ۂɎg�p����l���悵�����́B�Ȃ��B
    float g2 = g * g;
    
    // ���������V���̃��[���h���W
    float3 worldPos = normalize(pos) * fOuterRadius;
    worldPos = IntersectionPos(normalize(worldPos), float3(0.0, fInnerRadius, 0.0), fOuterRadius);
    
    // �J�������W ���v�Z�����ƒ��S�Œ�ɂȂ��Ă��܂��Ă������B
    float3 v3CameraPos = float3(0.0, fInnerRadius + 1.0f, 0.0f);
    
    // �f�B���N�V���i�����C�g�̏ꏊ�����߂�B
    float3 dirLightPos = -gSceneParam.light.dirLight.lightDir * 1000000.0f;
    
    // �f�B���N�V���i�����C�g�ւ̕��������߂�B
    float3 v3LightDir = normalize(dirLightPos - worldPos);
 
    // �V���㒸�_����J�����܂ł̃x�N�g��(������C���ɓ˓������_����J�����܂ł̌��̃x�N�g��)
    float3 v3Ray = worldPos - v3CameraPos;
    
    // ��C�ɓ˓����Ă���̓_�ƃJ�����܂ł̋����B
    float fFar = length(v3Ray);
    
    // ���K�����ꂽ�g�U�������������B
    v3Ray /= fFar;
 
    // �T���v�����O����n�_���W ��������A�̒��_
    float3 v3Start = v3CameraPos;
    // �T���v���ł̓J�����̈ʒu��(0,Radius,0)�Ȃ̂ŃJ�����̍����B�ǂ̈ʒu�Ɉړ����Ă��n�����_�Ō���Ό��_(�n���̒��S)����̍����B
    float fCameraHeight = length(v3CameraPos);
    // �n�ォ��̖@��(?)�Ɗg�U��������Ă����p�x�̓��ςɂ���ċ��߂�ꂽ�p�x���J�����̍����Ŋ���B
    float fStartAngle = dot(v3Ray, v3Start) / fCameraHeight;
    // �J�n�n�_�̍����ɕ��ϑ�C���x���������l�̎w�������߂�H
    float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    // �J�n�n�_�̂Ȃɂ��̊p�x�̃I�t�Z�b�g�B
    float fStartOffset = fStartDepth * scale(fStartAngle);
    
    // �T���v���|�C���g�Ԃ̒����B
    float fSampleLength = fFar / fSamples;
    // �T���v���|�C���g�Ԃ̒����ɒn���̑�C�̊�����������B���́B
    float fScaledLength = fSampleLength * fScale;
    // �g�U�������������ɃT���v���̒����������邱�ƂŃT���v���|�C���g�Ԃ̃��C���x�N�g�������߂�B
    float3 v3SampleRay = v3Ray * fSampleLength;
    // �ŏ��̃T���v���|�C���g�����߂�B0.5�������Ă�̂͏������������߁H
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;
 
    // �F���
    float3 v3FrontColor = 0.0f;
    for (int n = 0; n < int(fSamples); n++)
    {
        // �T���v���|�C���g�̍����B�ǂ���ɂ��挴�_�͒n���̒��S�Ȃ̂ŁA���̒l�����݈ʒu�̍����ɂȂ�B
        float fHeight = length(v3SamplePoint);
        // �n�ォ��T���v���|�C���g�̍����̍��ɕ��ϑ�C���x�����������́B  ���x�ɉ����đ�C���x���w���I�ɏ������Ȃ��Ă����̂�\�����Ă���H
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        // �n�ォ�猩���T���v���|�C���g�̖@���ƃf�B���N�V���i�����C�g�̕����̊p�x�����߂āA�T���v���|�C���g�̍����Ŋ���B
        float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight; // �����̒l��-1�ɂȂ遨Scale���̌v�Z��exp�̈�����43�ɂȂ�A�ƂĂ��Ȃ��ł����l������B �� -�ɂȂ�Ȃ��悤�ɂ���H
        // �n�ォ�猩���T���v���|�C���g�̖@���ƎU���������ł��Ă������̊p�x�����߂āA�T���v���|�C���g�̍����Ŋ���B
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
        // �U�����H
        float fScatter = (fStartOffset + fDepth * (scale(fLightAngle * 1) - scale(fCameraAngle * 1)));
        
        // �F���Ƃ̌������H
        float3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI));
        // �T���v���|�C���g�̈ʒu���l�����ĎU�������F�����߂�B
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        // �T���v���|�C���g���ړ�������B
        v3SamplePoint += v3SampleRay;
        
    }
 
    // ���C���[�U���Ɏg�p����F���
    float3 c0 = v3FrontColor * (v3InvWaveLength * fKrESun);
    // �~�[�U���Ɏg�p����F���
    float3 c1 = v3FrontColor * fKmESun;
    // �J�������W����V���̍��W�ւ̃x�N�g���B
    float3 v3Direction = v3CameraPos - worldPos;
 
    //float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos2 = fcos * fcos;
 
    // ���C���[�U���̖��邳�B
    float rayleighPhase = 0.75f * (1.0f + fcos2);
    // �~�[�U���̖��邳�B
    float miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fcos2) / pow(1.0f + g2 - 2.0f * g * fcos, 1.5f);
    
    // �~�[�U���̐F��ۑ��B
    mieColor = c0 * rayleighPhase;
    
    // ��_�܂ł̃x�N�g���Ƒ��z�܂ł̃x�N�g�����߂������甒�F�ɕ`�悷��B
    if (0.999f < dot(normalize(dirLightPos - v3CameraPos), normalize(worldPos - v3CameraPos)))
    {
        return float3(1, 1, 1);
    }
 
    // �ŏI���ʂ̐F
    float3 col = 1.0f;
    col.rgb = rayleighPhase * c0 + miePhase * c1;
    return col;
    
}

// �\�t�g�V���h�E�ˏo�֐�
float SoftShadow(Vertex vtx, float lightSize, float length, int lightIndex)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // �����ւ̒��S�x�N�g��
    float3 pointLightPosition = gSceneParam.light.pointLight[lightIndex].lightPos;
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

// ���z���̉e�`�F�b�N�p���C�̏����֐� �߂�l�͑��z���̐F
bool ShootDirShadow(Vertex vtx, float length)
{
    float3 worldPosition = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    
    // ���C�g�x�N�g���Ɛ����ȃx�N�g�������߂�B
    float3 perpL = cross(-gSceneParam.light.dirLight.lightDir, float3(0, 1, 0));
    if (all(perpL == 0.0f))
    {
        perpL.x = 1.0f;
    }
    
    // ���s�����̍��W�����ŋ��߂�B
    float3 dirLightPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;
    
    // ���s�����܂ł̃x�N�g���B
    float3 dirLightVec = dirLightPos - worldPosition;
    dirLightVec = normalize(dirLightVec);
    
    //// �����̒[�����߂�B
    //float3 toLightEdge = ((worldPosition + dirLightPos) + perpL * 10000) - worldPosition;
    //toLightEdge = normalize(toLightEdge);
    
    //// �p�x�����߂�B
    //float coneAngle = acos(dot(dirLightVec, toLightEdge)) * 2.0f;
    
    //// �����̎�����߂�B
    //uint2 pixldx = DispatchRaysIndex().xy;
    //uint2 numPix = DispatchRaysDimensions().xy;
    //int randSeed = initRand(DispatchRaysIndex().x + (worldPosition.x * gSceneParam.light.dirLight.lightDir.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);
    
    //// ���C�����x�N�g��
    //float3 shadowRayDir = GetConeSample(randSeed, dirLightVec, coneAngle);
    float3 shadowRayDir = dirLightVec;
    
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
    reflectPayload.recursive = 0;
    reflectPayload.rayID = CHS_IDENTIFICATION_RAYID_GI;
    
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

    matrix mtxViewInv = gSceneParam.camera.mtxViewInv;
    matrix mtxProjInv = gSceneParam.camera.mtxProjInv;

    // ���C�̐ݒ�
    RayDesc rayDesc;
    rayDesc.Origin = mul(mtxViewInv, float4(0, 0, 0, 1)).xyz;

    float4 target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
    float3 dir = mul(mtxViewInv, float4(target.xyz, 0)).xyz;

    rayDesc.Direction = normalize(dir);
    rayDesc.TMin = 0;
    rayDesc.TMax = 300000;

    // �y�C���[�h�̐ݒ�
    DenoisePayload payload;
    payload.color = float3(0, 0, 0);
    payload.aoLuminance = float3(0, 0, 0);
    payload.lightLuminance = float3(0, 0, 0);
    payload.giColor = float3(0, 0, 0);
    payload.recursive = 0;
    payload.rayID = CHS_IDENTIFICATION_RAYID_DEF;

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
    
    // ���ʏ������ݗpUAV����U�������B
    lightingOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    aoOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    colorOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    giOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    denoiseMaskoutput[launchIndex.xy] = float4(1, 1, 1, 1);

    // ���ʊi�[
    lightingOutput[launchIndex.xy] = float4(payload.lightLuminance, 1);
    aoOutput[launchIndex.xy] = float4(payload.aoLuminance, 1);
    colorOutput[launchIndex.xy] = float4(payload.color, 1);
    giOutput[launchIndex.xy] = float4(payload.giColor, 1);
    denoiseMaskoutput[launchIndex.xy] = float4(payload.denoiseMask, 1);

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


    // �Ăяo���񐔂������𒴂��Ȃ��悤�ɂ���B
    ++payload.recursive;
    if (2 < payload.recursive)
    {
        return;
    }
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldNormal = normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3()));

    // �e�N�X�`���̐F��ۑ��B
    float3 texColor = (float3) texture.SampleLevel(smp, vtx.uv, 0.0f);
    
    // �@���}�b�v�̐F���擾�B
    float3 normalMapColor = (float3) normalTexture.SampleLevel(smp, vtx.uv, 0.0f);
    
    // �}�X�N�p�̃e�N�X�`����ۑ��B
    payload.denoiseMask = normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3())) * (InstanceIndex() / 10.0f);
    
    // �@���}�b�v�̐F�ƃe�N�X�`���̐F��������������A���̖@���}�b�v�̓������̌��Ԃ𖄂߂邽�߂ɂ����ꂽ�e�N�X�`���Ȃ̂ŁA�@���}�b�v�Ƃ��ēK�������Ȃ��B
    if (!(texColor.x == normalMapColor.x && texColor.y == normalMapColor.y && texColor.z == normalMapColor.z))
    {
        worldNormal = normalize(mul(normalMapColor, (float3x3) ObjectToWorld4x3()));
    }
    
    // ���C��ID��CHS_IDENTIFICATION_GI��������GI�p�̃��C�Ȃ̂Ńe�N�X�`���̐F��Ԃ��B
    if (payload.rayID == CHS_IDENTIFICATION_RAYID_GI)
    {
        // ���C�̒���
        float rayLength = length(WorldRayOrigin() - worldPos);
        
        // ���C�̒����̍ő�l
        const float MAX_RAY = 500.0f;
        
        // ���������߂�B
        float rate = rayLength / MAX_RAY;
        rate = 1.0f - saturate(rate);
        
        payload.giColor += texColor * rate;
        
        // ���̃I�u�W�F�N�g���S���˂������獕��Ԃ��B
        if (instanceID == CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION)
        {
            payload.giColor = float3(0, 0, 0);
        }
        
        return;
    }

    
    // Instance����CHS_IDENTIFICATION_AS���������C�U�����v�Z�B
    if (instanceID == CHS_IDENTIFICATION_INSTNACE_AS)
    {
        float3 mieColor = float3(1, 1, 1);
        bool isUnderGround = false;
        
        payload.lightLuminance += float3(1, 1, 1);
        payload.color += AtmosphericScattering(worldPos, mieColor, isUnderGround);
        payload.aoLuminance += float3(1, 1, 1);
        payload.giColor += float3(0, 0, 0);
        
        // �}�X�N�p�̃e�N�X�`���𔒂�����B
        payload.denoiseMask = float3(1, 1, 1);
        
        // �V���̖��邳�����߂�B
        float t = dot(payload.color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
        
        // �n������艺����Ȃ������琯��`�悷��B
        if (!isUnderGround)
        {
            t = (1.0f - t);
            t = t == 0.0f ? 0.0f : pow(2.0f, 10.0f * t - 10.0f);
            t = t == 0.0f ? 0.0f : pow(2.0f, 10.0f * t - 10.0f);
            payload.color += texColor * t;
            payload.color = saturate(payload.color);
            
        }
        
        return;
    }
    
    // Instance����CHS_IDENTIFICATION_TEXCOLOR��������e�N�X�`���̐F�����̂܂ܕԂ��B
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_TEXCOLOR || instanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT)
    {
        payload.lightLuminance += float3(1, 1, 1);
        payload.color += texColor;
        payload.aoLuminance += float3(1, 1, 1);
        payload.giColor += float3(0, 0, 0);
        
        return;
    }
    
    // InstanceID�����˃��C�������犮�S���˂�����B
    if (instanceID == CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION)
    {
        
        // ���S���˃��C���΂��B
        ShootRay(CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION, worldPos, reflect(WorldRayDirection(), worldNormal), payload, gRtScene);
        
        // �f�o�b�O�p�Ŏ኱�����ۂ�����B
        payload.color += 0.1f;
        
        return;
        
    }
    
    // �|���S���̕`�悷��t���O�������Ă�����B
    if (gSceneParam.debug.isMeshScene)
    {
        payload.lightLuminance = CalcBarycentrics(attrib.barys);
        return;
    }
    
    // �@����`�悷��t���O�������Ă�����B
    if (gSceneParam.debug.isNormalScene)
    {
        payload.lightLuminance = worldNormal;
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
    
    for (int index = 0; index < POINT_LIGHT_COUNT; ++index)
    {
        
        // ���C�g���L��������Ă��Ȃ������珈�����΂��B
        if (!gSceneParam.light.pointLight[index].isActive)
        {
            continue;
        }
        
        // ���C�g�܂ł̋���
        float lightLength = length(gSceneParam.light.pointLight[index].lightPos - worldPos);
    
        // �_�����փV���h�E���C���΂��B
        if (lightLength < gSceneParam.light.pointLight[index].lightPower && gSceneParam.light.pointLight[index].isActive)
        {
            
            // �e���o���Ȃ��t���O�������Ă�����e���΂��Ȃ��B
            float pointLightVisibilityBuff = 0;
            if (!gSceneParam.light.pointLight[index].isShadow)
            {
                pointLightVisibilityBuff = SoftShadow(vtx, gSceneParam.light.pointLight[index].lightSize, length(gSceneParam.light.pointLight[index].lightPos - worldPos), index);
            }
            else
            {
                pointLightVisibilityBuff = 1;
            }
        
            // �e��������
            if (0 <= pointLightVisibilityBuff)
            {
                
                float3 pointLightDir = worldPos - gSceneParam.light.pointLight[index].lightPos;
                pointLightDir = normalize(pointLightDir);
            
                // ���邳������������B
                float rate = lightLength / gSceneParam.light.pointLight[index].lightPower;
                rate = pow(rate, 5);
                rate = 1.0f - rate;
                
                // �f�B�t���[�Y���v�Z����B
                float mDiffuse = material[0].diffuse.x;
                float3 diffuse = dot(-pointLightDir, worldNormal) * mDiffuse;
			    // ����x
                const float shininess = 4.0f;
		    	// ���_���王�_�ւ̕����x�N�g��
                float3 eyedir = normalize(WorldRayOrigin() - worldPos);
		    	// ���ˌ��x�N�g��
                float3 reflect = normalize(pointLightDir + 2.0f * dot(-pointLightDir, worldNormal) * worldNormal);
                // ���ʔ��ˌ�
                float mSpecular = material[0].specular.x;
                float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
                
                // �����o�[�g�̔��˗��Ɩ��邳��������B
                pointLightVisibilityBuff = pointLightVisibilityBuff * ((diffuse.x + specular.x) * rate);
                
                pointLightColor += gSceneParam.light.pointLight[index].lightColor * pointLightVisibilityBuff;
            
                pointLightVisibility += pointLightVisibilityBuff;
                
            
            }
        
        }
    }
    
    
    
    pointLightColor = saturate(pointLightColor);
    pointLightVisibility = saturate(pointLightVisibility);
            
    // ���z�̈ʒu
    float3 sunPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;
    float3 sunDir = normalize(sunPos - worldPos);
    
    // ���s�����ɃV���h�E���C���΂��B
    if (gSceneParam.light.dirLight.isActive/* && gSceneParam.light.dirLight.lightDir.y < 0.2f*/)
    {
        
        // ���s�����ł̉e�����擾�B
        dirLightVisibility = ShootDirShadow(vtx, 10000.0f);
        
        // �e����Ȃ��ꍇ�A�V����ł̃��C�e�B���O�̐F���擾����B
        if (0 < dirLightVisibility && gSceneParam.light.dirLight.lightDir.y < 0.2f)
        {
        
            // �e�̐F���v�Z�B
            //dirLightColor = dirLightVisibility * gSceneParam.light.dirLight.lightColor;
            dirLightColor = float3(0, 0, 0);
            
            // �f�B�t���[�Y���v�Z����B
            float mDiffuse = material[0].diffuse.x;
            float diffuse = dot(sunDir, worldNormal) * mDiffuse;
            
			// ����x
            const float shininess = 4.0f;
			// ���_���王�_�ւ̕����x�N�g��
            float3 eyedir = normalize(WorldRayOrigin() - worldPos);
			// ���ˌ��x�N�g��
            float3 reflect = normalize(-sunDir + 2.0f * dot(sunDir, worldNormal) * worldNormal);
            // ���ʔ��ˌ�
            float mSpecular = material[0].specular.x;
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
            
            // �B���x���X�V�B
            dirLightVisibility = diffuse + specular;
            
            const float SKYDOME_RADIUS = 15000.0f;
            const float SAMPLING_POS_Y = 0.0f;
            
            // �T���v�����O�������V����ł̈ʒu�܂ł̃x�N�g���B
            //float3 samplingVec = normalize(float3(1.0f, 0.1f, 0.1f)) * SKYDOME_RADIUS;
            float3 samplingVec = normalize(-gSceneParam.light.dirLight.lightDir * float3(1.0f, 0.0f, 1.0f)) * SKYDOME_RADIUS;
            samplingVec.y = 0.1f;
            
            // Y���̃T���v�����O�������ʒu��ݒ肷��B
            samplingVec.y = SAMPLING_POS_Y;
            
            // ���K������B
            samplingVec = normalize(samplingVec);
            
            // �V����̃T���v�����O�������ʒu�̍��W�����߂�B
            float3 samplingPos;
            samplingPos = samplingVec * SKYDOME_RADIUS;
            
            // ��C�U���̐F���擾�B
            float3 mieColor = float3(1, 1, 1);
            bool isUnderGround = false;
            float3 skydomeColor = AtmosphericScattering(samplingPos, mieColor, isUnderGround);
            
            // �f�B���N�V���i�����C�g�̐F��ݒ�B �[�Ă��̂Ƃ��ɐԂ��ۂ����邽�߂ɐԂ������ȊO����߂�B
            dirLightColor += mieColor * float3(1.0f, 0.5f, 0.5f);
            
            dirLightColor = normalize(dirLightColor);
            

            
        }
        else
        {
            dirLightColor = float3(0, 0, 0);
            dirLightVisibility = 0;
        }
        
    }
    
        
    // ��΂����C�̉�
    for (int aoindex = 0; aoindex < 1; ++aoindex)
    {
        // �A���r�G���g�I�N�����[�W�������s��Ȃ��t���O�������Ă����珈�����΂��B
        if (gSceneParam.debug.isNoAO)
        {
            break;
        }
       
        int seed = initRand(DispatchRaysIndex().x + (worldPos.x / 1000.0f) + aoindex + DispatchRaysIndex().y * numPix.x, 100);
        float3 sampleDir = GetUniformHemisphereSample(seed, worldNormal);
        
        // �V���h�E���C���΂��B
        float aoLightVisibilityBuff = ShootAOShadowRay(worldPos, sampleDir, 10, gRtScene);
        
        // �B���x���� += �T���v�����O�����l * �R�T�C���� / �m�����x�֐�
        float NoL = saturate(dot(worldNormal, sampleDir));
        float pdf = 1.0 / (2.0 * PI);
        aoLightVisibility += aoLightVisibilityBuff;
        
            
    }
    // ���ς����B
    //aoLightVisibility = (1.0f / PI) * (1.0f / float(gSceneParam.debug.aoSampleCount)) * aoLightVisibility;
        
    
    
    // �����ւ̃��C�e�B���O�����Z����B
    float lightVisibility = 0;
    lightVisibility += pointLightVisibility + dirLightVisibility;
    float aoVisibility = aoLightVisibility;
    
    // �ŏI���ʂ̐F��ۑ��B
    payload.color.xyz += texColor;
    payload.lightLuminance += lightVisibility + (pointLightColor + dirLightColor) / PI;
    payload.aoLuminance += aoVisibility;
    
    // GI�̐F���擾����B
    if (instanceID == 10 && !gSceneParam.debug.isNoGI)
    {
        payload.giColor += ShootGIRay(vtx, 500) * 1.0f;
        payload.giColor = saturate(payload.giColor);
    }
    else
    {
        payload.giColor += float3(0, 0, 0);
    }
    
    // GI�݂̂�`�悷��t���O�������Ă�����GI�ȊO�̐F�𖳌�������B
    if (gSceneParam.debug.isGIOnlyScene)
    {
        
        payload.lightLuminance = float3(1, 1, 1);
        payload.color = float3(0, 0, 0);
        payload.aoLuminance = float3(0, 0, 0);
        
    }
    
    // ���C�g�ɓ��������ʂ����\������t���O�������Ă�����B
    if (gSceneParam.debug.isLightHitScene)
    {
        
        // ���ɂ������Ă�����B
        if (0.0f < lightVisibility)
        {
            payload.color = float3(1, 1, 1);
            payload.lightLuminance = float3(1, 1, 1);
        }
        else
        {
            payload.color = float3(0, 0, 0);
            payload.lightLuminance = float3(0, 0, 0);
        }
        
        return;
        
    }
    
    
    // InstanceID�����˃��C�������甽�˂�����B
    if (instanceID == CHS_IDENTIFICATION_ISNTANCE_REFLECTION)
    {
        
        // ���˃��C���΂��B
        ShootRay(CHS_IDENTIFICATION_RAYID_RECLECTION, worldPos, reflect(WorldRayDirection(), worldNormal), payload, gRtScene);
        
    }

    // instanceID�����܂̃��C����������܂̃��C���΂��B
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_REFRACTION)
    {

        float refractVal = 1.4f;
        float3 rayDir = float3(0, 0, 0);

        // �@���ƃ��C�̕�������ς��ċ�C������I�u�W�F�N�g�ɋ��܂���̂��A�I�u�W�F�N�g�����C�ɋ��܂���̂��𔻒肷��B
        float nr = dot(worldNormal, WorldRayDirection());
        if (nr < 0)
        {

            // ��C������I�u�W�F�N�g
            float eta = 1.0f / refractVal;
            rayDir = refract(WorldRayDirection(), worldNormal, eta);

        }
        else
        {

            // �I�u�W�F�N�g�����C��
            float eta = refractVal / 1.0f;
            rayDir = refract(WorldRayDirection(), -worldNormal, eta);
      
        }
        
        if (length(rayDir) == 0)
        {
            payload.color = worldNormal;
            return;
        }

        ShootRay(CHS_IDENTIFICATION_RAYID_REFRACTION, worldPos, rayDir, payload, gRtScene);

    }
    
    //// �t�H�O�̌v�Z���s���B
    //const float MAX_RAY_LENGTH = 8000.0f; // ���C�̍ő�̒���
    //float rayLength = RayTCurrent(); // ���݂̐[�x�B
    //float depthRate = rayLength / MAX_RAY_LENGTH;
    //// ���C�̒����̃��[�g�ɃC�[�W���O��������B
    //depthRate = 1.0f - cos((depthRate * PI) / 2.0f);
    //const float SKYDOME_RADIUS = 15000.0f;
    //const float SAMPLING_POS_Y = 0.0f;
    //// �T���v�����O�������V����ł̈ʒu�܂ł̃x�N�g���B
    //float3 samplingVec = WorldRayDirection() * SKYDOME_RADIUS;
    //// Y���̃T���v�����O�������ʒu��ݒ肷��B
    //samplingVec.y = SAMPLING_POS_Y;
    //// ���K������B
    //samplingVec = normalize(samplingVec);
    //// �V����̃T���v�����O�������ʒu�̍��W�����߂�B
    //float3 samplingPos;
    //samplingPos = samplingVec * SKYDOME_RADIUS;
    //// ��C�U���̐F���擾�B
    //float3 mieColor = float3(1, 1, 1);
    //float3 skydomeColor = AtmosphericScattering(samplingPos, mieColor);
    //payload.color += mieColor * (depthRate - 0.5f);
    //payload.color = saturate(payload.color);
    
    
    // ���C�̎�ނɂ���ėv�f������B
    if (!(payload.rayID == CHS_IDENTIFICATION_RAYID_DEF))
    {
        
        // 0�����肽���Ȃ��̂�0�ȊO��������Ƃ�����������ǉ��B
        if (length(payload.aoLuminance) != 0)
        {
            payload.aoLuminance /= 2.0f;
        }
        // 0�����肽���Ȃ��̂�0�ȊO��������Ƃ�����������ǉ��B
        if (length(payload.color) != 0)
        {
            payload.color /= 2.0f;
        }
        // 0�����肽���Ȃ��̂�0�ȊO��������Ƃ�����������ǉ��B
        if (length(payload.giColor) != 0)
        {
            payload.giColor /= 2.0f;
        }
        // 0�����肽���Ȃ��̂�0�ȊO��������Ƃ�����������ǉ��B
        if (length(payload.lightLuminance) != 0)
        {
            payload.lightLuminance /= 2.0f;
        }
        
        // �y�C���[�h�̃��C��ID���f�t�H���g�ɖ߂��B
        payload.rayID = CHS_IDENTIFICATION_RAYID_DEF;
        
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
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT || instanceID == CHS_IDENTIFICATION_INSTANCE_INVISIBILITY)
    {
        IgnoreHit();

    }
    
}