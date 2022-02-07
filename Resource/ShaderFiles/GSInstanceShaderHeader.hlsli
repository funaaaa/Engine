#define INSTANCE_COUNT 700

struct ConstBufferDataB0
{
    matrix world; //���[���h�ϊ��s��
    float4 color; //�F
};

/*-----�萔1(�ϊ��s��Ǝ��_���W)-----*/
cbuffer cabuff0 : register(b0)
{
    ConstBufferDataB0 cBuffer[INSTANCE_COUNT];
}

//���_�V�F�[�_�[����Ԃ����f�[�^�\����
struct VSOutput
{
    float4 pos : POSITION; //�V�X�e���p���_���W
    uint id : SV_InstanceID;
};

//�W�I���g���V�F�[�_�[����Ԃ����f�[�^�\����
struct GSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float2 uv : TEXCOORD; //uv�l
    uint id : SV_InstanceID;
};