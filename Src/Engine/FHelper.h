#pragma once
#include "Struct.h"
#include "WindowsAPI.h"

//DirectX�Ɋւ���֗��@�\���܂Ƃ߂�����
namespace FHelper {

	/*----- �֗��ϐ� -----*/

	static const int COLOR_MAX = 255;												//�F�̍ő�l
	static const XMFLOAT3 COLOR_RED_VIVID = XMFLOAT3(COLOR_MAX, 0, 0);				//RGB�U��؂�̐�
	static const XMFLOAT3 COLOR_GREEN_VIVID = XMFLOAT3(0, COLOR_MAX, 0);			//RGB�U��؂�̗�
	static const XMFLOAT3 COLOR_BLUE_VIVID = XMFLOAT3(0, 0, COLOR_MAX);				//RGB�U��؂�̐�

	static const float F_PI_F = 3.14159265359;										//�~���� 180�x
	static const float F_PI_DIV_F = F_PI_F / 2.0f;									//�~���� 90�x
	static const float F_PI_3DIV_F = F_PI_F + F_PI_3DIV_F;							//�~���� 270�x
	static const float F_2PI_F = F_PI_F * 2.0f;										//�~���� 360�x

	/*----- �֗��֐� -----*/

	//2D���W���ł̓�_�Ԃ̋��������߂�
	inline float Distance2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f));
	}
	//3D���W���ł̓�_�Ԃ̋��������߂�
	inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
	}

	//���ς����߂�
	inline float Dot2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
	inline float Dot3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	//�O�ς����߂�
	inline float Cross2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}
	inline XMFLOAT3 Cross3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		//�^�C�v�~�X���Ă�\������
		return XMFLOAT3(lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x);
	}

	//�x�N�g���̒��������߂�
	inline float Length2D(const XMFLOAT2& rhs) {
		return sqrtf(Dot2D(rhs, rhs));
	}
	inline float Length3D(const XMFLOAT3& rhs) {
		return sqrtf(Dot3D(rhs, rhs));
	}

	//�p�x�����߂�
	inline float CalAngle3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return Dot3D(lhs, rhs) / (Length3D(lhs) * Length3D(rhs));
	}

	//�x�N�g���𐳋K������
	inline XMFLOAT2 Normalize2D(const XMFLOAT2& rhs) {
		float len = Length2D(rhs);
		XMFLOAT2 buff = rhs;
		buff.x /= len;
		buff.y /= len;
		return buff;
	}
	inline XMFLOAT3 Normalize3D(const XMFLOAT3& rhs) {
		float len = Length3D(rhs);
		XMFLOAT3 buff = rhs;
		buff.x /= len;
		buff.y /= len;
		buff.z /= len;
		return buff;
	}

	//�����𑫂�
	inline XMFLOAT2 AddXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	inline XMFLOAT3 AddXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	//����������
	inline XMFLOAT2 SubXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x - rhs.x, lhs.y - rhs.y);
	}
	inline XMFLOAT3 SubXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		XMFLOAT3 buff = XMFLOAT3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);

		//����Ȃ�0�ɋ߂�������0�ɂ���
		if (fabs(buff.x) < 0.00001f) {
			buff.x = 0;
		}
		if (fabs(buff.y) < 0.00001f) {
			buff.y = 0;
		}
		if (fabs(buff.z) < 0.00001f) {
			buff.z = 0;
		}

		return buff;
	}

	//������������
	inline XMFLOAT2 MulXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x * rhs.x, lhs.y * rhs.y);
	}
	inline XMFLOAT3 MulXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	//����������
	inline XMFLOAT2 DivXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x / rhs.x, lhs.y / rhs.y);
	}
	inline XMFLOAT3 DivXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

	//XMVECTOR -> XMFLOAT3
	inline XMFLOAT3 ConvertXMFLOAT3(const XMVECTOR& rhs) {
		XMFLOAT3 returnBuff;
		XMStoreFloat3(&returnBuff, rhs);
		return returnBuff;
	}

	//�����擾
	inline int GetRand(const int& min, const int& max) {
		return (rand() % (max - min + 1)) + min;
	}
	inline XMFLOAT3 GetRandXMFLOAT3(const int& min, const int& max) {
		return XMFLOAT3(GetRand(min, max), GetRand(min, max), GetRand(min, max));
	}

	//�w��̒l�̎w��̒l�͈̔͂ł�0~1�̊��������߂�
	inline float CalPercentageZeroToOne(const float& nowValue, const float& minValue, const float& maxValue) {
		float maxBuff = maxValue;
		float nowValueBuff = nowValue;
		//�ŏ��l��0�����̏ꍇ�͍ő�l�ɑ���
		if (minValue < 0) {
			maxBuff += fabs(minValue);
			nowValueBuff += fabs(minValue);
		}
		//�ŏ��l��0���傫���ꍇ�͍ő�l�������
		else if (minValue > 0) {
			maxBuff -= fabs(minValue);
			nowValueBuff -= fabs(minValue);
		}
		//���x0�̏ꍇ�͂��̂܂�
		else {
		}

		float returnBuff = nowValueBuff / maxBuff;

		//0�ȉ�1�ȏ�ɂȂ�Ȃ��悤�ɂ���
		if (returnBuff <= 0) returnBuff = 0;
		if (returnBuff >= 1) returnBuff = 1;

		return returnBuff;

	}

	//��ʒ���
	inline XMFLOAT3 WindowCenterPos() {
		return XMFLOAT3(window_width / 2.0f, window_height / 2.0f, 0);
	}

	//��ʃT�C�Y�̔���
	inline XMFLOAT2 WindowHalfSize() {
		return XMFLOAT2(window_width / 2.0f, window_height / 2.0f);
	}

	//�x�N�g���ɉ�]�s�����������l�𐳋K�����ĕԂ�
	inline XMFLOAT3 MulRotationMatNormal(const XMFLOAT3& formatVec, const XMMATRIX& rotationMat) {
		XMFLOAT3 formatVecBuff = formatVec;
		XMVECTOR formatVectorBuff = XMLoadFloat3(&formatVecBuff);
		//��]�s���������
		formatVectorBuff = XMVector3Transform(formatVectorBuff, rotationMat);
		//���K������
		formatVectorBuff = XMVector3Normalize(formatVectorBuff);
		//�x�N�g�����X�V����
		XMStoreFloat3(&formatVecBuff, formatVectorBuff);

		return formatVecBuff;
	}

	//��]�s��𐶐�����
	inline XMMATRIX CalRotationMat(const XMFLOAT3& value) {
		XMMATRIX returnBuff = XMMatrixIdentity();
		returnBuff *= XMMatrixRotationZ(value.z);
		returnBuff *= XMMatrixRotationX(value.x);
		returnBuff *= XMMatrixRotationY(value.y);
		return returnBuff;
	};

	//�x�N�g���ɉ�]�s���������
	inline XMFLOAT3 MulRotationMat(const XMFLOAT3& formatVec, const XMMATRIX& rotationMat) {
		XMFLOAT3 formatVecBuff = formatVec;
		XMVECTOR formatVectorBuff = XMLoadFloat3(&formatVecBuff);
		//��]�s���������
		formatVectorBuff = XMVector3Transform(formatVectorBuff, rotationMat);
		//�x�N�g�����X�V����
		XMStoreFloat3(&formatVecBuff, formatVectorBuff);

		return formatVecBuff;
	}

	//�w��̒l���w��̒l�ɋߕt�����l��Ԃ�
	inline float AddDivValue(const float& nowValue, const float& targetValue, const int& divCount) {
		return (targetValue - nowValue) / divCount + nowValue;
	}
	inline XMFLOAT2 AddDivValueXMFLOAT2(const XMFLOAT2& nowValue, const XMFLOAT2& targetValue, const int& divCount) {
		return XMFLOAT2((targetValue.x - nowValue.x) / divCount + nowValue.x,
			(targetValue.y - nowValue.y) / divCount + nowValue.y);
	}
	inline XMFLOAT3 AddDivValueXMFLOAT3(const XMFLOAT3& nowValue, const XMFLOAT3& targetValue, const int& divCount) {
		return XMFLOAT3(AddDivValue(nowValue.x, targetValue.x, divCount),
			AddDivValue(nowValue.y, targetValue.y, divCount),
			AddDivValue(nowValue.z, targetValue.z, divCount));
	}

	//��ʓ��Ɏ��܂��Ă��邩�𔻒�
	inline bool CheckInScreen(const XMFLOAT3& checkPos, const float& virtualWidth, const float& virtualHeight, const XMMATRIX& matView, const XMMATRIX& matPerspective) {
		float w = window_width / 2.0f;
		float h = window_height / 2.0f;
		XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
		};

		// �r���[�ϊ��ƃv���W�F�N�V�����ϊ�
		XMVECTOR worldPos = XMLoadFloat3(&checkPos);
		worldPos = XMVector3Transform(worldPos, matView);
		worldPos = XMVector3Transform(worldPos, matPerspective);

		XMFLOAT3 temp;
		XMStoreFloat3(&temp, worldPos);
		// z�Ŋ�����-1~1�͈̔͂Ɏ��߂�
		// �X�N���[���ϊ�
		XMVECTOR viewVec = XMVectorSet(temp.x / temp.z, temp.y / temp.z, 1.0f, 1.0f);
		viewVec = XMVector3Transform(viewVec, viewport);
		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, viewVec);


		if (pos.x > -virtualWidth && pos.x < window_width + virtualWidth && pos.y > -virtualHeight && pos.y < window_height + virtualHeight) {

			return true;
		}
		else {

			return false;

		}
	}
}

//�C�[�W���O�֌W
namespace FEasing {

	/*----- �C�[�W���O�֐� -----*/

	//t��0~1�͈̔�

	inline float EaseInSine(const float& time) {

		return 1.0f - cosf((time * FHelper::F_PI_F) / 2.0f);

	}

	inline float EaseInOutSine(const float& time) {
		return -(cosf(FHelper::F_PI_F * time) - 1.0f) / 2.0f;
	}

	inline float EaseOutSine(const float& time) {

		return sinf((time * FHelper::F_PI_F) / 2.0f);

	}

	inline float EaseOutCubic(const float& time) {
		return 1.0f - powf(1 - time, 3.0f);
	}

	inline float EaseOutQuar(const float& time) {
		return 1.0f - powf(1 - time, 4.0f);
	}

	inline float EaseInOutQuint(const float& time) {
		return time < 0.5f ? 16.0f * time * time * time * time * time : 1.0f - powf(-2.0f * time + 2.0f, 5.0f) / 2.0f;
	}

}