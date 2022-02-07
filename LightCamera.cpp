#include "LightCamera.h"
#include "FHelper.h"

LightCamera::LightCamera()
{
}

void LightCamera::Init()
{
	eye = nullptr;
	dir = nullptr;
}

void LightCamera::Generate(XMFLOAT3* eyePtr, XMVECTOR* dirPtr)
{

	/*-- �������� --*/

	// null�`�F�b�N
	if (eyePtr == nullptr || dirPtr == nullptr) {

		MessageBoxA(nullptr, "eyePtr or dirPtr ��nullptr�ł��BLight����������Ă��邩���`�F�b�N���Ă��������B", "�G���[", MB_OK);

		assert(0);

	}

	// �����Ŏw�肳�ꂽ���C�g�ƍ��W�ƕ����x�N�g����R�t����B
	eye = eyePtr;
	dir = dirPtr;

	// ��]�s������߂�B
	rotationMat = XMMatrixIdentity();

	//�f�t�H���g�̐��ʃx�N�g��
	XMFLOAT3 defUpVec = { 0,0,1 };

	//���t���[���̏�x�N�g��
	XMFLOAT3 nowUpVec;
	XMStoreFloat3(&nowUpVec, *dir);

	//��]�������߂�
	XMFLOAT3 axisOfRevolutionBuff = FHelper::Cross3D(defUpVec, nowUpVec);

	//��]���𐳋K������
	if (!(axisOfRevolutionBuff.x == 0 && axisOfRevolutionBuff.y == 0 && axisOfRevolutionBuff.z == 0)) {
		axisOfRevolutionBuff = FHelper::Normalize3D(axisOfRevolutionBuff);
	}

	//��]����XMVECTOR�Ɏ���
	XMVECTOR axisOfRevolution = XMLoadFloat3(&axisOfRevolutionBuff);

	/*-- ���ς����]�ʂ����߂� --*/

	//��]�ʂ����߂�
	float amountOfRotation = FHelper::CalAngle3D(defUpVec, nowUpVec);

	//�t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ�
	amountOfRotation = acosf(amountOfRotation);


	/*-- �N�H�[�^�j�I�����g���ĉ�]������ --*/

	//��]����{0,0,0}�������珈�����΂�
	if (!(axisOfRevolution.m128_f32[0] == 0 && axisOfRevolution.m128_f32[1] == 0 && axisOfRevolution.m128_f32[2] == 0)) {

		//�N�H�[�^�j�I�������߂�
		XMVECTOR quaternion = XMQuaternionRotationAxis(axisOfRevolution, amountOfRotation);

		//���߂��N�H�[�^�j�I�����s��Ɏ���
		rotationMat = XMMatrixRotationQuaternion(quaternion);

	}

	// ��]�s������ƂɁA��x�N�g�������߂�B
	up = FHelper::MulRotationMatNormal(XMFLOAT3(0, 1, 0), rotationMat);

	// �����_���W�����߂�B
	const XMFLOAT3 eyeTargetDistance = XMFLOAT3(EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE);
	target = FHelper::AddXMFLOAT3(*eye, FHelper::MulXMFLOAT3(XMFLOAT3(dir->m128_f32[0], dir->m128_f32[1], dir->m128_f32[2]), eyeTargetDistance));

	// �r���[�s������߂�B
	matView = XMMatrixLookAtLH(XMLoadFloat3(eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	// �������e�ϊ��s������߂�B
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(20.0f),				//��p(60�x)
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);

}

void LightCamera::GenerateViewMat()
{

	// ��]�s������߂�B
	rotationMat = XMMatrixIdentity();

	//�f�t�H���g�̐��ʃx�N�g��
	XMFLOAT3 defUpVec = { 0,0,1 };

	//���t���[���̏�x�N�g��
	XMFLOAT3 nowUpVec;
	XMStoreFloat3(&nowUpVec, *dir);

	//��]�������߂�
	XMFLOAT3 axisOfRevolutionBuff = FHelper::Cross3D(defUpVec, nowUpVec);

	//��]���𐳋K������
	if (!(axisOfRevolutionBuff.x == 0 && axisOfRevolutionBuff.y == 0 && axisOfRevolutionBuff.z == 0)) {
		axisOfRevolutionBuff = FHelper::Normalize3D(axisOfRevolutionBuff);
	}

	//��]����XMVECTOR�Ɏ���
	XMVECTOR axisOfRevolution = XMLoadFloat3(&axisOfRevolutionBuff);

	/*-- ���ς����]�ʂ����߂� --*/

	//��]�ʂ����߂�
	float amountOfRotation = FHelper::CalAngle3D(defUpVec, nowUpVec);

	//�t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ�
	amountOfRotation = acosf(amountOfRotation);


	/*-- �N�H�[�^�j�I�����g���ĉ�]������ --*/

	//��]����{0,0,0}�������珈�����΂�
	if (!(axisOfRevolution.m128_f32[0] == 0 && axisOfRevolution.m128_f32[1] == 0 && axisOfRevolution.m128_f32[2] == 0)) {

		//�N�H�[�^�j�I�������߂�
		XMVECTOR quaternion = XMQuaternionRotationAxis(axisOfRevolution, amountOfRotation);

		//���߂��N�H�[�^�j�I�����s��Ɏ���
		rotationMat = XMMatrixRotationQuaternion(quaternion);

	}

	// ��]�s������ƂɁA��x�N�g�������߂�B
	up = FHelper::MulRotationMatNormal(XMFLOAT3(0, 1, 0), rotationMat);

	// �����_���W�����߂�B
	const XMFLOAT3 eyeTargetDistance = XMFLOAT3(EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE);
	target = FHelper::AddXMFLOAT3(*eye, FHelper::MulXMFLOAT3(XMFLOAT3(dir->m128_f32[0], dir->m128_f32[1], dir->m128_f32[2]), eyeTargetDistance));

	// �r���[�s������߂�B
	matView = XMMatrixLookAtLH(XMLoadFloat3(eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	// �������e�ϊ��s������߂�B
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//��p(60�x)
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);

}
