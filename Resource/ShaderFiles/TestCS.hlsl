//平均点、最高得点、最低得点を計算するコンピュートシェーダー

#define NUM_STUDENT 3.0f;   //生徒の数

//出力データ構造体
struct OutputData
{
    float averageScore; //平均点
    float maxScore; //最高点
    float minScore; //最小点
};

//入力データにアクセスするための変数
StructuredBuffer<int> gScores : register(t0);

//出力データにアクセスするための変数
RWStructuredBuffer<OutputData> gOutputData : register(u0);

//エントリーポイント
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

