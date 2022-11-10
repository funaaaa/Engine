
//#pragma enable_d3d11_debug_symbols

#include "RaytracingShaderHeader.hlsli"

// TLAS
RaytracingAccelerationStructure gRtScene : register(t0);
ConstantBuffer<ConstBufferData> gSceneParam : register(b0);

// �e���\�[�X��
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
StructuredBuffer<Material> material : register(t2, space1);
Texture2D<float4> texture : register(t3, space1);
Texture2D<float4> normalTexture : register(t4, space1);
Texture2D<float4> metalnessTexture : register(t5, space1);
RWTexture2D<float4> tireMaskTexture : register(u0, space1);
// �T���v���[
SamplerState smp : register(s0, space1);

// �o�͐�UAV
RWTexture2D<float4> aoOutput : register(u0);
RWTexture2D<float4> lightingOutput : register(u1);
RWTexture2D<float4> colorOutput : register(u2);
RWTexture2D<float4> giOutput : register(u3);
RWTexture2D<float4> denoiseMaskoutput : register(u4);

// ��C�U��
float3 AtmosphericScattering(float3 pos, inout float3 mieColor)
{

    // ���C���[�U���萔
    float kr = 0.0025f;
    // �~�[�U���萔
    float km = 0.005f;

    // ��C���̐������T���v�����O���鐔�B
    float fSamples = 2.0f;

    // ��̐F �F�I�ɂ͔��߂̒��F
    float3 three_primary_colors = float3(0.68f, 0.55f, 0.44f);
    // ���̔g���H
    float3 v3InvWaveLength = 1.0f / pow(three_primary_colors, 4.0f);

    // ��C���̈�ԏ�̍����B
    float fOuterRadius = 10250.0f;
    // �n���S�̂̒n��̍����B
    float fInnerRadius = 10000.0f;

    // ���z���̋����H
    float fESun = 20.0f;
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
    float fScaleDepth = 0.35f;
    // �n���S�̂ł̑�C�̊����𕽋ϑ�C���x�Ŋ������l�B
    float fScaleOverScaleDepth = fScale / fScaleDepth;

    // �U���萔�����߂�ۂɎg�p����l�B
    float g = -0.999f;
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
    float fStartOffset = fStartDepth * Scale(fStartAngle);

    // �T���v���|�C���g�Ԃ̒����B
    float fSampleLength = fFar / fSamples;
    // �T���v���|�C���g�Ԃ̒����ɒn���̑�C�̊�����������B
    float fScaledLength = fSampleLength * fScale;
    // �g�U�������������ɃT���v���̒����������邱�ƂŃT���v���|�C���g�Ԃ̃��C���x�N�g�������߂�B
    float3 v3SampleRay = v3Ray * fSampleLength;
    // �ŏ��̃T���v���|�C���g�����߂�B0.5�������Ă�̂͏������������߁H
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;

    // �F���
    float3 v3FrontColor = 0.0f;
    for (int n = 0; n < int(fSamples); ++n)
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
        float fScatter = (fStartOffset + fDepth * (Scale(fLightAngle * 1) - Scale(fCameraAngle * 1)));

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
    int randSeed = InitRand(DispatchRaysIndex().x + (vtx.Position.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);
    
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
    //int randSeed = InitRand(DispatchRaysIndex().x + (worldPosition.x * gSceneParam.light.dirLight.lightDir.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100);

    //// ���C�����x�N�g��
    //float3 shadowRayDir = GetConeSample(randSeed, dirLightVec, coneAngle);
    float3 shadowRayDir = dirLightVec;
    
    return ShootShadowRayNoAH(worldPosition + normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3())) * 2.0f, shadowRayDir, length, gRtScene);
    
}

// GI
void ShootGIRay(Vertex vtx, float length, inout Payload PayloadData, float3 WorldNormal)
{
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldRayDir = WorldRayDirection();
    float3 reflectDir = reflect(worldRayDir, WorldNormal);
    
    // ���C�̃t���O��ݒ�B
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�𖳎��B

    // ���C�̃}�X�N
    uint rayMask = 0xFF;

    // ���C�̃p�����[�^�[��ݒ�B
    RayDesc rayDesc;
    rayDesc.Origin = worldPos;
    rayDesc.Direction = reflectDir;
    rayDesc.TMin = 0.0;
    rayDesc.TMax = length;

    // �y�C���[�h���������B
    int rayID = PayloadData.rayID_;
    PayloadData.rayID_ = CHS_IDENTIFICATION_RAYID_GI;
    
    // ���C�𔭎ˁB
    TraceRay(
        gRtScene,
        flag,
        rayMask,
        0, // ray index
        1, // MultiplierForGeometryContrib
        0, // miss index
        rayDesc,
        PayloadData);
    
    PayloadData.rayID_ = rayID;
    
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
    Payload payloadData;
    payloadData.impactAmount_ = 1.0f;
    payloadData.rayID_ = CHS_IDENTIFICATION_RAYID_DEF;
    payloadData.recursive_ = 0;
    payloadData.ao_ = 0;
    payloadData.color_ = float3(0, 0, 0);
    payloadData.denoiseMask_ = float3(0, 0, 0);
    payloadData.gi_ = float3(0, 0, 0);
    payloadData.light_ = float3(0, 0, 0);
    payloadData.isCullingAlpha_ = false;
    payloadData.alphaCounter_ = 0;

    // TransRay�ɕK�v�Ȑݒ���쐬
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    //flag |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShader�����s�B

    // ���C�𔭎�
    TraceRay(
    gRtScene, // TLAS
    flag, // �Փ˔��萧�������t���O
    rayMask, // �Փ˔���Ώۂ̃}�X�N�l
    0, // ray index
    1, // MultiplierForGeometryContrib
    0, // miss index
    rayDesc,
    payloadData);

    // ���ʏ������ݗpUAV����U�������B
    lightingOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    aoOutput[launchIndex.xy] = float4(0, 0, 0, 1);
    colorOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    giOutput[launchIndex.xy] = float4(1, 1, 1, 1);
    denoiseMaskoutput[launchIndex.xy] = float4(1, 1, 1, 1);

    // Linear -> sRGB
    payloadData.light_ = 1.055f * pow(payloadData.light_, 1.0f / 2.4f) - 0.055f;
    payloadData.ao_ = 1.055f * pow(payloadData.ao_, 1.0f / 2.4f) - 0.055f;
    //payloadData.color_ = pow(payloadData.color_, 1.0f / 2.2f);
    //payloadData.gi_ = pow(payloadData.gi_, 1.0f / 2.2f);

    // ���ʊi�[
    lightingOutput[launchIndex.xy] = float4(saturate(payloadData.light_), 1);
    aoOutput[launchIndex.xy] = float4(saturate(payloadData.ao_), saturate(payloadData.ao_), saturate(payloadData.ao_), 1);
    colorOutput[launchIndex.xy] = float4(saturate(payloadData.color_), 1);
    giOutput[launchIndex.xy] = float4(saturate(payloadData.gi_), 1);
    denoiseMaskoutput[launchIndex.xy] = float4(payloadData.denoiseMask_, 1);

}

// miss�V�F�[�_�[ ���C���q�b�g���Ȃ��������ɌĂ΂��V�F�[�_�[
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
    
    if (PayloadData.rayID_ == CHS_IDENTIFICATION_RAYID_GI || PayloadData.rayID_ == CHS_IDENTIFICATION_RAYID_SHADOW)
        return;
    
    // �y�C���[�h�󂯎��p�ϐ��B
    Payload payloadBuff = PayloadData;
    
    // �e���x�������F��ۑ��B
    float3 mieColor = float3(1, 1, 1);
    payloadBuff.light_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
    payloadBuff.color_ += AtmosphericScattering(WorldRayOrigin() + WorldRayDirection() * RayTCurrent(), mieColor) * payloadBuff.impactAmount_ * payloadBuff.impactAmount_;
    payloadBuff.ao_ += 1.0f * payloadBuff.impactAmount_;
    payloadBuff.gi_ += float3(0, 0, 0) * payloadBuff.impactAmount_;
        
    // �}�X�N�̐F�𔒂�����B(���C�g���[�N�΍�ő��̃}�X�N�̐F�Ƃ��Ԃ�Ȃ��悤�ɂ��邽�߁B)
    payloadBuff.denoiseMask_ = float3(1, 1, 1);
        
    // �T���v�����O�����_�̋P�x���擾����B
    float t = dot(payloadBuff.color_.xyz, float3(0.2125f, 0.7154f, 0.0721f));
        
    // �T���v�����O�����_���n���艺����Ȃ�������B
    if (t <= 0.9f)
    {
            
        // �T���v�����O�����P�x�����ƂɁA�Â������琯���`�悷��B
        t = (1.0f - t);
        if (t != 0.0f)
        {
            t = pow(t, 10.0f);
            //t = pow(2.0f, 10.0f * t - 10.0f);
        }
        //payloadBuff.color_ += (float3) TexColor * t * payloadBuff.impactAmount_;
        //payloadBuff.color_ = saturate(payloadBuff.color_);
            
    }
        
    // �e���x��0�ɂ���B
    payloadBuff.impactAmount_ = 0.0f;
        
    PayloadData = payloadBuff;

}

// �V���h�E�pmiss�V�F�[�_�[
[shader("miss")]
void shadowMS(inout Payload payload)
{
    // ���ɂ��������Ă��Ȃ��Ƃ������ƂȂ̂ŁA�e�͐������Ȃ��B
    payload.impactAmount_ = 1.0f;
}

// ���C�e�B���O�O����
bool ProcessingBeforeLighting(inout Payload PayloadData, Vertex Vtx, MyAttribute Attrib, float3 WorldPos, float3 WorldNormal, inout float4 TexColor, uint InstanceID)
{
    
    // �y�C���[�h�󂯎��p�ϐ��B
    Payload payloadBuff = PayloadData;
    
    // �f�m�C�Y�p�̃}�X�N�Ɏg�p����e�N�X�`���ɖ@���̐F��InstanceIndex�����������̂��������ށB
    if (payloadBuff.recursive_ == 1)
    {
        payloadBuff.denoiseMask_ = (WorldNormal);
    }
    
    // InstanceID��CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK��������e�N�X�`���ɐF�����Z�B
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK)
    {
        float4 tiremasktex = (float4) tireMaskTexture[uint2((uint) (Vtx.subUV.x * 4096.0f), (uint) (Vtx.subUV.y * 4096.0f))];
        //if (tiremasktex.x == 0)
        //{
        //    TexColor = tiremasktex;
        //}
        //else if (tiremasktex.x != 1)
        //{
        //    TexColor += tiremasktex;
        //    TexColor = normalize(TexColor);

        //}
        TexColor += tiremasktex * tiremasktex.a;
        TexColor = normalize(TexColor);
    }
    
    // �����˂���Ă��郌�C��ID��GI�p��������
    if (payloadBuff.rayID_ == CHS_IDENTIFICATION_RAYID_GI)
    {
        
        // ���C�̒��������߂�B
        float rayLength = length(WorldRayOrigin() - WorldPos);
        
        // GI��\�����郌�C�̒����̍ő�l�B
        const float MAX_RAY = 300.0f;
        
        // ���C�̒����̊����B
        float rate = rayLength / MAX_RAY;
        rate = 1.0f - saturate(rate);
        
        float3 giBuff = (float3) TexColor * rate * (1.0f - material[0].metalness_);
        
        //// ���������I�u�W�F�N�g�����S���˂�������GI�̐F����������B(���S���˂ŐF���Ȃ�����B���͐F�Ƃ��Ĕ��f����Ȃ��B)
        //if (InstanceID == CHS_IDENTIFICATION_ISNTANCE_COMPLETE_REFLECTION)
        //{
        //    giBuff = float3(0, 0, 0);
        //}
        
        //// ���������I�u�W�F�N�g�����˂�������GI�̐F�𔖂�����B
        //if (InstanceID == CHS_IDENTIFICATION_ISNTANCE_REFLECTION)
        //{
        //    giBuff /= 2.0f;
        //}
        
        payloadBuff.gi_ += giBuff;
        
        PayloadData = payloadBuff;
        
        return true;
    }

    
    // ���������I�u�W�F�N�g���V����������B
    //if (InstanceID == CHS_IDENTIFICATION_INSTNACE_AS)
    //{
    //    float3 mieColor = float3(1, 1, 1);
        
    //    // �e���x�������F��ۑ��B
    //    payloadBuff.light_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
    //    payloadBuff.color_ += AtmosphericScattering(WorldPos, mieColor) * payloadBuff.impactAmount_ * payloadBuff.impactAmount_;
    //    payloadBuff.ao_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
    //    payloadBuff.gi_ += float3(0, 0, 0) * payloadBuff.impactAmount_;
        
    //    // �}�X�N�̐F�𔒂�����B(���C�g���[�N�΍�ő��̃}�X�N�̐F�Ƃ��Ԃ�Ȃ��悤�ɂ��邽�߁B)
    //    payloadBuff.denoiseMask_ = float3(1, 1, 1);
        
    //    // �T���v�����O�����_�̋P�x���擾����B
    //    float t = dot(payloadBuff.color_.xyz, float3(0.2125f, 0.7154f, 0.0721f));
        
    //    // �T���v�����O�����_���n���艺����Ȃ�������B
    //    if (t <= 0.9f)
    //    {
            
    //        // �T���v�����O�����P�x�����ƂɁA�Â������琯���`�悷��B
    //        t = (1.0f - t);
    //        if (t != 0.0f)
    //        {
    //            t = pow(t, 10.0f);
    //            //t = pow(2.0f, 10.0f * t - 10.0f);
    //        }
    //        payloadBuff.color_ += (float3) TexColor * t * payloadBuff.impactAmount_;
    //        payloadBuff.color_ = saturate(payloadBuff.color_);
            
    //    }
        
    //    // �e���x��0�ɂ���B
    //    payloadBuff.impactAmount_ = 0.0f;
        
    //    PayloadData = payloadBuff;
        
    //    return true;
    //}
    
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT)
    {
        payloadBuff.light_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
        payloadBuff.color_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
        payloadBuff.ao_ += 1 * payloadBuff.impactAmount_;
        payloadBuff.gi_ += float3(0, 0, 0);
        
        // �e���x��0�ɂ���B
        payloadBuff.impactAmount_ = 0.0f;
        
        PayloadData = payloadBuff;
        
        return true;
    }
    
    // ���������I�u�W�F�N�gInstanceID���e�N�X�`���̐F�����̂܂ܕԂ� or ���C�g�p�I�u�W�F�N�g��������
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_TEXCOLOR)
    {
        payloadBuff.light_ += float3(1, 1, 1) * payloadBuff.impactAmount_;
        payloadBuff.color_ += (float3) TexColor * payloadBuff.impactAmount_;
        payloadBuff.ao_ += 1 * payloadBuff.impactAmount_;
        payloadBuff.gi_ += float3(0, 0, 0);
        
        // �e���x��0�ɂ���B
        payloadBuff.impactAmount_ = 0.0f;
        
        PayloadData = payloadBuff;
        
        return true;
    }
    
    // ���������I�u�W�F�N�g��InstanceID�����S���˂�������B
    float metalness = 1.0f - material[0].metalness_;
    if (metalness <= 0.0f)
    {
        
        // ���S���˗p�̃��C�𔭎ˁB
        ShootRay(CHS_IDENTIFICATION_RAYID_DEF, WorldPos, reflect(WorldRayDirection(), WorldNormal), payloadBuff, gRtScene);
        
        // �F�������������邭����B
        //PayloadData.color += 0.1f;
        
        payloadBuff.color_ = saturate(payloadBuff.color_);
        
        //// �c�����e���x������B
        //PayloadData.rayData_[RayDataIndex].impactRate_ = 0.0f;
        
        PayloadData = payloadBuff;
        
        return true;
        
    }
        
    PayloadData = payloadBuff;
    
    return false;
    
}

// UE4��GGX���z
float DistributionGGX(float Alpha, float NdotH)
{
    float alpha2 = Alpha * Alpha;
    float t = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * t * t);
}

// Schlick�ɂ��t���l���̋ߎ� 
float SchlickFresnel(float F0, float F90, float Cosine)
{
    float m = saturate(1.0f - Cosine);
    float m2 = m * m;
    float m5 = m2 * m2 * m;
    return lerp(F0, F90, m5);
}
float3 SchlickFresnel3(float3 F0, float3 F90, float Cosine)
{
    float m = saturate(1.0f - Cosine);
    float m2 = m * m;
    float m5 = m2 * m2 * m;
    return lerp(F0, F90, m5);
}

// �f�B�Y�j�[�̃t���l���v�Z
float3 DisneyFresnel(float LdotH)
{
    
    // �P�x
    float luminance = 0.3f * material[0].baseColor_.r + 0.6f * material[0].baseColor_.g + 0.1f * material[0].baseColor_.b;
    // �F����
    float3 tintColor = material[0].baseColor_ / luminance;
    // ������̋��ʔ��ːF���v�Z
    float3 nonMetalColor = material[0].specular_ * 0.08f * tintColor;
    // metalness�ɂ��F�⊮ �����̏ꍇ�̓x�[�X�J���[
    float3 specularColor = lerp(nonMetalColor, material[0].baseColor_, material[0].metalness_);
    // NdotH�̊�����SchlickFresnel���
    return SchlickFresnel3(specularColor, float3(1.0f, 1.0f, 1.0f), LdotH);
    
}

// UE4��Smith���f��
float GeometricSmith(float Cosine)
{
    float k = (material[0].roughness_ + 1.0f);
    k = k * k / 8.0f;
    return Cosine / (Cosine * (1.0f - k) + k);
}

// ���ʔ��˂̌v�Z
float3 CookTorranceSpecular(float NdotL, float NdotV, float NdotH, float LdotH)
{
    
    // D��(���z:Distribution)
    float Ds = DistributionGGX(material[0].roughness_ * material[0].roughness_, NdotH);
    
    // F��(�t���l��:Fresnel)
    float3 Fs = DisneyFresnel(LdotH);
    
    // G��(�􉽌���:Geometry attenuation)
    float Gs = GeometricSmith(NdotL) * GeometricSmith(NdotV);
    
    // M��(����)
    float m = 4.0f * NdotL * NdotV;
    
    // �������ċ��ʔ��˂̐F�𓾂�B
    return Ds * Fs * Gs / m;
    
}

// �o�������˕��z�֐�
float3 BRDF(float3 LightVec, float3 ViewVec, float3 Normal)
{
    // �@���ƃ��C�g�����̓���
    float NdotL = dot(Normal, LightVec);
    
    // �@���ƃJ���������̓���
    float NdotV = dot(Normal, ViewVec);
    
    // �ǂ��炩��90�x�ȏ�ł���ΐ^������Ԃ��B
    if (NdotL < 0.0f || NdotV < 0.0f)
    {
        
        return float3(0.0f, 0.0f, 0.0f);
        
    }
    
    // ���C�g�����ƃJ���������̒��Ԃł���n�[�t�x�N�g��
    float3 halfVec = normalize(LightVec + ViewVec);
    
    // �@���ƃn�[�t�x�N�g���̓���
    float NdotH = dot(Normal, halfVec);
    
    // ���C�g�ƃn�[�t�x�N�g���̓���
    float LdotH = dot(LightVec, halfVec);
    
    // �g�U���˗�
    float diffuseReflectance = 1.0f / PI;
    
    // ���ˊp��90�x�̏ꍇ�̊g�U���˗�
    float energyBias = 0.5f * material[0].roughness_;
    float FD90 = energyBias + 2.0f * LdotH * LdotH * material[0].roughness_;
    
    // �����Ă����Ƃ��̊g�U���˗�
    float FL = SchlickFresnel(1.0f, FD90, NdotL);
    
    // �o�Ă����Ƃ��̊g�U���˗�
    float FV = SchlickFresnel(1.0f, FD90, NdotV);
    
    // �����ďo�Ă����܂ł̊g�U���˗�
    float energyFactor = lerp(1.0f, 1.0f / 1.51f, material[0].roughness_);
    float FD = FL * FV * energyFactor;
    
    // �g�U���ˍ�
    float3 diffuseColor = diffuseReflectance * FD * material[0].baseColor_ * (1.0f - material[0].metalness_);
    
    // ���ʔ��ˍ�
    float3 specularColor = CookTorranceSpecular(NdotL, NdotV, NdotH, LdotH);
    
    return diffuseColor + specularColor;
    
}

// ���C�e�B���O����
bool Lighting(inout Payload PayloadData, float3 WorldPos, float3 WorldNormal, Vertex Vtx, float4 TexColor)
{
    
    // Payload�ꎞ�󂯎��p�ϐ��B
    Payload payloadBuff = PayloadData;
    
    // �����̎�ƂȂ�l���擾�B
    uint2 pixldx = DispatchRaysIndex().xy;
    uint2 numPix = DispatchRaysDimensions().xy;
    
    // �e�����̖��邳���
    float aoLightVisibility = 0;
    
    for (int index = 0; index < POINT_LIGHT_COUNT; ++index)
    {
        
        // �|�C���g���C�g���L��������Ă��Ȃ������珈�����΂��B
        if (!gSceneParam.light.pointLight[index].isActive)
        {
            continue;
        }
        
        // �����܂ł̒���
        float lightLength = length(gSceneParam.light.pointLight[index].lightPos - WorldPos);
    
        // �����܂ł̒������_�����̋�����菬���������珈�����΂��B
        if (lightLength < gSceneParam.light.pointLight[index].lightPower && gSceneParam.light.pointLight[index].isActive)
        {
            
            // ���C�g�̉B���x
            float pointLightVisibilityBuff = 0;
            if (gSceneParam.light.pointLight[index].isShadow)
            {
                pointLightVisibilityBuff = SoftShadow(Vtx, gSceneParam.light.pointLight[index].lightSize, lightLength, index);
            }
        
            // ���C�g�̖��邳�����ȏゾ������X�y�L�����[�Ȃǂ��v�Z����B
            if (0 < pointLightVisibilityBuff)
            {
                
                float3 pointLightDir = WorldPos - gSceneParam.light.pointLight[index].lightPos;
                pointLightDir = normalize(pointLightDir);
            
                // ���C�g�܂ł̋����̊���
                float rate = lightLength / gSceneParam.light.pointLight[index].lightPower;
                rate = pow(rate, 5);
                rate = 1.0f - rate;
                

                payloadBuff.light_ += gSceneParam.light.pointLight[index].lightColor * BRDF(-pointLightDir, -WorldRayDirection(), WorldNormal) * rate * PayloadData.impactAmount_;

            }
        
        }
    }
    
            
    // ���z�̈ʒu�ƃx�N�g��
    float3 sunPos = -gSceneParam.light.dirLight.lightDir * 300000.0f;
    float3 sunDir = normalize(sunPos - WorldPos);
    
    // �f�B���N�V���i�����C�g�̐F
    if (gSceneParam.light.dirLight.isActive && gSceneParam.light.dirLight.lightDir.y < 0.2f)
    {
        
        // �f�B���N�V���i�����C�g�̕����Ƀ��C���΂��B
        float dirLightVisibility = ShootDirShadow(Vtx, 10000.0f);
        
        // �f�B���N�V���i�����C�g�̖��邳�����ȏゾ������
        if (0.0f < dirLightVisibility)
        {
            
            const float SKYDOME_RADIUS = 15000.0f;
            const float SAMPLING_POS_Y = 0.0f;
            
            // �V���̐F���T���v�����O
            //float3 samplingVec = normalize(float3(1.0f, 0.1f, 0.1f)) * SKYDOME_RADIUS;
            float3 samplingVec = normalize(-gSceneParam.light.dirLight.lightDir * float3(1.0f, 0.0f, 1.0f)) * SKYDOME_RADIUS;
            samplingVec.y = 0.1f;
            
            // �T���v�����O����x�N�g��
            samplingVec.y = SAMPLING_POS_Y;
            samplingVec = normalize(samplingVec);
            
            // �T���v�����O������W
            float3 samplingPos;
            samplingPos = samplingVec * SKYDOME_RADIUS;
            
            // ��C�U���̐F
            float3 mieColor = float3(1, 1, 1);
            float3 skydomeColor = AtmosphericScattering(samplingPos, mieColor);
            
            payloadBuff.light_ += (mieColor * float3(1.0f, 0.5f, 0.5f)) + BRDF(-gSceneParam.light.dirLight.lightDir, -WorldRayDirection(), WorldNormal) * PayloadData.impactAmount_;
            payloadBuff.light_ = saturate(payloadBuff.light_);
            
        }
        else
        {
            // �g�U���ˌ����[���I�ɍČ����邽�߂ɖ��邳�����������グ��B
            payloadBuff.light_ += float3(0.3f, 0.3f, 0.3f);
            payloadBuff.light_ = saturate(payloadBuff.light_);
        }
        
        
    }
        
    // AO�̌v�Z�B
    {
     
        int seed = InitRand(DispatchRaysIndex().x + (WorldPos.x / 1000.0f) + DispatchRaysIndex().y * numPix.x, 100, 16);
        float3 sampleDir = GetUniformHemisphereSample(seed, WorldNormal);
        
        float aoLightVisibilityBuff = ShootAOShadowRay(WorldPos, sampleDir, 15, gRtScene);
        
        float NoL = saturate(dot(WorldNormal, sampleDir));
        float pdf = 1.0f / (2.0f * PI);
        aoLightVisibility += aoLightVisibilityBuff;
        
    }
    
    
    // ���C�g�̑����B���x�����߂�B
    float aoVisibility = aoLightVisibility;
    
    // �e�F��ݒ�B
    payloadBuff.ao_ += aoVisibility * payloadBuff.impactAmount_;
    
    PayloadData = payloadBuff;
    
    return false;
    
}

// ���C�e�B���O�㏈��
void ProccessingAfterLighting(inout Payload PayloadData, Vertex Vtx, float3 WorldPos, float3 WorldNormal, inout float4 TexColor, uint InstanceID)
{
    
    // Payload�ꎞ�󂯎��p�ϐ��B
    Payload payloadBuff = PayloadData;
    
    // ���������I�u�W�F�N�g��GI���s���I�u�W�F�N�g�ŁAGI���s���t���O�������Ă�����B
    if ((InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_GI || InstanceID == CHS_IDENTIFICATION_INSTANCE_DEF_GI_TIREMASK))
    {
        if (0.0f < payloadBuff.impactAmount_)
        {
            ShootGIRay(Vtx, 150, payloadBuff, WorldNormal);
            payloadBuff.gi_ = (payloadBuff.gi_ * payloadBuff.impactAmount_);
            payloadBuff.gi_ = saturate(payloadBuff.gi_);
            payloadBuff.color_.xyz += (float3) TexColor * payloadBuff.impactAmount_;
        
            payloadBuff.impactAmount_ = 0.0f;
        }
        
    }
        
    // �����x
    float metalness = 1.0f - material[0].metalness_;
    
    // ���������I�u�W�F�N�g��InstanceID���A���t�@��������
    if (InstanceID == CHS_IDENTIFICATION_INSTANCE_ALPHA || InstanceID == CHS_IDENTIFICATION_INSTANCE_ADD)
    {
        
        // �A���t�@�l�����߂�B
        int instanceIndex = InstanceIndex();
        float alpha = 0;
        for (int alphaIndex = 0; alphaIndex < ALPHA_DATA_COUNT; ++alphaIndex)
        {
            if (gSceneParam.alphaData_.alphaData_[alphaIndex].instanceIndex_ != instanceIndex)
            {
                continue;
            }
            alpha = gSceneParam.alphaData_.alphaData_[alphaIndex].alpha_;
            break;
        }
        
        // �A���t�@�u�����h��������
        if (InstanceID == CHS_IDENTIFICATION_INSTANCE_ALPHA)
        {
        
            if (payloadBuff.impactAmount_ < alpha * TexColor.w)
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w * payloadBuff.impactAmount_;
                payloadBuff.light_ += float3(1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_);
                payloadBuff.impactAmount_ = 0.0f;

            }
            else
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w * alpha;
                payloadBuff.light_ += float3(1 * alpha * TexColor.w, 1 * alpha * TexColor.w, 1 * alpha * TexColor.w);
                payloadBuff.impactAmount_ -= alpha * TexColor.w;
            }
            
        }
        // ���Z������������
        else if (InstanceID == CHS_IDENTIFICATION_INSTANCE_ADD)
        {
            
            payloadBuff.light_ = float3(TexColor.w, TexColor.w, TexColor.w);
            
            if (payloadBuff.impactAmount_ < alpha * TexColor.w)
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w;
                payloadBuff.light_ += float3(1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_, 1 * payloadBuff.impactAmount_);
                payloadBuff.impactAmount_ = 0.0f;

            }
            else
            {
                payloadBuff.color_.xyz += (float3) TexColor * TexColor.w;
                payloadBuff.light_ += float3(1 * alpha * TexColor.w, 1 * alpha * TexColor.w, 1 * alpha * TexColor.w);
                payloadBuff.impactAmount_ -= alpha * TexColor.w;
            }
        }
        
        // �A���t�@�����ȉ���������B
        if (alpha < 0.5f)
        {
            ++payloadBuff.alphaCounter_;
            if (3 <= payloadBuff.alphaCounter_)
            {
                payloadBuff.isCullingAlpha_ = true;
            }
        }
            
        if (0.0f < payloadBuff.impactAmount_)
        {
                
            // ���˃��C���΂��B
            ShootRay(CHS_IDENTIFICATION_RAYID_DEF, WorldPos, WorldRayDirection(), payloadBuff, gRtScene);
            
        }
        
    }
    // ���������I�u�W�F�N�g��InstanceID�����܂�������
    else if (InstanceID == CHS_IDENTIFICATION_INSTANCE_REFRACTION)
    {
        
        if (payloadBuff.impactAmount_ < metalness)
        {
            payloadBuff.color_.xyz += (float3) TexColor * payloadBuff.impactAmount_;
            payloadBuff.impactAmount_ = 0.0f;

        }
        else
        {
            payloadBuff.color_.xyz += (float3) TexColor * metalness;
            payloadBuff.impactAmount_ -= metalness;
        }

        float refractVal = 1.4f;
        float3 rayDir = float3(0, 0, 0);

        float nr = dot(WorldNormal, WorldRayDirection());
        if (nr < 0)
        {

            // ��C��->�I�u�W�F�N�g
            float eta = 1.0f / refractVal;
            rayDir = refract(WorldRayDirection(), WorldNormal, eta);

        }
        else
        {

            // �I�u�W�F�N�g->��C��
            float eta = refractVal / 1.0f;
            rayDir = refract(WorldRayDirection(), -WorldNormal, eta);
      
        }
            
        if (0.0f < payloadBuff.impactAmount_)
        {
                
            ShootRay(CHS_IDENTIFICATION_RAYID_REFRACTION, WorldPos, rayDir, payloadBuff, gRtScene);
            
        }

    }
    // ���˂̏���
    else
    {
        
        // metalness�}�b�v�̐F���擾�B
        float3 metalnessMapColor = (float3) metalnessTexture.SampleLevel(smp, Vtx.uv, 0.0f);
        float metal = metalness;
        
        // metalness�}�b�v�̐F�ƃe�N�X�`���̐F����������Ȃ�������metallness�}�b�v�̐F���Ď擾�B(metalness�}�b�v���Ȃ��e�N�X�`���ɂ̓������̌��Ԃ𖄂߂邽�߂Ɉꉞ�e�N�X�`��������Ă��邩��B)
        if (!(TexColor.x == metalnessMapColor.x && TexColor.y == metalnessMapColor.y && TexColor.z == metalnessMapColor.z))
        {
            metal = saturate(metal + ((1.0f - metal) * (1.0f - metalnessMapColor.x)));
        }
        
        if (payloadBuff.impactAmount_ < metal)
        {
            payloadBuff.color_.xyz += (float3) TexColor * payloadBuff.impactAmount_;
            payloadBuff.impactAmount_ = 0.0f;
        }
        else
        {
            payloadBuff.color_.xyz += (float3) TexColor * metal;
            payloadBuff.impactAmount_ -= metal;
            
            if (0.0f < payloadBuff.impactAmount_)
            {
                
                // ���˃��C���΂��B
                ShootRay(CHS_IDENTIFICATION_RAYID_RECLECTION, WorldPos, reflect(WorldRayDirection(), WorldNormal), payloadBuff, gRtScene);
                
            }
        
        }
        
    }
    
    PayloadData = payloadBuff;
    
}

// closesthit�V�F�[�_�[ ���C���q�b�g�������ɌĂ΂��V�F�[�_�[
[shader("closesthit")]

    void mainCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
    
    // �e�p���C��������B
    if (payload.rayID_ == CHS_IDENTIFICATION_RAYID_SHADOW)
    {
        
        // �A���t�@�̃I�u�W�F�N�g��������
        if (InstanceID() == CHS_IDENTIFICATION_INSTANCE_ALPHA)
        {
            
            // �e�N�X�`���̐F���擾�B
            Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
            float4 texColor = (float4) texture.SampleLevel(smp, vtx.uv, 0.0f);
        
            payload.impactAmount_ = texColor.a;
            
            return;
            
        }
           
        payload.impactAmount_ = 0.0f;
        
        
        return;
    }

    // �Ăяo���񐔂������𒴂��Ȃ��悤�ɂ���B
    ++payload.recursive_;
    if (5 < payload.recursive_ || payload.impactAmount_ <= 0 || payload.isCullingAlpha_)
    {
        
        return;
    }
    
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    uint instanceID = InstanceID();
    float3 worldPos = mul(float4(vtx.Position, 1), ObjectToWorld4x3());
    float3 worldNormal = normalize(mul(vtx.Normal, (float3x3) ObjectToWorld4x3()));

    // �e�N�X�`���̐F���擾�B
    float4 texColor = (float4) texture.SampleLevel(smp, vtx.uv, 1.0f);   
    
    // �@���}�b�v�̐F���擾�B
    float3 normalMapColor = (float3) normalTexture.SampleLevel(smp, vtx.uv, 0.0f);
    
    // �@���}�b�v�̐F�ƃe�N�X�`���̐F����������Ȃ�������@���}�b�v�̐F���Ď擾�B(�@���}�b�v���Ȃ��e�N�X�`���ɂ̓������̌��Ԃ𖄂߂邽�߂Ɉꉞ�e�N�X�`��������Ă��邩��B)
    if (!(texColor.x == normalMapColor.x && texColor.y == normalMapColor.y && texColor.z == normalMapColor.z))
    {
        worldNormal = normalize(mul(normalMapColor, (float3x3) ObjectToWorld4x3()));
        
        // �����������b�V���̏����擾�B
        Vertex meshInfo[3];
        GetHitMeshInfo(attrib, vertexBuffer, indexBuffer, meshInfo);
        
        // �ڋ�ԕϊ��p
        float3 tan;
        float3 bnorm;
        CalcTangentAndBinormal(meshInfo[0].Position, meshInfo[1].Position, meshInfo[2].Position, meshInfo[0].uv, meshInfo[1].uv, meshInfo[2].uv, tan, bnorm);
        
        // ����ԍs������߂�B
        float3x3 mat =
        {
            float3(tan),
            float3(bnorm),
            float3(vtx.Normal)
        };
        
        worldNormal = mul(worldNormal, mat);

    }
    

    // ���C�e�B���O�O�̏��������s�B----- �S���˃I�u�W�F�N�g��e�N�X�`���̐F�����̂܂܎g���I�u�W�F�N�g�̐F�擾�����B
    if (ProcessingBeforeLighting(payload, vtx, attrib, worldPos, worldNormal, texColor, instanceID))
    {
        return;
    }
    

    // ���C�e�B���O�̏���
    if (instanceID != CHS_IDENTIFICATION_INSTANCE_ALPHA)
    {
        if (Lighting(payload, worldPos, worldNormal, vtx, texColor))
        {
            return;
        }
    }
    
    // ���C�e�B���O��̏��������s�B ----- ���C�e�B���O�̌��ʂ��������鏈���┽�˂���ܓ��̍Ăу��C���΂��K�v������I�u�W�F�N�g�̏����B
    ProccessingAfterLighting(payload, vtx, worldPos, worldNormal, texColor, instanceID);
    
}

// �e�pCHS �g�p���Ă��Ȃ��B
[shader("closesthit")]

    void shadowCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
}

// AnyHitShader
[shader("anyhit")]

    void mainAnyHit
    (inout
    Payload payload, MyAttribute
    attrib)
{
        
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    float4 diffuse = texture.SampleLevel(smp, vtx.uv, 0);
    if (diffuse.w < 0.1f)
    {
        IgnoreHit();

    }
    
    int instanceID = InstanceID();
    
    // ���������I�u�W�F�N�g��InstanceID�����C�g�por�s���̃I�u�W�F�N�g�������瓖���蔻������p
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_LIGHT || instanceID == CHS_IDENTIFICATION_INSTANCE_INVISIBILITY)
    {
        IgnoreHit();

    }
    
    // ���������I�u�W�F�N�g��InstanceID���A���t�@��������
    if (instanceID == CHS_IDENTIFICATION_INSTANCE_ALPHA || instanceID == CHS_IDENTIFICATION_INSTANCE_ADD)
    {
        
        // ���ȏ㔖���A���t�@�l�̃I�u�W�F�N�g�Ƃ������Ă�����B
        if (payload.isCullingAlpha_)
        {
            IgnoreHit();

        }
        
        if (payload.rayID_ == CHS_IDENTIFICATION_RAYID_SHADOW)
        {
            IgnoreHit();

        }
        
        // �A���t�@�l�����߂�B
        int instanceIndex = InstanceIndex();
        float alpha = 0;
        for (int alphaIndex = 0; alphaIndex < ALPHA_DATA_COUNT; ++alphaIndex)
        {
            if (gSceneParam.alphaData_.alphaData_[alphaIndex].instanceIndex_ != instanceIndex)
            {
                continue;
            }
            alpha = gSceneParam.alphaData_.alphaData_[alphaIndex].alpha_;
            break;
        }
        
        // �e�N�X�`���̐F���擾�B
        float4 texColor = (float4) texture.SampleLevel(smp, vtx.uv, 0.0f);
        texColor.xyz *= texColor.w;
        
        alpha *= texColor.w;
        
        if (alpha <= 0.1f)
        {
            IgnoreHit();

        }
        
    }
    
    //AcceptHitAndEndSearch();
    
}
