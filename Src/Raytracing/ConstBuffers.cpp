#include "ConstBuffers.h"
#include "Camera.h"
#include "WindowsAPI.h"
#include "FHelper.h"	

void CameraConstBufferData::Init()
{

	/*===== �J�����p�萔�o�b�t�@�������� =====*/

	Camera::Ins()->GenerateMatView();

	mtxProj_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),				//��p(60�x)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);
	mtxProjInv_ = XMMatrixInverse(nullptr, mtxProj_);
	mtxView_ = DirectX::XMMatrixLookAtLH(Camera::Ins()->eye_.ConvertXMVECTOR(), Camera::Ins()->target_.ConvertXMVECTOR(), Camera::Ins()->up_.ConvertXMVECTOR());
	mtxViewInv_ = XMMatrixInverse(nullptr, mtxView_);

}

void RayLightConstBufferData::Init()
{

	/*===== ���C�g�p�萔�o�b�t�@�������� =====*/

	// ���s�������Z�b�e�B���O
	dirLight_.isActive_ = true;
	dirLight_.lightColor_ = Vec3{ 0.0f,0.0f,1.0f };
	dirLight_.lihgtDir_ = Vec3{ -0.5f,-0.2f,0 };
	dirLight_.lihgtDir_.Normalize();
	dirLight_.seed_ = FHelper::GetRand(0, 1000);

}

void RayConstBufferData::Init()
{

	/*===== ���������� =====*/

	camera_.Init();
	light_.Init();
	alphaData_.Init();
	debug_.Init();
	volumeTextureData_.Init();

}

void AlphaConstBufferData::Init()
{

	/*===== ���������� =====*/

	for (auto& index : alphaData_) {

		index.alpha_ = 0;
		index.instanceIndex_ = 0;

	}

}

void DebugData::Init()
{

	/*===== �f�o�b�O�p���̏��������� =====*/

	isDebugMesnInfo_ = false;
	debugMeshMoveX_ = 0;
	timer_ = 0;

}

void VolumeTextureData::Init()
{

	color_ = Vec3(1.0f, 1.0f, 1.0f);
	gridSize_ = 0.5f;
	wrapCount_ = 1.0f;
	samplingLength_ = 10.0f;
	sanplingMaxCount_ = 100.0f;
	density_ = 1.0f;

}
