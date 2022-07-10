#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RaytracingPipline.h"
#include <memory>

#include "OBB.h"

class Player;
class DynamicConstBuffer;
class RaytracingPipline;
class TLAS;
class RaytracingOutput;
class Sprite;

// �Q�[���V�[��
class GameScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	RayConstBufferData constBufferData;
	std::shared_ptr<DynamicConstBuffer> constBuffer;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	std::vector<RayPiplineShaderData> dAOuseShaders;
	std::shared_ptr<RaytracingPipline> pipline;

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

	// �u�[�X�g�̃M�~�b�N
	int boostGimmickTest;

	// �����̃u���b�N�̃C���f�b�N�X
	std::vector<int> stageOrnamentBlas;
	std::vector<int> stageOrnamentIns;

	// �S�[���O�łӂ�ӂ킵�Ă���I�u�W�F�N�g�̃C���f�b�N�X�B
	int beforeTheGoalObjectBlas;
	std::vector<std::pair<int, int>> beforeTheGoalObjectIns;
	std::vector<std::pair<Vec3, Vec3>> beforeTheGoalObjectDefPos;
	std::vector<float> beforeTheGoalObjectTimer;

	// UI�֌W
	std::shared_ptr<Sprite> nowRapCountSprite;
	std::shared_ptr<Sprite> maxRapCountSprite;
	std::shared_ptr<Sprite> rapSlashSprite;
	std::array<int, 11> numFontHandle;


	// �S�[���֌W
	bool isPassedMiddlePoint;
	int rapCount;
	const Vec3 GOAL_DEF_POS = Vec3(10, -30, 0);

	// TLAS�𐶐��B
	std::shared_ptr<TLAS> tlas;

	// �A���r�G���g�I�N���[�W�����o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> aoOutput;
	std::shared_ptr<RaytracingOutput> denoiseAOOutput;

	// �F�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> colorOutput;

	// �f�m�C�Y���郉�C�g�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> lightOutput;
	std::shared_ptr<RaytracingOutput> denoiseLightOutput;

	// GI�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> giOutput;
	std::shared_ptr<RaytracingOutput> denoiseGiOutput;

	// �f�m�C�Y�}�X�N�p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> denoiseMaskOutput;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput;

	// FPS�\�������邩�ۂ�
	bool isDisplayFPS;

	// ���z�̊p�x
	float sunAngle;
	float sunSpeed;

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
	void InputImGUI();

	// �M�~�b�N�𐶐��B
	void GenerateGimmick();


};