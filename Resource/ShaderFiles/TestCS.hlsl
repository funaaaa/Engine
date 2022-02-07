//���ϓ_�A�ō����_�A�Œᓾ�_���v�Z����R���s���[�g�V�F�[�_�[

#define NUM_STUDENT 3.0f;   //���k�̐�

//�o�̓f�[�^�\����
struct OutputData
{
    float averageScore; //���ϓ_
    float maxScore; //�ō��_
    float minScore; //�ŏ��_
};

//���̓f�[�^�ɃA�N�Z�X���邽�߂̕ϐ�
StructuredBuffer<int> gScores : register(t0);

//�o�̓f�[�^�ɃA�N�Z�X���邽�߂̕ϐ�
RWStructuredBuffer<OutputData> gOutputData : register(u0);

//�G���g���[�|�C���g
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    gOutputData[0].maxScore = 0;
    gOutputData[0].minScore = 100;

    int totalScore = 0;

    for (int i = 0; i < 3; ++i)
    {
        totalScore += gScores[i];
        gOutputData[0].maxScore = max(gOutputData[0].maxScore, gScores[i]);
        gOutputData[0].minScore = min(gOutputData[0].minScore, gScores[i]);
    }

    gOutputData[0].averageScore = totalScore / NUM_STUDENT;

}

