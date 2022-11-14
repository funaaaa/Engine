// 入力UAV
RWTexture2D<float4> InputImg : register(u0);

// 出力先UAV
RWTexture2D<float4> OutputImg : register(u1);

struct UVSet
{
    float2 uv_;
    float2 prevuv_;
};

struct CarUVSet
{
    UVSet forwardLeft_;
    UVSet forwardRight_;
    UVSet behindLeft_;
    UVSet behindRight_;
};

// 書き換える場所指定定数バッファ
cbuffer WriteUV : register(b0)
{
    CarUVSet playerCar_;
    CarUVSet aiCar_;
};

// 書き込む関数
void WriteColor(uint2 WriteUV, float4 color)
{
    // アルファ値が1じゃなかったら
    //if (length(OutputImg[WriteUV].xyz) != 0)
    //{
        OutputImg[WriteUV] = color;
    //}
}

// 書き込む
void WriteOutputImg(uint2 WriteUV)
{
    
    OutputImg[uint2(WriteUV.x, WriteUV.y)] = float4(0, 0, 0, 0.6);
    
    // 上下左右に薄く出す。
    WriteColor(uint2(WriteUV.x + 1, WriteUV.y + 1), float4(0, 0, 0, 0.6));
    WriteColor(uint2(WriteUV.x - 1, WriteUV.y + 1), float4(0, 0, 0, 0.6));
    WriteColor(uint2(WriteUV.x - 1, WriteUV.y - 1), float4(0, 0, 0, 0.6));
    WriteColor(uint2(WriteUV.x + 1, WriteUV.y - 1), float4(0, 0, 0, 0.6));
    WriteColor(uint2(WriteUV.x, WriteUV.y + 1), float4(0, 0, 0, 0.6));
    WriteColor(uint2(WriteUV.x, WriteUV.y - 1), float4(0, 0, 0, 0.6));
    WriteColor(uint2(WriteUV.x - 1, WriteUV.y), float4(0, 0, 0, 0.6));
    WriteColor(uint2(WriteUV.x + 1, WriteUV.y), float4(0, 0, 0, 0.6));
    
    
};

// 前フレームのUVといまフレームのUVの値を補間して書き込む。
void Interpolation(uint2 WriteUV, uint2 PrevWriteUV)
{
    
    if (WriteUV.x == 0 || WriteUV.y == 0 || PrevWriteUV.x == 0 || PrevWriteUV.y == 0)
    {
        return;
    }
    
    // 補間するの長さを求める。
    float interpolationLength = length(WriteUV - PrevWriteUV);
    
    // 補間するベクトルを求める。
    float2 interpolationVec = normalize(WriteUV - PrevWriteUV);
    
    // 終点と始点を書き込む。
    WriteOutputImg(WriteUV);
    WriteOutputImg(PrevWriteUV);
    
    // 中間地点にも書き込む。
    WriteOutputImg(WriteUV * 0.1f + PrevWriteUV * 0.9f);
    WriteOutputImg(WriteUV * 0.2f + PrevWriteUV * 0.8f);
    WriteOutputImg(WriteUV * 0.3f + PrevWriteUV * 0.7f);
    WriteOutputImg(WriteUV * 0.4f + PrevWriteUV * 0.6f);
    WriteOutputImg(WriteUV * 0.5f + PrevWriteUV * 0.5f);
    WriteOutputImg(WriteUV * 0.6f + PrevWriteUV * 0.4f);
    WriteOutputImg(WriteUV * 0.7f + PrevWriteUV * 0.3f);
    WriteOutputImg(WriteUV * 0.8f + PrevWriteUV * 0.2f);
    WriteOutputImg(WriteUV * 0.9f + PrevWriteUV * 0.1f);
    
    //if (1000000 < interpolationLength)
    //{
    //    OutputImg[uint2(WriteUV.x, WriteUV.y)] = float4(1, 0, 0, 1);
    //}
    //else
    //{
    
    //    // 補間して書き込む。
    //    for (int index = 0; index < interpolationLength; ++index)
    //    {
        
    //        WriteOutputImg(PrevWriteUV + uint2(interpolationVec * index));
        
    //    }
        
    //}
    
};

[numthreads(2, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    if (DTid.x == 0)
    {
    
        /*== プレイヤーの車 ==*/
    
        // 一個目
        Interpolation(uint2((uint) (playerCar_.forwardLeft_.uv_.x * 4096.0f), (uint) (playerCar_.forwardLeft_.uv_.y * 4096.0f)), uint2((uint) (playerCar_.forwardLeft_.prevuv_.x * 4096.0f), (uint) (playerCar_.forwardLeft_.prevuv_.y * 4096.0f)));
    
        // 二個目
        Interpolation(uint2((uint) (playerCar_.forwardRight_.uv_.x * 4096.0f), (uint) (playerCar_.forwardRight_.uv_.y * 4096.0f)), uint2((uint) (playerCar_.forwardRight_.prevuv_.x * 4096.0f), (uint) (playerCar_.forwardRight_.prevuv_.y * 4096.0f)));
     
        // 三個目
        //Interpolation(uint2((uint) (playerCar_.behindLeft_.uv_.x * 4096.0f), (uint) (playerCar_.behindLeft_.uv_.y * 4096.0f)), uint2((uint) (playerCar_.behindLeft_.prevuv_.x * 4096.0f), (uint) (playerCar_.behindLeft_.prevuv_.y * 4096.0f)));
    
        // 四個目
        //Interpolation(uint2((uint) (playerCar_.behindRight_.uv_.x * 4096.0f), (uint) (playerCar_.behindRight_.uv_.y * 4096.0f)), uint2((uint) (playerCar_.behindRight_.prevuv_.x * 4096.0f), (uint) (playerCar_.behindRight_.prevuv_.y * 4096.0f)));
    
    }
    else if (DTid.x == 1)
    {
    
        /*== プレイヤーの車 ==*/
    
        // 一個目
        Interpolation(uint2((uint) (aiCar_.forwardLeft_.uv_.x * 4096.0f), (uint) (aiCar_.forwardLeft_.uv_.y * 4096.0f)), uint2((uint) (aiCar_.forwardLeft_.prevuv_.x * 4096.0f), (uint) (aiCar_.forwardLeft_.prevuv_.y * 4096.0f)));
    
        // 二個目
        Interpolation(uint2((uint) (aiCar_.forwardRight_.uv_.x * 4096.0f), (uint) (aiCar_.forwardRight_.uv_.y * 4096.0f)), uint2((uint) (aiCar_.forwardRight_.prevuv_.x * 4096.0f), (uint) (aiCar_.forwardRight_.prevuv_.y * 4096.0f)));
     
        // 三個目
       /// Interpolation(uint2((uint) (aiCar_.behindLeft_.uv_.x * 4096.0f), (uint) (aiCar_.behindLeft_.uv_.y * 4096.0f)), uint2((uint) (aiCar_.behindLeft_.prevuv_.x * 4096.0f), (uint) (aiCar_.behindLeft_.prevuv_.y * 4096.0f)));
    
        // 四個目
        //Interpolation(uint2((uint) (aiCar_.behindRight_.uv_.x * 4096.0f), (uint) (aiCar_.behindRight_.uv_.y * 4096.0f)), uint2((uint) (aiCar_.behindRight_.prevuv_.x * 4096.0f), (uint) (aiCar_.behindRight_.prevuv_.y * 4096.0f)));
       
    }
    
}