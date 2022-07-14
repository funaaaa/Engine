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

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	std::vector<RayPiplineShaderData> dAOuseShaders_;
	std::shared_ptr<RaytracingPipline> pipline_;

	// SPONZA��ǂݍ��ށB
	std::vector<int> sponzaInstance_;

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	int sphereBlas_;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns_;

	// �V���p�̃X�t�B�A
	int skyDomeBlas_;
	int skyDomeIns_;

	// �X�e�[�W
	int stageBlas_;
	int stageIns_;
	int stageGrassBlas_;
	int stageGrassIns_;

	// �S�[��
	int goalBlas_;
	int goalIns_;

	// �S�[���Ƃ��̓����蔻��p
	int goalCollisionBlas_;
	int goalCollisionIns_;
	int middlePointCollisionBlas_;
	int middlePointCollisionIns_;

	// �u�[�X�g�̃M�~�b�N
	int boostGimmickTest_;

	// �����̃u���b�N�̃C���f�b�N�X
	std::vector<int> stageOrnamentBlas_;
	std::vector<int> stageOrnamentIns_;

	// �S�[���O�łӂ�ӂ킵�Ă���I�u�W�F�N�g�̃C���f�b�N�X�B
	int beforeTheGoalObjectBlas_;
	std::vector<std::pair<int, int>> beforeTheGoalObjectIns_;
	std::vector<std::pair<Vec3, Vec3>> beforeTheGoalObjectDefPos_;
	std::vector<float> beforeTheGoalObjectTimer_;

	// UI�֌W
	std::shared_ptr<Sprite> nowRapCountSprite_;
	std::shared_ptr<Sprite> maxRapCountSprite_;
	std::shared_ptr<Sprite> rapSlashSprite_;
	std::array<int, 11> numFontHandle_;


	// �S�[���֌W
	bool isPassedMiddlePoint_;
	int rapCount_;
	const Vec3 GOAL_DEF_POS = Vec3(10, -30, 0);

	// TLAS�𐶐��B
	std::shared_ptr<TLAS> tlas_;

	// �A���r�G���g�I�N���[�W�����o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> aoOutput_;
	std::shared_ptr<RaytracingOutput> denoiseAOOutput_;

	// �F�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> colorOutput_;

	// �f�m�C�Y���郉�C�g�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> lightOutput_;
	std::shared_ptr<RaytracingOutput> denoiseLightOutput_;

	// GI�o�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> giOutput_;
	std::shared_ptr<RaytracingOutput> denoiseGiOutput_;

	// �f�m�C�Y�}�X�N�p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> denoiseMaskOutput_;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput_;

	// FPS�\�������邩�ۂ�
	bool isDisplayFPS_;

	// ���z�̊p�x
	float sunAngle_;
	float sunSpeed_;

	// �v���C���[
	std::shared_ptr<Player> player_;

	int testIns_;


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