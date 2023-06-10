
// �o�͐�UAV
RWTexture3D<float4> OutputImg : register(u0);

cbuffer param : register(b0)
{
    float3 worldPos_;
    float timer_;
    float windSpeed_;
    float windStrength_;
    float threshold_;
    float scale_;
    int octaves_;
    float persistence_;
    float lacunarity_;
    float pad_;
};



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

// 3D�p�[�����m�C�Y�֐��i���̕\���t���j
float3 PerlinNoiseWithWind(float3 st, int octaves, float persistence, float lacunarity, float windStrength, float windSpeed, float t, float3 worldPos, float threshold)
{
    float amplitude = 1.0;

    // ���̉e�����v�Z
    float3 windDirection = normalize(float3(1, 0, 0)); // ���̕�����ݒ�i���̏ꍇ�� (1, 0, 0) �̕����j
    float3 windEffect = windDirection * windStrength * (t * windSpeed);

    // ���X���[�Y�Ȏ��ԕϐ�
    float smoothTime = t * 0.1; // �C�ӂ̌W���i���̏ꍇ��0.1�j�Ŏ��ԕϐ����X�P�[�����O

    // �v���C���[�̃��[���h���W�Ɋ�Â��m�C�Y����
    float3 worldSpaceCoords = st + worldPos / 100.0f;

    float3 noiseValue = float3(0, 0, 0);

    for (int j = 0; j < 3; ++j)
    {
        float frequency = pow(2.0, float(j));
        float localAmplitude = amplitude;
        float sum = 0.0;
        float maxValue = 0.0;
        
        for (int i = 0; i < octaves; ++i)
        {
            sum += localAmplitude * noise((worldSpaceCoords + windEffect) * frequency + (smoothTime + windEffect.x)); // �X���[�Y�Ȏ��ԕϐ��ƕ��̉e�����m�C�Y�֐��ɓK�p
            maxValue += localAmplitude;

            localAmplitude *= persistence;
            frequency *= lacunarity;
        }

        noiseValue[j] = (sum / maxValue + 1.0) * 0.5; // �m�C�Y�l��0.0����1.0�͈̔͂ɍă}�b�s���O

        if (noiseValue[j] <= threshold)
        {
            noiseValue[j] = 0.0;
        }
    }

    return noiseValue;
}

[numthreads(8, 8, 4)]
void main(uint3 threadIdx : SV_DispatchThreadID)
{
    
    //float3 st = threadIdx.xyz / 256.0f * 10.0; // �X�P�[������
    float3 st = threadIdx.xyz / scale_ * 10.0f; // �X�P�[������
    

    // �p�[�����m�C�Y��K�p
    float3 perlinValue = PerlinNoiseWithWind(st, octaves_, persistence_, lacunarity_, windStrength_, windSpeed_, timer_, worldPos_, threshold_);
    
    // �F��ۑ��B
    OutputImg[threadIdx] = float4(perlinValue.x, perlinValue.y, perlinValue.z, 0.1f);
    
}