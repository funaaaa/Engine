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
class BLAS;
class RayComputeShader;
class CharacterMgr;
class ConcentrationLineMgr;
class PolygonMeshInstance;

// �Q�[���V�[��
class PBRTestScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	std::vector<RayPiplineShaderData> dAOuseShaders_;
	std::shared_ptr<RaytracingPipline> pipline_;

	std::shared_ptr<Sprite> itemFrameUI_;
	float itemFrameEasingTimer_;
	const Vec3 ITEM_FRAME_OUT_POS = Vec3(128.0f, -200.0f, 1.0f);
	const Vec3 ITEM_FRAME_IN_POS = Vec3(128.0f, 125.0f, 1.0f);

	// PBR�p�I�u�W�F�N�g
	std::weak_ptr<BLAS> pbrSphereBlas_;
	std::weak_ptr<PolygonMeshInstance> pbrSphereIns_;

	// ���C�g�ʒu�p�X�t�B�A
	std::weak_ptr<BLAS> lightSphereBlas_;
	std::weak_ptr<PolygonMeshInstance> lightSphereIns_;

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




public:

	/*===== �����o�֐� =====*/

	PBRTestScene();
	void Init()override;
	void Update()override;
	void Draw()override;


};