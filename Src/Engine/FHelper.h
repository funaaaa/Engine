#pragma once
#include "WindowsAPI.h"

#include <DirectXMath.h>
#include "Vec.h"
#include <vector>

//DirectX�Ɋւ���֗��@�\���܂Ƃ߂�����
namespace FHelper {

	/*----- �֗��ϐ� -----*/

	static const int COLOR_MAX = 255;												//�F�̍ő�l
	static const Vec3 COLOR_RED_VIVID = Vec3(COLOR_MAX, 0, 0);				//RGB�U��؂�̐�
	static const Vec3 COLOR_GREEN_VIVID = Vec3(0, COLOR_MAX, 0);			//RGB�U��؂�̗�
	static const Vec3 COLOR_BLUE_VIVID = Vec3(0, 0, COLOR_MAX);				//RGB�U��؂�̐�

	static const float F_PI_F = 3.14159265359f;										//�~���� 180�x
	static const float F_PI_DIV_F = F_PI_F / 2.0f;									//�~���� 90�x
	static const float F_PI_3DIV_F = F_PI_F + F_PI_3DIV_F;							//�~���� 270�x
	static const float F_2PI_F = F_PI_F * 2.0f;										//�~���� 360�x

	/*----- �֗��֐� -----*/

	//�����擾
	inline int GetRand(const int& min, const int& max) {
		return (rand() % (max - min + 1)) + min;
	}
	inline float GetRand(const float& min, const float& max) {

		// ��U�ŏ��l�ƍő�l��10000�{����int�^�ɂ���B
		const float MUL = 10000.0f;
		int intMin = static_cast<int>(min * MUL);
		int intMax = static_cast<int>(max * MUL);

		int random = (rand() % (intMax - intMin + 1)) + intMin;

		return static_cast<float>(random) / MUL;

	}
	inline float GetRand(const float& max) {

		// ��U�ŏ��l�ƍő�l��10000�{����int�^�ɂ���B
		const float MUL = 10000.0f;
		int intMax = static_cast<int>(max * MUL);

		int random = rand() % (intMax + 1);

		return static_cast<float>(random) / MUL;

	}
	inline Vec3 GetRandVec3(const int& min, const int& max) {
		return Vec3(static_cast<float>(GetRand(min, max)), static_cast<float>(GetRand(min, max)), static_cast<float>(GetRand(min, max)));
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
	inline Vec3 WindowCenterPos() {
		return Vec3(window_width / 2.0f, window_height / 2.0f, 0);
	}

	//��ʃT�C�Y�̔���
	inline DirectX::XMFLOAT2 WindowHalfSize() {
		return DirectX::XMFLOAT2(window_width / 2.0f, window_height / 2.0f);
	}

	//�x�N�g���ɉ�]�s�����������l�𐳋K�����ĕԂ�
	inline Vec3 MulRotationMatNormal(const Vec3& formatVec, const DirectX::XMMATRIX& rotationMat) {
		Vec3 formatVecBuff = formatVec;
		//��]�s���������
		formatVecBuff = DirectX::XMVector3Transform(formatVecBuff.ConvertXMVECTOR(), rotationMat);
		//���K������
		formatVecBuff.Normalize();

		return formatVecBuff;
	}

	//��]�s��𐶐�����
	inline DirectX::XMMATRIX CalRotationMat(const Vec3& value) {
		DirectX::XMMATRIX returnBuff = DirectX::XMMatrixIdentity();
		returnBuff *= DirectX::XMMatrixRotationZ(value.z);
		returnBuff *= DirectX::XMMatrixRotationX(value.x);
		returnBuff *= DirectX::XMMatrixRotationY(value.y);
		return returnBuff;
	};

	//�w��̒l���w��̒l�ɋߕt�����l��Ԃ�
	inline float AddDivValue(const float& nowValue, const float& targetValue, const int& divCount) {
		return (targetValue - nowValue) / divCount + nowValue;
	}
	inline DirectX::XMFLOAT2 AddDivValueXMFLOAT2(const DirectX::XMFLOAT2& nowValue, const DirectX::XMFLOAT2& targetValue, const int& divCount) {
		return DirectX::XMFLOAT2((targetValue.x - nowValue.x) / divCount + nowValue.x,
			(targetValue.y - nowValue.y) / divCount + nowValue.y);
	}
	inline Vec3 AddDivValueXMFLOAT3(const Vec3& nowValue, const Vec3& targetValue, const int& divCount) {
		return Vec3(AddDivValue(nowValue.x, targetValue.x, divCount),
			AddDivValue(nowValue.y, targetValue.y, divCount),
			AddDivValue(nowValue.z, targetValue.z, divCount));
	}

	//��ʓ��Ɏ��܂��Ă��邩�𔻒�
	inline bool CheckInScreen(const Vec3& checkPos, const float& virtualWidth, const float& virtualHeight, const DirectX::XMMATRIX& matView, const DirectX::XMMATRIX& matPerspective) {
		float w = window_width / 2.0f;
		float h = window_height / 2.0f;
		DirectX::XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
		};

		// �r���[�ϊ��ƃv���W�F�N�V�����ϊ�
		Vec3 worldPos = checkPos;
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), matView);
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), matPerspective);

		Vec3 tmp;
		tmp = worldPos;
		// z�Ŋ�����-1~1�͈̔͂Ɏ��߂�
		// �X�N���[���ϊ�
		Vec3 viewVec = DirectX::XMVectorSet(tmp.x / tmp.z, tmp.y / tmp.z, 1.0f, 1.0f);
		viewVec = DirectX::XMVector3Transform(viewVec.ConvertXMVECTOR(), viewport);
		Vec3 pos = viewVec;


		if (pos.x > -virtualWidth && pos.x < window_width + virtualWidth && pos.y > -virtualHeight && pos.y < window_height + virtualHeight) {

			return true;
		}
		else {

			return false;

		}
	}

	//���C�ƃI�u�W�F�N�g�̓����蔻��
	struct RayToModelCollisionData {

		std::vector<Vec3> targetVertex;
		std::vector<UINT> targetIndex;
		std::vector<Vec3> targetNormal;
		DirectX::XMMATRIX matRot;
		DirectX::XMMATRIX matScale;
		DirectX::XMMATRIX matTrans;
		Vec3 rayPos;
		Vec3 rayDir;

	};
	bool RayToModelCollision(RayToModelCollisionData CollisionData, Vec3& ImpactPos, float& Distance, Vec3& HitNormal);

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