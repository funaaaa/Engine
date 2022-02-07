#include "ObjectInstanceShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳��ăe�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(1,1,1,1);

    float4 shadeColor = float4(0, 0, 0, 1); //�ŏI�I�ɕ`�悷��e�̐F

    //���s����
    for (int i = 0; i < DIRECTIONLIGHT_NUM; ++i)
    {
        if (cbuffData2.dirLights[i].active == 1)
        {
			//�����ˌ��̌v�Z
            float3 ambient = mAmbient;
			//�g�U���ˌ��̌v�Z
            float3 diffuse = dot(-cbuffData2.dirLights[i].lightv, input.normal) * mDiffuse;
			//����x
            const float shininess = 4.0f;
			//���_���王�_�ւ̕����x�N�g��
            float3 eyedir = normalize(cbuff0Data[input.instanceID].eye - input.worldpos.xyz);
			//���ˌ��x�N�g��
            float3 reflect = normalize(cbuffData2.dirLights[i].lightv + 2 * dot(-cbuffData2.dirLights[i].lightv, input.normal) * input.normal);
			//���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//���ׂĉ��Z�����F
            float3 color = (ambient + diffuse + specular) * cbuffData2.dirLights[i].lightcolor;
            float3 outputColor = color;
            shadeColor.rgb += outputColor;
        }
    }
	//�_����
    for (int i = 0; i < POINTLIGHT_NUM; ++i)
    {
        if (cbuffData2.pointLights[i].active == 1)
        {
			//���C�g�̃x�N�g��
            float3 lightv = cbuffData2.pointLights[i].lightpos - input.worldpos.xyz;
			//�x�N�g���̒���
            float d = length(lightv);
			//���K�����ĒP�ʃx�N�g���ɂ���
            lightv = normalize(lightv);
			//���������W��
            float atten = 1.0f / (cbuffData2.pointLights[i].lightatten.x +
				cbuffData2.pointLights[i].lightatten.y * d +
				cbuffData2.pointLights[i].lightatten.z * (d * d));
            atten *= 1000;
			//���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, input.normal);
			//���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			//�g�U���ˌ�
            float3 diffuse = dotlightnormal * mDiffuse;
			//����x
            const float shininess = 4.0f;
			//���_���王�_�ւ̕����x�N�g��
            float3 eyedir = normalize(cbuff0Data[input.instanceID].eye - input.worldpos.xyz);
			//���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//���ׂĉ��Z�����F
            float3 outputColor = saturate(atten * (diffuse + specular) * cbuffData2.pointLights[i].lightcolor);
            shadeColor.rgb += outputColor;
        }
    }
    //�X�|�b�g���C�g
    for (int i = 0; i < SPOTLIGHT_NUM; ++i)
    {
        if (cbuffData2.spotlights[i].active == 1)
        {
			//���C�g�ւ̕����x�N�g��
            float3 lightv = cbuffData2.spotlights[i].lightpos - input.worldpos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
			//���������W��
            float atten = saturate(1.0f / (cbuffData2.spotlights[i].lightatten.x +
				cbuffData2.spotlights[i].lightatten.y * d +
				cbuffData2.spotlights[i].lightatten.z * d * d));
			//�p�x����
            float cos = dot(lightv, cbuffData2.spotlights[i].lightv);
			//�����J�n�p�x����A�����I���p�x�ɂ����Č���
			//�����J�n�p�x�̓�����1�{ �����I���p�x�̊O����0�{�̋P�x
            float angleatten = smoothstep(cbuffData2.spotlights[i].lightfactoranglecos.y, cbuffData2.spotlights[i].lightfactoranglecos.x, -cos);
			//�p�x��������Z
            atten *= angleatten;
			//���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, input.normal);
			//���_���王�_�ւ̕����x�N�g��
            float3 eyedir = normalize(cbuff0Data[input.instanceID].eye - input.worldpos.xyz);
			//����x
            const float shininess = 4.0f;
			//���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			//�g�U���ˌ�
            float3 diffuse = dotlightnormal * mDiffuse;
			//���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * mSpecular;
			//���ׂĉ��Z
            float3 outputColor = atten * (diffuse + specular) * cbuffData2.spotlights[i].lightcolor;
            shadeColor.rgb += outputColor;
        }
    }
    
    //�Ԑ���
    float buff = 0.35f;
    float buff2 = 0.65f;
    float buff3 = 1.0f;
    if (shadeColor.r < buff)
    {
        shadeColor.r = buff;
    }
    else if (shadeColor.r < buff2)
    {
        shadeColor.r = buff2;
    }
    else
    {
        shadeColor.r = buff3;
    }
    
    //�ΐ���
    if (shadeColor.g < buff)
    {
        shadeColor.g = buff;
    }
    else if (shadeColor.g < buff2)
    {
        shadeColor.g = buff2;
    }
    else
    {
        shadeColor.g = buff3;
    }
    
    //����
    if (shadeColor.b < buff)
    {
        shadeColor.b = buff;
    }
    else if (shadeColor.b < buff2)
    {
        shadeColor.b = buff2;
    }
    else
    {
        shadeColor.b = buff3;
    }

	//�e�N�X�`���̐F
    float4 texcolor = tex.Sample(smp, input.uv);
    return (shadeColor * texcolor) * cbuff0Data[input.instanceID].color;

}