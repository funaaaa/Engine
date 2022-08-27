#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RaytracingPipline.h"
#include <memory>

#include "OBB.h"


class Character;
class DynamicConstBuffer;
class RaytracingPipline;
class TLAS;
class RaytracingOutput;
class Sprite;
class BaseStage;
class RayComputeShader;
class CharacterMgr;
class ConcentrationLineMgr;

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

	// �X�e�[�W�֌W�B
	std::vector<std::shared_ptr<BaseStage>> stages_;
	enum STAGE_ID {
		CIRCUIT,	// �T�[�L�b�g
		MAX_STAGE,	// �X�e�[�W�̍ő吔
	};
	STAGE_ID nowStageID;

	// UI�֌W
	std::shared_ptr<Sprite> coinUI_;
	std::shared_ptr<Sprite> rapUI_;
	std::array<std::shared_ptr<Sprite>, 2> coinCountUI_;
	std::shared_ptr<Sprite> nowRapCountUI_;
	std::shared_ptr<Sprite> slashUI_;
	std::shared_ptr<Sprite> maxRapCountUI_;
	std::array<int, 11> numFontHandle_;

	std::shared_ptr<Sprite> itemFrameUI_;
	float itemFrameEasingTimer_;
	const Vec3 ITEM_FRAME_OUT_POS = Vec3(128.0f, -200.0f, 1.0f);
	const Vec3 ITEM_FRAME_IN_POS = Vec3(128.0f, 125.0f, 1.0f);

	// �W����
	std::shared_ptr<ConcentrationLineMgr> concentrationLine_;


	// �S�[���֌W
	bool isPassedMiddlePoint_;
	int rapCount_;
	const Vec3 GOAL_DEF_POS = Vec3(10, -30, 0);

	// TLAS�𐶐��B
	std::shared_ptr<TLAS> tlas_;

	// AO�o�͗p�B
	std::shared_ptr<RaytracingOutput> aoOutput_;
	std::shared_ptr<RaytracingOutput> denoiseAOOutput_;

	// �F�o�͗p�N���X�B
	std::shared_ptr<RaytracingOutput> colorOutput_;

	// ���C�g�o�͗p�B
	std::shared_ptr<RaytracingOutput> lightOutput_;
	std::shared_ptr<RaytracingOutput> denoiseLightOutput_;

	// GI�o�͗p�B
	std::shared_ptr<RaytracingOutput> giOutput_;
	std::shared_ptr<RaytracingOutput> denoiseGiOutput_;

	// �f�m�C�Y�}�X�N�p�B
	std::shared_ptr<RaytracingOutput> denoiseMaskOutput_;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput_;

	// �^�C�����o�͗p�N���X
	std::shared_ptr<RaytracingOutput> tireMaskTexture_;

	// �^�C�����o�e�X�g�p�N���X
	std::shared_ptr<RaytracingOutput> tireMaskTextureOutput_;
	std::shared_ptr<RayComputeShader> tireMaskComputeShader_;
	std::shared_ptr<RayComputeShader> whiteOutComputeShader_;
	std::shared_ptr<DynamicConstBuffer> tireMaskConstBuffer_;

	// FPS�\�������邩�ۂ�
	bool isDisplayFPS_;

	// ���z�̊p�x
	float sunAngle_;
	float sunSpeed_;

	// �L�����Ǘ��N���X
	std::shared_ptr<CharacterMgr> characterMgr_;

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