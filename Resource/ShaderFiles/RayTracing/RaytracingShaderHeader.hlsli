
// ���_���
struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 uv;
};

// �����
struct SceneCB
{
    matrix mtxView; // �r���[�s��.
    matrix mtxProj; // �v���W�F�N�V�����s��.
    matrix mtxViewInv; // �r���[�t�s��.
    matrix mtxProjInv; // �v���W�F�N�V�����t�s��.
    float4 lightDirection; // ���s�����̌���.
    float4 lightColor; // ���s�����F.
    float4 ambientColor; // ����.
};


// �y�C���[�h �F�����擾���邽�߂̍\����
struct Payload
{
    float3 color;
    uint recursive;
};
// �y�C���[�h �e�����擾���邽�߂̍\����
struct ShadowPayload
{
    float shadowRate;
};
// �A�g���r���[�g ���������ʒu���擾���邽�߂̍\����
struct MyAttribute
{
    float2 barys;
};

// ���������ʒu���v�Z����֐�
inline float3 CalcBarycentrics(float2 barys)
{
    return float3(1.0 - barys.x - barys.y, barys.x, barys.y);
}

// ���������ʒu�����
inline float2 CalcHitAttribute2(float2 vertexAttribute[3], float2 barycentrics)
{
    float2 ret;
    ret = vertexAttribute[0];
    ret += barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]);
    ret += barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
    return ret;
}
inline float3 CalcHitAttribute3(float3 vertexAttribute[3], float2 barycentrics)
{
    float3 ret;
    ret = vertexAttribute[0];
    ret += barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]);
    ret += barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
    return ret;
}


// �����ȏ�g���[�X���Ȃ��悤�ɂ���
inline bool checkRecursiveLimit(inout Payload payload)
{
    payload.recursive++;
    if (payload.recursive >= 29)
    {
        payload.color = float3(0, 0, 0);
        return true;
    }
    return false;
}