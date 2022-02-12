
// TLAS�̃o�b�t�@���w��B
RaytracingAccelerationStructure gRtScene : register(t0);
// ���C�g���[�V���O���ʃo�b�t�@���w��B
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

// RayGeneration�V�F�[�_�[
[shader("raygeneration")]
void mainRayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;

    // ���C�̐ݒ�
    RayDesc rayDesc;
    rayDesc.Origin = float3(d.x, -d.y, 1);
    rayDesc.Direction = float3(0, 0, -1);
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

    float3 col = 0;

    // ���C���q�b�g�����ꏊ�̐F���擾����B
    col.xy = attrib.barys;
    col.z = 1.0 - col.x - col.y;

    // �F��Ԃ��B
    payload.color = col;

}
