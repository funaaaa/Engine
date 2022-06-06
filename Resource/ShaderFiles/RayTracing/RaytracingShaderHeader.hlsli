
// �~����
static const float PI = 3.141592653589f;

// ���_���
struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 uv;
};

// �_�����̏��
struct PointLightData
{
    float3 lightPos;
    float lightSize;
    float3 lightColor;
    float lightPower;
    int isActive;
    float3 pad;
};

// ���s�����̏��
struct DirLightData
{
    float3 lightDir;
    int isActive;
    float3 lightColor;
    float pad;
};

// �X�|�b�g���C�g�̏��
struct SpotLightData
{
    float3 pos;
    float angle;
    float3 dir;
    float power;
    float3 color;
    int isActive;
};

// �����
struct SceneCB
{
    matrix mtxView; // �r���[�s��.
    matrix mtxProj; // �v���W�F�N�V�����s��.
    matrix mtxViewInv; // �r���[�t�s��.
    matrix mtxProjInv; // �v���W�F�N�V�����t�s��.
    float4 ambientColor; // ����.
    DirLightData dirLight;
    PointLightData pointLight;
    SpotLightData spotLight;
    int seed;
    int counter;
    int aoSampleCount;
    int isNoiseScene; // �m�C�Y��`�悷��t���O
    int isLightHitScene; // ���C�g�ɂ��������ʂ����`�悷��t���O
    int isNormalScene; // �@������`�悷��t���O
    int isMeshScene; // �|���S������`�悷��t���O
    int isNoAO;
    int isNoGI; // GI���s��Ȃ��t���O
    int isGIOnlyScene;
};


// �y�C���[�h �F�����擾���邽�߂̍\����
struct Payload
{
    float3 color;
    uint recursive;
};
// �y�C���[�h AO�f�m�C�U�p
struct DenoisePayload
{
    float3 color;
    float3 aoLuminance;
    float3 lightLuminance;
    float3 giColor;
    uint recursive;
};
// �y�C���[�h �e�����擾���邽�߂̍\����
struct ShadowPayload
{
    bool isShadow;
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
inline bool checkRecursiveLimit(inout Payload payload, int max)
{
    ++payload.recursive;
    if (max < payload.recursive)
    {
        //payload.color = float3(0, 0, 0);
        return true;
    }
    return false;
}
inline bool checkRecursiveLimitDenoiseAO(inout DenoisePayload payload)
{
    ++payload.recursive;
    if (1 < payload.recursive)
    {
        //payload.color = float3(0, 0, 0);
        return true;
    }
    return false;
}

// ��������
uint randomU(float2 uv)
{
    float r = dot(uv, float2(127.1, 311.7));
    return uint(12345 * frac(sin(r) * 43758.5453123));
}
float nextRand(inout uint s)
{
    s = (1664525u * s + 1013904223u);
    return float(s & 0x00FFFFFF) / float(0x01000000);
}

float3x3 angleAxis3x3(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return float3x3(
        t * x * x + c, t * x * y - s * z, t * x * z + s * y,
        t * x * y + s * z, t * y * y + c, t * y * z - s * x,
        t * x * z - s * y, t * y * z + s * x, t * z * z + c
        );
}

// �\�t�g�V���h�E�p�ɗՎ��Ŏ����Ă��������B
float3 GetConeSample(inout uint randSeed, float3 direction, float coneAngle)
{
    float cosAngle = cos(coneAngle);
    const float PI = 3.1415926535;

    // Generate points on the spherical cap around the north pole [1].
    // [1] See https://math.stackexchange.com/a/205589/81266
    float z = nextRand(randSeed) * (1.0f - cosAngle) + cosAngle;
    float phi = nextRand(randSeed) * 2.0f * PI;

    float x = sqrt(1.0f - z * z) * cos(phi);
    float y = sqrt(1.0f - z * z) * sin(phi);
    float3 north = float3(0.f, 0.f, 1.f);

    // Find the rotation axis `u` and rotation angle `rot` [1]
    float3 axis = normalize(cross(north, normalize(direction)));
    float angle = acos(dot(normalize(direction), north));

    // Convert rotation axis and angle to 3x3 rotation matrix [2]
    float3x3 R = angleAxis3x3(angle, axis);

    return mul(R, float3(x, y, z));
}

float3 GetPerpendicularVector(float3 u)
{
    float3 a = abs(u);
    uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
    uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
    uint zm = 1 ^ (xm | ym);
    return cross(u, float3(xm, ym, zm));
}

// �@����Ŕ�����̃����_���ȃx�N�g���𐶐��B
float3 GetCosHemisphereSample(uint randSeed, float3 hitNorm)
{
    // 2�̃����_���Ȑ��l���擾
    float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));
    // �@���ɐ����ȃx�N�g�������i�Ō�ɗ��p����j
    float3 bitangent = GetPerpendicularVector(hitNorm);
    float3 tangent = cross(bitangent, hitNorm);
    // �f�B�X�N��Ɉ�l�ɃT���v�����O
    float r = sqrt(randVal.x);
    float phi = 2.0f * 3.14f * randVal.y;
    // �����Ɏˉe����
    float x = r * cos(phi);
    float z = r * sin(phi);
    float y = sqrt(1.0 - randVal.x); // 1- r2
    // �@���x�N�g���̍��W�n�Ɏˉe
    return x * tangent + y * hitNorm.xyz + z * bitangent;
}

float3 GetUniformHemisphereSample(inout uint randSeed, float3 hitNorm)
{
    // 2�̃����_���Ȑ��l���擾
    float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));
    // �@���ɐ����ȃx�N�g�������i�Ō�ɗ��p����j
    float3 bitangent = GetPerpendicularVector(hitNorm);
    float3 tangent = cross(bitangent, hitNorm);
    // �ƁA�������߂�
    float cosTheta = randVal.x;
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    float phi = 2.0f * 3.14f * randVal.y;
    // �����Ɏˉe����
    float x = sinTheta * cos(phi);
    float z = sinTheta * sin(phi);
    float y = cosTheta;
    // �@���x�N�g���̍��W�n�Ɏˉe
    return x * tangent + y * hitNorm.xyz + z * bitangent;
}

// �V���h�E���C����
bool ShootShadowRay(float3 origin, float3 direction, float tMax, RaytracingAccelerationStructure gRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = origin;
    rayDesc.Direction = direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = tMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    //flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_NON_OPAQUE; // AnyHitShader�X�L�b�v���Ȃ�
    
    // ���C�g�͏��O�B
    uint rayMask = ~(0x08);

    TraceRay(
    gRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // MISS�V�F�[�_�[�̃C���f�b�N�X
    rayDesc,
    payload);

    return payload.isShadow;
}
bool ShootShadowRayNoAH(float3 origin, float3 direction, float tMax, RaytracingAccelerationStructure gRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = origin;
    rayDesc.Direction = direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = tMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    //flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�X�L�b�v
    
    // ���C�g�͏��O�B
    uint rayMask = ~(0x08);

    TraceRay(
    gRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // MISS�V�F�[�_�[�̃C���f�b�N�X
    rayDesc,
    payload);

    return payload.isShadow;
}

// �V���h�E���C����
bool ShootAOShadowRay(float3 origin, float3 direction, float tMax, RaytracingAccelerationStructure gRtScene)
{
    RayDesc rayDesc;
    rayDesc.Origin = origin;
    rayDesc.Direction = direction;
    rayDesc.TMin = 0.1f;
    rayDesc.TMax = tMax;

    ShadowPayload payload;
    payload.isShadow = false;

    RAY_FLAG flags = RAY_FLAG_NONE;
    //flags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    flags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    flags |= RAY_FLAG_FORCE_OPAQUE; // AnyHitShader�X�L�b�v
    
    // ���C�g�͏��O�B
    uint rayMask = ~(0x08);

    TraceRay(
    gRtScene,
    flags,
    rayMask,
    0,
    1,
    1, // MISS�V�F�[�_�[�̃C���f�b�N�X
    rayDesc,
    payload);

    return payload.isShadow;
}

uint initRand(uint val0, uint val1, uint backoff = 16)
{
    uint v0 = val0, v1 = val1, s0 = 0;

	[unroll]
    for (uint n = 0; n < backoff; n++)
    {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }
    return v0;
}