
// 出力先UAV
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



// 3Dのランダムハッシュ関数
float3 random3(float3 st)
{
    float3 seed = float3(dot(st, float3(127.1, 311.7, 523.3)), dot(st, float3(269.5, 183.3, 497.5)), dot(st, float3(419.2, 371.9, 251.6)));
    return -1.0 + 2.0 * frac(sin(seed) * 43758.5453123);
}

// 3Dグラディエントノイズ関数
float noise(float3 st)
{
    float3 i = floor(st);
    float3 f = frac(st);

    // 八つの隣接点の座標を求める
    float3 u = f * f * (3.0 - 2.0 * f);

    float a = dot(random3(i), f - float3(0, 0, 0));
    float b = dot(random3(i + float3(1, 0, 0)), f - float3(1, 0, 0));
    float c = dot(random3(i + float3(0, 1, 0)), f - float3(0, 1, 0));
    float d = dot(random3(i + float3(1, 1, 0)), f - float3(1, 1, 0));
    float e = dot(random3(i + float3(0, 0, 1)), f - float3(0, 0, 1));
    float f1 = dot(random3(i + float3(1, 0, 1)), f - float3(1, 0, 1));
    float g = dot(random3(i + float3(0, 1, 1)), f - float3(0, 1, 1));
    float h = dot(random3(i + float3(1, 1, 1)), f - float3(1, 1, 1));

    // ノイズ値を補間する
    float x1 = lerp(a, b, u.x);
    float x2 = lerp(c, d, u.x);
    float y1 = lerp(e, f1, u.x);
    float y2 = lerp(g, h, u.x);

    float xy1 = lerp(x1, x2, u.y);
    float xy2 = lerp(y1, y2, u.y);

    return lerp(xy1, xy2, u.z);
}

// 3Dパーリンノイズ関数（風の表現付き）
float3 PerlinNoiseWithWind(float3 st, int octaves, float persistence, float lacunarity, float windStrength, float windSpeed, float t, float3 worldPos, float threshold)
{
    float amplitude = 1.0;

    // 風の影響を計算
    float3 windDirection = normalize(float3(1, 0, 0)); // 風の方向を設定（この場合は (1, 0, 0) の方向）
    float3 windEffect = windDirection * windStrength * (t * windSpeed);

    // よりスムーズな時間変数
    float smoothTime = t * 0.1; // 任意の係数（この場合は0.1）で時間変数をスケーリング

    // プレイヤーのワールド座標に基づくノイズ生成
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
            sum += localAmplitude * noise((worldSpaceCoords + windEffect) * frequency + (smoothTime + windEffect.x)); // スムーズな時間変数と風の影響をノイズ関数に適用
            maxValue += localAmplitude;

            localAmplitude *= persistence;
            frequency *= lacunarity;
        }

        noiseValue[j] = (sum / maxValue + 1.0) * 0.5; // ノイズ値を0.0から1.0の範囲に再マッピング

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
    
    //float3 st = threadIdx.xyz / 256.0f * 10.0; // スケール調整
    float3 st = threadIdx.xyz / scale_ * 10.0f; // スケール調整
    

    // パーリンノイズを適用
    float3 perlinValue = PerlinNoiseWithWind(st, octaves_, persistence_, lacunarity_, windStrength_, windSpeed_, timer_, worldPos_, threshold_);
    
    // 色を保存。
    OutputImg[threadIdx] = float4(perlinValue.x, perlinValue.y, perlinValue.z, 0.1f);
    
}