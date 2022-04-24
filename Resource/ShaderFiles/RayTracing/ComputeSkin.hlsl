
// ���_�f�[�^
static const int MAX_BONE = 16;

struct VertexPosNormalUvSkin
{
    float3 pos : POSITION; // ���W
    float3 normal : NORMAL; // �@��
    float2 uv : TEXCOORD; // UV
    uint4 boneIndex : BONEINDICES; // �{�[���̔ԍ�
    float4 boneWeight : BONEWEIGHTS; // �{�[���̏d��
};

struct SkinData
{
    matrix bones[MAX_BONE];
};

// ���̓f�[�^�\����
struct InputData
{
    VertexPosNormalUvSkin vertex;
    SkinData skinData;
};

//�o�̓f�[�^�\����
struct OutputData
{
    float3 pos;
    float3 normal;
    float2 uv;
};

//���̓f�[�^�ɃA�N�Z�X���邽�߂̕ϐ�
StructuredBuffer<InputData> input : register(t0);

//�o�̓f�[�^�ɃA�N�Z�X���邽�߂̕ϐ�
RWStructuredBuffer<OutputData> output : register(u0);

//�G���g���[�|�C���g
[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

    // �[���N���A
    OutputData outputData = (OutputData) 0;

    uint iBone; // �v�Z����{�[���̔ԍ�
    float weight; // �{�[���E�F�C�g(�d��)
    matrix m; // �X�L�j���O�s��

    // �{�[��0
    iBone = input[DTid.x].vertex.boneIndex.x;
    weight = input[DTid.x].vertex.boneWeight.x;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    // �{�[��1
    iBone = input[DTid.x].vertex.boneIndex.y;
    weight = input[DTid.x].vertex.boneWeight.y;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    // �{�[��2
    iBone = input[DTid.x].vertex.boneIndex.z;
    weight = input[DTid.x].vertex.boneWeight.z;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    // �{�[��3
    iBone = input[DTid.x].vertex.boneIndex.w;
    weight = input[DTid.x].vertex.boneWeight.w;
    m = input[DTid.x].skinData.bones[iBone];
    outputData.pos += weight * mul(m, input[DTid.x].vertex.pos);
    outputData.normal += weight * mul((float3x3) m, input[DTid.x].vertex.normal);

    output[DTid.x] = outputData;

}

