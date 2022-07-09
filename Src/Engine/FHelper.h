#pragma once
#include "WindowsAPI.h"

#include <DirectXMath.h>
#include "Vec.h"
#include <vector>

//DirectX�Ɋւ���֗��@�\���܂Ƃ߂�����
namespace FHelper {

	/*----- �֗��ϐ� -----*/

	static const int COLOR_MAX = 255;										// �F�̍ő�l
	static const Vec3 COLOR_RED_VIVID = Vec3(COLOR_MAX, 0, 0);				// RGB�U��؂�̐�
	static const Vec3 COLOR_GREEN_VIVID = Vec3(0, COLOR_MAX, 0);			// RGB�U��؂�̗�
	static const Vec3 COLOR_BLUE_VIVID = Vec3(0, 0, COLOR_MAX);				// RGB�U��؂�̐�

	static const float F_PI_F = 3.14159265359f;								// �~���� 180�x
	static const float F_PI_DIV_F = F_PI_F / 2.0f;							// �~���� 90�x
	static const float F_PI_3DIV_F = F_PI_F + F_PI_3DIV_F;					// �~���� 270�x
	static const float F_2PI_F = F_PI_F * 2.0f;								// �~���� 360�x

	/*----- �֗��֐� -----*/

	// �����擾
	inline int GetRand(const int& Min, const int& Max) {
		return (rand() % (Max - Min + 1)) + Min;
	}
	inline float GetRand(const float& Min, const float& Max) {

		// ��U�ŏ��l�ƍő�l��10000�{����int�^�ɂ���B
		const float MUL = 10000.0f;
		int intMin = static_cast<int>(Min * MUL);
		int intMax = static_cast<int>(Max * MUL);

		int random = (rand() % (intMax - intMin + 1)) + intMin;

		return static_cast<float>(random) / MUL;

	}
	inline float GetRand(const float& Max) {

		// ��U�ŏ��l�ƍő�l��10000�{����int�^�ɂ���B
		const float MUL = 10000.0f;
		int intMax = static_cast<int>(Max * MUL);

		int random = rand() % (intMax + 1);

		return static_cast<float>(random) / MUL;

	}
	inline Vec3 GetRandVec3(const int& Min, const int& Max) {
		return Vec3(static_cast<float>(GetRand(Min, Max)), static_cast<float>(GetRand(Min, Max)), static_cast<float>(GetRand(Min, Max)));
	}

	// �w��̒l�̎w��̒l�͈̔͂ł�0~1�̊��������߂�
	inline float CalPercentageZeroToOne(const float& NowValue, const float& MinValue, const float& MaxValue) {
		float maxBuff = MaxValue;
		float nowValueBuff = NowValue;
		// �ŏ��l��0�����̏ꍇ�͍ő�l�ɑ���
		if (MinValue < 0) {
			maxBuff += fabs(MinValue);
			nowValueBuff += fabs(MinValue);
		}
		// �ŏ��l��0���傫���ꍇ�͍ő�l�������
		else if (0 < MinValue) {
			maxBuff -= fabs(MinValue);
			nowValueBuff -= fabs(MinValue);
		}
		// ���x0�̏ꍇ�͂��̂܂�
		else {
		}

		float returnBuff = nowValueBuff / maxBuff;

		// 0�ȉ�1�ȏ�ɂȂ�Ȃ��悤�ɂ���
		if (returnBuff <= 0) returnBuff = 0;
		if (1 <= returnBuff) returnBuff = 1;

		return returnBuff;

	}

	// ��ʒ���
	inline Vec3 WindowCenterPos() {
		return Vec3(static_cast<float>(window_width) / 2.0f, static_cast<float>(window_height) / 2.0f, 0);
	}

	// ��ʃT�C�Y�̔���
	inline Vec2 WindowHalfSize() {
		return Vec2(static_cast<float>(window_width) / 2.0f, static_cast<float>(window_height) / 2.0f);
	}

	// �x�N�g���ɉ�]�s�����������l�𐳋K�����ĕԂ�
	inline Vec3 MulRotationMatNormal(const Vec3& FormatVec, const DirectX::XMMATRIX& RotationMat) {
		Vec3 formatVecBuff = FormatVec;
		// ��]�s���������
		formatVecBuff = DirectX::XMVector3Transform(formatVecBuff.ConvertXMVECTOR(), RotationMat);
		// ���K������
		formatVecBuff.Normalize();

		return formatVecBuff;
	}
	// �x�N�g���ɍs����������l��Ԃ��B
	inline Vec3 MulMat(Vec3 Vec, const DirectX::XMMATRIX& Mat) {
		return DirectX::XMVector3Transform(Vec.ConvertXMVECTOR(), Mat);
	}

	// ��]�s��𐶐�����
	inline DirectX::XMMATRIX CalRotationMat(const Vec3& Value) {
		DirectX::XMMATRIX returnBuff = DirectX::XMMatrixIdentity();
		returnBuff *= DirectX::XMMatrixRotationZ(Value.z);
		returnBuff *= DirectX::XMMatrixRotationX(Value.x);
		returnBuff *= DirectX::XMMatrixRotationY(Value.y);
		return returnBuff;
	};

	// ��ʓ��Ɏ��܂��Ă��邩�𔻒�
	inline bool CheckInScreen(const Vec3& CheckPos, const float& VirtualWidth, const float& VirtualHeight, const DirectX::XMMATRIX& MatView, const DirectX::XMMATRIX& MatPerspective) {
		float w = window_width / 2.0f;
		float h = window_height / 2.0f;
		DirectX::XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
		};

		// �r���[�ϊ��ƃv���W�F�N�V�����ϊ�
		Vec3 worldPos = CheckPos;
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), MatView);
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), MatPerspective);

		Vec3 tmp;
		tmp = worldPos;
		// z�Ŋ�����-1~1�͈̔͂Ɏ��߂�
		// �X�N���[���ϊ�
		Vec3 viewVec = DirectX::XMVectorSet(tmp.x / tmp.z, tmp.y / tmp.z, 1.0f, 1.0f);
		viewVec = DirectX::XMVector3Transform(viewVec.ConvertXMVECTOR(), viewport);
		Vec3 pos = viewVec;


		if (-VirtualWidth < pos.x && pos.x < window_width + VirtualWidth && -VirtualHeight < pos.y && pos.y < window_height + VirtualHeight) {

			return true;
		}
		else {

			return false;

		}
	}

	// ���C�ƃI�u�W�F�N�g�̓����蔻��
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

	inline float EaseInSine(const float& Time) {

		return 1.0f - cosf((Time * FHelper::F_PI_F) / 2.0f);

	}

	inline float EaseInOutSine(const float& Time) {
		return -(cosf(FHelper::F_PI_F * Time) - 1.0f) / 2.0f;
	}

	inline float EaseOutSine(const float& Time) {

		return sinf((Time * FHelper::F_PI_F) / 2.0f);

	}

	inline float EaseOutCubic(const float& Time) {
		return 1.0f - powf(1 - Time, 3.0f);
	}

	inline float EaseOutQuar(const float& Time) {
		return 1.0f - powf(1 - Time, 4.0f);
	}

	inline float EaseInOutQuint(const float& Time) {
		return Time < 0.5f ? 16.0f * Time * Time * Time * Time * Time : 1.0f - powf(-2.0f * Time + 2.0f, 5.0f) / 2.0f;
	}

}