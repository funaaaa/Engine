#pragma once
#include <DirectXMath.h>
#include "Singleton.h"
#include "Vec.h"

class Camera : public Singleton<Camera> {
public:
	DirectX::XMMATRIX matView;		//�r���[�s��
	Vec3 eye;			//�Q�[�����[���h���ł̃J�������W
	Vec3 target;			//�Q�[�����[���h���ŃJ���������Ă�����W
	Vec3 up;				//�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��

	Vec3 forwardVec;

	Vec3 honraiEye;		//�{������ׂ����_���W
	Vec3 honraiTarget;	//�{������ׂ������_���W
	Vec3 honraiUp;		//�{������ׂ���x�N�g��

	DirectX::XMMATRIX rotationMat;	//�J�����̉�]�s��
	DirectX::XMMATRIX upRotationMat;	//�J�����̏�����x�N�g���̉�]�s��

	DirectX::XMMATRIX matPerspective;
	DirectX::XMMATRIX matProjection;

	float angleOfView;		//��p

	float angleXZ;		// XZ���ʂł̊p�x

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
	void Update();

	// ��]
	void AddRotation(const float& RotX, const float& RotY, const float& RotZ);
	void AddRotationXZ(const float& Rot);

	// �ړ�
	void Move(const float& Speed);
	void MoveRight(const float& Speed);

	//�A�N�Z�b�^
	Vec3 GetEyeVector();
};