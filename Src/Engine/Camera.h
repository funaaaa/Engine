#pragma once
#include <DirectXMath.h>
#include "Singleton.h"
#include "Vec.h"

class Camera : public Singleton<Camera> {
public:
	DirectX::XMMATRIX matView;	// �r���[�s��
	Vec3 eye;					// �Q�[�����[���h���ł̃J�������W
	Vec3 target;				// �Q�[�����[���h���ŃJ���������Ă�����W
	Vec3 up;					// �Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��

	Vec3 forwardVec;

	Vec3 baseEye;		// ��ƂȂ鎋�_���W eye�͂��̒l�Ɍ������ĕ�Ԃ����B
	Vec3 baseTarget;	// ��ƂȂ钍���_���W target�͂��̒l�Ɍ������ĕ�Ԃ����B
	Vec3 baseUp;		// ��ƂȂ��x�N�g�� up�͂��̒l�Ɍ������ĕ�Ԃ����B

	DirectX::XMMATRIX matPerspective;
	DirectX::XMMATRIX matProjection;

	float angleOfView;		// ��p
	float baseAngleOfView;	// ��ƂȂ��p�̒l angleOfView�����̒l�Ɍ������ĕ�Ԃ���B

private:

	const float EYE_PLAYER_DISTANCE = 100;			// �v���C���[�Ǝ��_�̋���
	const float TARGET_PLAYER_DISTNACE = 50;		// �v���C���[�ƒ����_�̋���
	const float TARGET_UPPER = 50;					// �^�[�Q�b�g����ɂ��炷��
	const float DEF_ANGLEOFVIEW = 60.0f;			// ��p�̃f�t�H���g�l
	const float MAX_ANGLEOFVIEW = 100.0f;			// �ő��p

public:

	// �R���X�g���N�^
	Camera();

	// ������
	void Init();

	// �r���[�s�񐶐�
	void GenerateMatView();

	// �X�V����
	void Update(const Vec3& CharaPos, const Vec3& CharaForwardVec, const Vec3& CharaUpVec, const float& CharaSpeedPer);

};