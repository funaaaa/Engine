#pragma once
#include <DirectXMath.h>
#include "Singleton.h"
#include "Vec.h"

using namespace DirectX;

class Camera : public Singleton<Camera> {
public:
	XMMATRIX matView;		//�r���[�s��
	Vec3 eye;			//�Q�[�����[���h���ł̃J�������W
	Vec3 target;			//�Q�[�����[���h���ŃJ���������Ă�����W
	Vec3 up;				//�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��

	Vec3 forwardVec;

	Vec3 honraiEye;		//�{������ׂ����_���W
	Vec3 honraiTarget;	//�{������ׂ������_���W
	Vec3 honraiUp;		//�{������ׂ���x�N�g��

	XMMATRIX rotationMat;	//�J�����̉�]�s��
	XMMATRIX upRotationMat;	//�J�����̏�����x�N�g���̉�]�s��

	XMMATRIX matPerspective;
	XMMATRIX matProjection;

	float angleOfView;		//��p

private:

	const float EYE_PLAYER_DISTANCE = 100;			//�v���C���[�Ǝ��_�̋���
	const float TARGET_PLAYER_DISTNACE = 50;		//�v���C���[�ƒ����_�̋���
	const float TARGET_UPPER = 50;					//�^�[�Q�b�g����ɂ��炷��
#define MAX_ANGLEOFVIEW 100.0f						//�ő��p

public:

	//�R���X�g���N�^
	friend Singleton<Camera>;
	Camera();

	//������
	void Init();

	//�r���[�s�񐶐�
	void GenerateMatView();

	//�v���C���[�̃X�s�[�h�ɉ����Ď���p��ς������_�s��𐶐�����
	void GenerateMatViewSpeed(const float& nowSpeed, const float& maxSpeed);

	//�X�V����
	void UpdateRacingGame(const Vec3& playerPos, const Vec3& playerForwardVec, const Vec3& playerUpVec, const float& nowSpeed, const float& maxSpeed);
	void Update();

	// ��]
	void AddRotation(const float& RotX, const float& RotY, const float& RotZ);

	// �ړ�
	void Move(const float& Speed);
	void MoveRight(const float& Speed);

	//�A�N�Z�b�^
	Vec3 GetEyeVector();
};