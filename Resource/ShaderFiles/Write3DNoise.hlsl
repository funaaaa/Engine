
// 出力先UAV
RWTexture3D<float4> OutputImg : register(u0);


// 2Dのランダムハッシュ関数
float2 random2(float2 st)
{
    float2 seed = float2(dot(st, float2(127.1, 311.7)), dot(st, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(seed) * 43758.5453123);
}


// 2Dグラディエントノイズ関数
float noise(float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);

    // 四つの隣接点の座標を求める
    float2 u = f * f * (3.0 - 2.0 * f);

    float a = dot(random2(i), f - float2(0, 0));
    float b = dot(random2(i + float2(1, 0)), f - float2(1, 0));
    float c = dot(random2(i + float2(0, 1)), f - float2(0, 1));
    float d = dot(random2(i + float2(1, 1)), f - float2(1, 1));

    // ノイズ値を補間する
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// 2Dパーリンノイズ関数
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
    
    float2 st = threadIdx.xy / 256.0f * 40.0; // スケール調整
    int octaves = 8; // オクターブ数
    float persistence = 0.5; // 持続度
    float lacunarity = 2.0; // ラクナリティ
    float speed = 0.05; // 流れる速さ
    

    // パーリンノイズを適用
    float perlinValue = PerlinNoise(st, octaves, persistence, lacunarity) * 100.0f;
    
    // 色を保存。
    OutputImg[threadIdx] = float4(perlinValue, threadIdx.z / 256.0f, 0, 1);
    
}