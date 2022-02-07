#include "SoundManager.h"
#include "LightManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Sprite.h"
#include "FHelper.h"
#include "Enum.h"
#include "Object3D.h"
#include "MultiPathFunction.h"
#include "Camera.h"
#include "LightCamera.h"
#include "Input.h"
#include "TextureManager.h"

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

// fps�X�V
void FPS();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX����������----------*/
	DirectXBase directXBase;							// DirectX��Օ���
	directXBase.Init();									// DirectX��Ղ̏�����
	SoundManager::Instance()->SettingSoundManager();	// �T�E���h�}�l�[�W���[���Z�b�g����

	/*----------�p�C�v���C������----------*/
	PiplineManager::Instance()->Init();

	/*----------�ϐ��錾----------*/
	srand(time(NULL));

	// ���C�g�𐶐�
	for (int i = 0; i < POINTLIGHT_NUM; ++i) {
		//LightManager::Instance()->GeneratePointlight({}, { 1,1,1 }, { 0.5f,0.5f,0.5f });
		//LightManager::Instance()->pointlights[i].active = false;
	}
	LightManager::Instance()->GenerateDirlight({ 0,-1,0 }, { 1,1,1 });

	// �X�|�b�g���C�g�𐶐��B
	//LightManager::Instance()->GenerateSpotlight(XMFLOAT3(0, -1, 0), XMFLOAT3(0, 1000, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(0, 0, 00), XMFLOAT2(0, 0));

	RenderTarget shadowMapRenderTarget;
	float shadowMapClearColor[4] = { 1,1,1,1 };
	shadowMapRenderTarget.Create(window_width, window_height, 1, 1, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT, shadowMapClearColor);

	// �ʏ퉎�I�u�W�F�N�g�𐶐��B
	Object3D monkey;
	monkey.Generate(XMFLOAT3(0, 0, 0), PROJECTIONID_OBJECT, PIPLINE_OBJECT_LIGHT_ALPHA, "Resource/", "monkey.obj", L"Resource/plane.png", TRUE);
	monkey.ChangeScale(100, 100, 100);

	// �e�p���I�u�W�F�N�g�𐶐��B
	Object3D shadowMonkey;
	shadowMonkey.Generate(XMFLOAT3(0, 0, 0), PROJECTIONID_OBJECT, PIPLINE_OBJECT_GET_SHADOWMAP, "Resource/", "monkey.obj", L"Resource/plane.png", TRUE);
	shadowMonkey.ChangeScale(100, 100, 100);
	shadowMonkey.ClearTextureID();

	// ���I�u�W�F�N�g�𐶐��B
	Object3D ground;
	ground.Generate(XMFLOAT3(0, 0, 0), PROJECTIONID_OBJECT, PIPLINE_OBJECT_LIGHT_SHADOWMAP_ALPHA, "Resource/", "plane.obj", L"Resource/plane.png", FALSE);
	ground.ChangeScale(1000, 1000, 1000);
	ground.AddTextureID(shadowMapRenderTarget.textureID);
	ground.ChangePosition(monkey.GetPos().x, monkey.GetPos().y - 100.0f, monkey.GetPos().z);

	// ���C�g�J�����𐶐�
	LightCamera lightCamera;
	XMFLOAT3 lihghtEye = monkey.GetPos();
	lihghtEye.y += 1000;
	XMVECTOR dir = { 0,-1,0 };
	lightCamera.Generate(&lihghtEye, &dir);
	//lightCamera.GenerateViewMat();

	// ���C�g�J�����p�̒萔�o�b�t�@
	LightCameraConstBufferData lightCameraConstData;
	lightCameraConstData.lightViewProj = lightCamera.GetViewMat() * lightCamera.GetPerspectiveMat();

	short groundConstDataHandle = ground.AddConstBuffer(sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());
	short shadowMonkeyConstDataHandle = shadowMonkey.AddConstBuffer(sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());

	/*----------�Q�[�����[�v----------*/
	while (true) {
		/*----------���t���[������(�`��O����)----------*/
		directXBase.processBeforeDrawing();
		// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂�
		//DirectXBase::Instance()->SetRenderTarget();


		/*----- �X�V���� -----*/

		// �r���[�s��𐶐��B
		Camera::GenerateMatView();

		Camera::target = monkey.GetPos();

		monkey.ChangeRotation(0, 0.01f, 0);

		// �ʏ퉎�Ƒ��̉��̍s�����v������B
		shadowMonkey.AssignmentWorldMatrix(monkey.GetPositionMat(), monkey.GetScaleMat(), monkey.GetRotationMat());

		ground.TransData(groundConstDataHandle, &lightCameraConstData, sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());
		shadowMonkey.TransData(shadowMonkeyConstDataHandle, &lightCameraConstData, sizeof(LightCameraConstBufferData), typeid(LightCameraConstBufferData).name());

		/*----- �`�揈�� -----*/

		// �e�p�̃����_�[�^�[�Q�b�g�Ɏ����B
		MultiPathFunction::Instance()->SetRenderTargetAndClear(shadowMapRenderTarget);

		shadowMonkey.Draw();

		// ���\�[�X�o���A�����Ƃɖ߂��B
		MultiPathFunction::Instance()->ResourceBarrierAfter(shadowMapRenderTarget);

		// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂��B
		DirectXBase::Instance()->SetRenderTarget();

		// �ǂ̉���\�����邩�̃t���O�ɂ���ĕ`�悷�鉎��ς���
		monkey.Draw();

		ground.Draw();

		directXBase.processAfterDrawing();

	}

	return 0;
}


// �^�C�g���o�[��FPS�̍X�V
void FPS()
{
	static DWORD prev_time = timeGetTime();	// �O��̎���
	static int frame_count = 0;		// �t���[���J�E���g
	DWORD now_time = timeGetTime();		// ����̃t���[���̎���

	frame_count++;	// �t���[�������J�E���g����

	// �o�ߎ��Ԃ��P�b�𒴂�����J�E���g�Ǝ��Ԃ����Z�b�g
	if (now_time - prev_time >= 1000)
	{
		wchar_t fps[1000];
		_itow_s(frame_count, fps, 10);
		wchar_t moji[] = L"FPS";
		wcscat_s(fps, moji);
		SetWindowText(DirectXBase::Instance()->windowsAPI.hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}
}

/*

��������
�E�܂��̓V���h�E�}�b�v����������B
�E���̏�ԂŒʏ�`��ɕK�v�Ȓ萔�o�b�t�@�����S��void*�ŊǗ�����B
�E���̏�Ԃ�memcopy��map���Đ������������`�F�b�N�B
�E���ɉe�f�[�^��ǉ����Đ������������`�F�b�N�B


�E�萔�o�b�t�@��ǉ����鏈����ǉ�����B
�E�萔�o�b�t�@��]�����鏈����ǉ�����B


�}�b�v�����̈ȑO�ɕK�v�ȏ���
�ED3D12Resource�𐶐�����B
�E�܂�D3D12Resource��vector�z���p�ӂ���K�v������B
�E�����֐��ł͕ۑ�����Ă���Index���擾����B

�}�b�v�����ɕK�v�ȏ��
�ED3D12Resource�݂����Ȗ��O�̂�̃A�h���X(Map�֐����ĂԂ��߂ɕK�v�B)
�E���肽�����̃A�h���X�B
�E���肽�����̃T�C�Y�B




���̂��萔�o�b�t�@�������Ă��Ȃ��B�Ȃ����B

*/