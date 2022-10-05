#pragma once
#include "BaseScene.h"
#include "ConstBuffers.h"
#include "RayPipeline.h"
#include <memory>

#include "OBB.h"


class Character;
class DynamicConstBuffer;
class RaytracingOutput;
class Sprite;
class BaseStage;
class RayComputeShader;
class CharacterMgr;
class ConcentrationLineMgr;
class PolygonMeshInstance;

// �Q�[���V�[��
class GameScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	int sphereBlas_;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns_;

	// �V���p�̃X�t�B�A
	std::weak_ptr<BLAS> skyDomeBlas_;
	std::weak_ptr<PolygonMeshInstance> skyDomeIns_;

	// �X�e�[�W�֌W�B
	std::vector<std::shared_ptr<BaseStage>> stages_;
	enum STAGE_ID {
		MUGEN,	// �����^�X�e�[�W
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
	bool isGameFinish_;
	int transitionTimer;
	const int TRANSION_TIME = 180;

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


	// PBR�e�X�g�p
	std::weak_ptr<BLAS> pbrSphereBlas_;
	std::weak_ptr<PolygonMeshInstance> pbrSphereIns_;


	/*-- ���[�X�J�n�O�̕ϐ� --*/

	std::shared_ptr<Sprite> countDownSprite_;
	std::shared_ptr<Sprite> goSprite_;
	const Vec2 COUNT_DOWN_FONT_SIZE = Vec2(16.0f * 2.1f, 32.0f * 2.1f);
	const Vec2 GO_FONT_SIZE = Vec2(512.0f / 5.0f, 256.0f / 5.0f);
	int countDownNumber_;		// �J�E���g�_�E���ł̌��݂̔ԍ�
	int countDownStartTimer_;	// �J�n�O�ɃJ�E���g�_�E�����n�܂�܂ł̃^�C�}�[
	const int COUNT_DOWN_TIMER = 120;
	float countDownEasingTimer_;						// �J�E���g�_�E����UI�������ۂ̉��Z��
	const float COUNT_DOWN_EASING_TIMER = 1.0f / 30.0f;	// �J�E���g�_�E����UI�������ۂ̉��Z�� �l��0.5�b�ŉ�ʂ̔����𓮂����Ȃ���΍s���Ȃ����߁A1.0f��30F�Ŋ����Ă���B
	bool isCountDownExit_;		// �J�E���g�_�E���̍ۂ�UI�̓����Ŏg�p�B t:�A���t�@�l�������� f:�ォ��^��
	bool isBeforeStart_;		// �J�n�O���ǂ����t���O
	bool isCountDown_;			// �J�E���g�_�E�����B

	Vec3 WINDOW_CENTER = Vec3(1280.0f / 2.0f, 720.0f / 2.0f, 0.1f);
	Vec3 COUNT_DOWN_START_POS = Vec3(1280.0f / 2.0f, 720.0f / 2.0f - 300.0f, 0.1f);


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

private:

	// �Q�[���J�n�O�̍X�V����
	void UpdateCountDown();


};