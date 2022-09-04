#pragma once
#include <DirectXMath.h>
#include "Singleton.h"
#include "Vec.h"

class Camera : public Singleton<Camera> {

public:

	DirectX::XMMATRIX matView_;	// �r���[�s��
	Vec3 eye_;					// �Q�[�����[���h���ł̃J�������W
	Vec3 target_;				// �Q�[�����[���h���ŃJ���������Ă�����W
	Vec3 up_;					// �Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��

	Vec3 baseEye_;		// ��ƂȂ鎋�_���W eye�͂��̒l�Ɍ������ĕ�Ԃ����B
	Vec3 baseTarget_;	// ��ƂȂ钍���_���W target�͂��̒l�Ɍ������ĕ�Ԃ����B
	Vec3 baseUp_;		// ��ƂȂ��x�N�g�� up�͂��̒l�Ɍ������ĕ�Ԃ����B

	DirectX::XMMATRIX matPerspective_;
	DirectX::XMMATRIX matProjection_;

	float angleOfView_;		// ��p
	float baseAngleOfView_;	// ��ƂȂ��p�̒l angleOfView�����̒l�Ɍ������ĕ�Ԃ���B

	int convTimer_;		// �Q�[�����n�܂��Ă���̃t���[�����v�Z����B

private:

	const float BEFORE_START_CAMERA_DISTANCE = 100;	// �J�n�O�̃J�������o�̍ۂ̃v���C���[�Ǝ��_�̋���
	const float EYE_PLAYER_DISTANCE = 150;			// �v���C���[�Ǝ��_�̋���
	const float TARGET_PLAYER_DISTNACE = 50;		// �v���C���[�ƒ����_�̋���
	const float TARGET_UPPER = 40;					// �^�[�Q�b�g����ɂ��炷��
	const float DEF_ANGLEOFVIEW = 60.0f;			// ��p�̃f�t�H���g�l
	const float MAX_ANGLEOFVIEW = 120.0f;			// �ő��p

public:

	// �R���X�g���N�^
	Camera();

	// ������
	void Init();

	// �r���[�s�񐶐�
	void GenerateMatView();

	// �X�V����
	void Update(const Vec3& CharaPos, const Vec3& CharaForwardVec, const Vec3& CharaUpVec, const float& CharaSpeedPer, const bool& IsBeforeStart, const bool& IsGameFinish);

};