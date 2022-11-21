#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include <memory>
#include <array>

class Character;
class PolygonMeshInstance;
class BLAS;
class BaseStage;
class RayComputeShader;
class RaytracingOutput;
class DynamicConstBuffer;

// �^�C�g���V�[��
class TitleScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	Sprite title_;
	Sprite titleOperation_;

	// ���x���I����UI
	std::array<Sprite, 3> levelSprite_;

	// ���x���I����UI�̍��W
	std::array<Vec3, 3> levelPos_;
	const std::array<Vec3, 3> LEVEL_POS = { Vec3(248.0f, 634.0f, 0.1f) ,Vec3(637.0f, 634.0f, 0.1f),Vec3(1022.0f, 634.0f, 0.1f) };
	std::array<float, 3> levelScaleRate_;	// Level�̉摜�̑傫���ɂ�����X�P�[���̊����B
	const Vec2 LEVEL_SPRITE_SCALE = Vec2(308.0f / 2.0f, 32.0f);
	float sinTimer_;	// ���x���̉摜���g�k������悤�̃^�C�}�[
	const float ADD_SIN_TIMER = 0.3f;
	const float SIN_SCALE_RATE = 0.04f;

	// ���x����UI�̌��̐Ԃ���p�ϐ�
	Sprite redSprite_;
	float redSpriteScaleRate_;
	float redSpriteAlpha_;
	int redSpriteExpSpan_;

	// �J�ڗ\��
	bool isReservationTransition_;
	bool isStartSceneTransition_;	// �V�[���J�ڂ��J�n�������t���O

	// �X�e�[�W�֌W�B
	std::vector<std::shared_ptr<BaseStage>> stages_;
	enum STAGE_ID {
		MUGEN,	// �����^�X�e�[�W
		MAX_STAGE,	// �X�e�[�W�̍ő吔
	};
	STAGE_ID nowStageID;

	// �^�C�����o�͗p�N���X
	std::shared_ptr<RaytracingOutput> tireMaskTexture_;
	std::shared_ptr<RaytracingOutput> tireMaskTextureOutput_;
	std::shared_ptr<RayComputeShader> tireMaskComputeShader_;
	std::shared_ptr<RayComputeShader> whiteOutComputeShader_;
	std::shared_ptr<DynamicConstBuffer> tireMaskConstBuffer_;

	std::shared_ptr<Character> player_;

	// �J�����̏��
	enum class CAMERA_MODE {
		START = 0,	// �X�^�[�g���̃J�������
		PUT_BEFORE = 1,	// �Ԃ̐��ʂɔz�u
		UP_CLOSE = 2,	// ���ڋ�
		FROM_FRONT = 3,	// ���ʂ���
	};
	// �e��Ԃ̎���
	const std::array<int, 4> CAMERA_TIMER = { 270, 180, 200, 200 };
	Vec3 cameraPutBeforePos_;	// �J�����𐳖ʂɒu���ۂ̍��W
	const float UPCLOSE_DISTANCE_MIN = 100.0f;
	const float UPCLOSE_DISTANCE_MAX = 400.0f;
	CAMERA_MODE cameraMode_;	// ���݂̃J�����̏��
	int transitionCounter_;		// �J�������J�ڂ�����
	int cameraTimer_;			// �J�����̐؂�ւ��Ɏg�p����^�C�}�[
	float cameraAngle_;			// �J��������]������Ƃ��̊p�x�B��ɃX�^�[�g���̃J�����Ŏg�p����B
	float cameraHeight_;		// �J�����̍����̃I�t�Z�b�g�B��ɃX�^�[�g���̃J�����Ŏg�p����B
	float cameraDistance_;		// �J�����ƃv���C���[�̋����B��ɃX�^�[�g���̃J�����Ŏg�p����B

	// ������@��UI�p
	Vec3 titleSpritePos_;
	bool isExp;
	const float ADD_EASING_TIMER_UI = 0.05f;
	float easingTimerUI_;

	// ���݂̃X�e�[�^�X
	enum class TITLE_STAT {
		TITLE,			// �^�C�g�����
		SELECT_LEVEL,	// ���x���I�����
	};
	float SPRITE_EXIT_POS_Y = 1280.0f + 1000.0f;
	TITLE_STAT titleStat_;
	TITLE_STAT nextStat_;
	float transitionEasingTimer_;
	int nowSelectLevel_;
	Vec2 LEVEL_RANGE = Vec2(0, 2);
	const float TRANSITION_EASING_ADD = 0.05f;
	bool isExitSprite_;	// �J�ڑO�̉摜����ʊO�ւł����ǂ����̃t���O
	bool isStatTransition_;	// �X�e�[�^�X��J�ڂ��邩�ǂ����B
	bool isFinishSceneTransition_;
	bool isAppear_;		// ���݃X�v���C�g���łĂ����Ԃ��B

	int logoTimer_;	// �N�����̂݃��S�𒷂߂ɂ����B
	const int LOGO_TIMER = 30;


public:

	/*===== �����o�֐� =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

private:

	// ���͏���
	void Input();

	// �J�����̍X�V����
	void CameraUpdate();

	// �^�C�g���̃X�e�[�^�X���Ƃ̍X�V����
	void UpdateTitleStat();

};