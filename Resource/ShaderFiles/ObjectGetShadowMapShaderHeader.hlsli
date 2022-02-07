/*-----�萔1(�ϊ��s��Ǝ��_���W)-----*/
cbuffer cabuff0 : register(b0)
{
    matrix world; //���[���h�ϊ��s��
    matrix viewproj; //�r���[�v���W�F�N�V�����s��
    float4 color; //�F
    float3 eye; //���_���W
}

/*-----�萔3(�J�������C�g�s��)-----*/
cbuffer cbuff1 : register(b1)
{
    matrix lightViewProj; //���C�g�̃r���[�v���W�F�N�V�����s��
}

//���_�V�F�[�_�[����Ԃ����f�[�^�\����
struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float4 worldpos : POSITION; //���[���h���W
    float3 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD; //uv�l
};