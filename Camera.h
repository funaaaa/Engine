#pragma once
#include <DirectXMath.h>
#include "Singleton.h"

using namespace DirectX;

class Camera : public Singleton<Camera> {
public:
	static XMMATRIX matView;		//�r���[�s��
	static XMFLOAT3 eye;			//�Q�[�����[���h���ł̃J�������W
	static XMFLOAT3 target;			//�Q�[�����[���h���ŃJ���������Ă�����W
	static XMFLOAT3 up;				//�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��

	static XMFLOAT3 honraiEye;		//�{������ׂ����_���W
	static XMFLOAT3 honraiTarget;	//�{������ׂ������_���W
	static XMFLOAT3 honraiUp;		//�{������ׂ���x�N�g��

	static XMMATRIX rotationMat;	//�J�����̉�]�s��
	static XMMATRIX upRotationMat;	//�J�����̏�����x�N�g���̉�]�s��

	static XMMATRIX matPerspective;
	static XMMATRIX matProjection;

	static float angleOfView;		//��p

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
	static void Init();

	//�r���[�s�񐶐�
	static void GenerateMatView();

	//�v���C���[�̃X�s�[�h�ɉ����Ď���p��ς������_�s��𐶐�����
	static void GenerateMatViewSpeed(const float& nowSpeed, const float& maxSpeed);

	//�X�V����
	void Update(const XMFLOAT3& playerPos, const XMFLOAT3& playerForwardVec, const XMFLOAT3& playerUpVec, const float& nowSpeed, const float& maxSpeed);

	//�A�N�Z�b�^
	XMFLOAT3 GetEyeVector();
};