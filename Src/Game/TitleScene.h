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
	bool isExp;
	const float ADD_EASING_TIMER_UI = 0.05f;
	float easingTimerUI_;


public:

	/*===== �����o�֐� =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};