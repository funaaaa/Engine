#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RaytracingPipline.h"
#include <memory>

class Player;
class DynamicConstBuffer;
class RaytracingPipline;
class TLAS;
class RaytracingOutput;

// �Q�[���V�[��
class GameScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	RayConstBufferData constBufferData;
	std::shared_ptr<DynamicConstBuffer> constBuffer;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	std::vector<RayPiplineShaderData> dAOuseShaders;
	std::shared_ptr<RaytracingPipline> deAOPipline;

	// SPONZA��ǂݍ��ށB
	std::vector<int> sponzaInstance;

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	int sphereBlas;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns;

	// �V���p�̃X�t�B�A
	int skyDomeBlas;
	int skyDomeIns;

	// �X�e�[�W
	int stageBlas;
	int stageIns;
	int stageGrassBlas;
	int stageGrassIns;

	// �S�[��
	int goalBlas;
	int goalIns;

	// �S�[���Ƃ��̓����蔻��p
	int goalCollisionBlas;
	int goalCollisionIns;
	int middlePointCollisionBlas;
	int middlePointCollisionIns;

	// �S�[���֌W
	bool isPassedMiddlePoint;
	int rapCount;
	const Vec3 GOAL_DEF_POS = Vec3(10, -30, 0);

	// TLAS�𐶐��B
	std::shared_ptr<TLAS> tlas;

	// �A���r�G���g�I�N���[�W�����o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> aoOutput;

	// �F�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> colorOutput;

	// �f�m�C�Y���郉�C�g�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> lightOutput;

	// GI�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> giOutput;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput;

	// FPS�\�������邩�ۂ�
	bool isDisplayFPS;


	// �f�o�b�O�p�̃p�C�v���C����؂�ւ����B
	enum DEGU_PIPLINE_ID {
		DEF_PIPLINE,
		AO_PIPLINE,
		DENOISE_AO_PIPLINE,
	};

	// �f�o�b�O�p�Ńm�C�Y��ʂ��o���t���O�B
	DEGU_PIPLINE_ID debugPiplineID;

	// ���C�g����������
	bool isMoveLight;

	// �v���C���[
	std::shared_ptr<Player> player;


public:

	/*===== �����o�֐� =====*/

	GameScene();
	void Init()override;
	void Update()override;
	void Draw()override;


	// fps�X�V
	void FPS();

	// ���͑���
	void Input();
	void InputImGUI(bool& IsMove);


};