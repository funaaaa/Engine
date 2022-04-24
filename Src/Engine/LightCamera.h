#pragma once
#include "Struct.h"

// ���C�g���_�ł̃J�����N���X
class LightCamera {

private:

	/*-- �����o�ϐ� --*/

	XMFLOAT3* eye;				// ���_���W
	XMVECTOR* dir;				// �����x�N�g��

	XMFLOAT3 up;				// ��x�N�g��
	XMFLOAT3 target;			// �����_���W

	XMMATRIX rotationMat;		// ��]�s��

	XMMATRIX matView;			// �r���[�s��
	XMMATRIX matPerspective;	// �������e�s��


private:

	/*-- �萔 --*/

	const float EYE_TARGET_DISTANCE = 10.0f;	// ���_�ƒ����_�̋���


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	LightCamera();

	// ����������
	void Init();

	// ��������
	void Generate(XMFLOAT3* eyePtr, XMVECTOR* dirPtr);

	// �s�񐶐�����
	void GenerateViewMat();

	// �A�N�Z�b�^
	inline const XMFLOAT3& GetEye() { return *eye; }
	inline const XMFLOAT3& GetTarget() { return target; }
	inline const XMFLOAT3& GetUp() { return  up; }
	inline const XMMATRIX& GetPerspectiveMat() { return matPerspective; }
	inline const XMMATRIX& GetViewMat() { return matView; }

};