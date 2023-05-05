
// �o�͐�UAV
RWTexture3D<float4> OutputImg : register(u0);


// 3D�̃����_���n�b�V���֐�
float3 random3(float3 st)
{
    float3 seed = float3(dot(st, float3(127.1, 311.7, 523.3)), dot(st, float3(269.5, 183.3, 497.5)), dot(st, float3(419.2, 371.9, 251.6)));
    return -1.0 + 2.0 * frac(sin(seed) * 43758.5453123);
}

// 3D�O���f�B�G���g�m�C�Y�֐�
float noise(float3 st)
{
    float3 i = floor(st);
    float3 f = frac(st);

    // ���̗אړ_�̍��W�����߂�
    float3 u = f * f * (3.0 - 2.0 * f);

    float a = dot(random3(i), f - float3(0, 0, 0));
    float b = dot(random3(i + float3(1, 0, 0)), f - float3(1, 0, 0));
    float c = dot(random3(i + float3(0, 1, 0)), f - float3(0, 1, 0));
    float d = dot(random3(i + float3(1, 1, 0)), f - float3(1, 1, 0));
    float e = dot(random3(i + float3(0, 0, 1)), f - float3(0, 0, 1));
    float f1 = dot(random3(i + float3(1, 0, 1)), f - float3(1, 0, 1));
    float g = dot(random3(i + float3(0, 1, 1)), f - float3(0, 1, 1));
    float h = dot(random3(i + float3(1, 1, 1)), f - float3(1, 1, 1));

    // �m�C�Y�l���Ԃ���
    float x1 = lerp(a, b, u.x);
    float x2 = lerp(c, d, u.x);
    float y1 = lerp(e, f1, u.x);
    float y2 = lerp(g, h, u.x);

    float xy1 = lerp(x1, x2, u.y);
    float xy2 = lerp(y1, y2, u.y);

    return lerp(xy1, xy2, u.z);
}

// 3D�p�[�����m�C�Y�֐�
float PerlinNoise(float3 st, int octaves, float persistence, float lacunarity)
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
    
    float3 st = threadIdx.xyz / 256.0f * 20.0; // �X�P�[������
    int octaves = 8; // �I�N�^�[�u��
    float persistence = 0.5; // �����x
    float lacunarity = 2.0; // ���N�i���e�B
    float speed = 0.05; // ����鑬��
    

    // �p�[�����m�C�Y��K�p
    float perlinValue = PerlinNoise(st, octaves, persistence, lacunarity);
    
    // �F��ۑ��B
    OutputImg[threadIdx] = float4(perlinValue, perlinValue, perlinValue, 1);
    
}