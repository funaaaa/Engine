
// �o�͐�UAV
RWTexture3D<float4> OutputImg : register(u0);


// 2D�̃����_���n�b�V���֐�
float2 random2(float2 st)
{
    float2 seed = float2(dot(st, float2(127.1, 311.7)), dot(st, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(seed) * 43758.5453123);
}


// 2D�O���f�B�G���g�m�C�Y�֐�
float noise(float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);

    // �l�̗אړ_�̍��W�����߂�
    float2 u = f * f * (3.0 - 2.0 * f);

    float a = dot(random2(i), f - float2(0, 0));
    float b = dot(random2(i + float2(1, 0)), f - float2(1, 0));
    float c = dot(random2(i + float2(0, 1)), f - float2(0, 1));
    float d = dot(random2(i + float2(1, 1)), f - float2(1, 1));

    // �m�C�Y�l���Ԃ���
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// 2D�p�[�����m�C�Y�֐�
float PerlinNoise(float2 st, int octaves, float persistence, float lacunarity)
{
    float amplitude = 1.0;
    float frequency = 1.0;
    float sum = 0.0;
    float maxValue = 0.0;

    for (int i = 0; i < octaves; ++i)
    {
        sum += amplitude * noise(st * frequency);
        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return sum / maxValue;
}

[numthreads(8, 8, 4)]
void main(uint3 threadIdx : SV_DispatchThreadID)
{
    
    float2 st = threadIdx.xy / 256.0f * 40.0; // �X�P�[������
    int octaves = 8; // �I�N�^�[�u��
    float persistence = 0.5; // �����x
    float lacunarity = 2.0; // ���N�i���e�B
    float speed = 0.05; // ����鑬��
    

    // �p�[�����m�C�Y��K�p
    float perlinValue = PerlinNoise(st, octaves, persistence, lacunarity) * 100.0f;
    
    // �F��ۑ��B
    OutputImg[threadIdx] = float4(perlinValue, threadIdx.z / 256.0f, 0, 1);
    
}