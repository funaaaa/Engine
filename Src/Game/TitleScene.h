#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include <memory>

class Character;
class PolygonMeshInstance;
class BLAS;

// �^�C�g���V�[��
class TitleScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	Sprite title_;

	// ���}�b�v3���
	std::weak_ptr<PolygonMeshInstance> envMap1_;
	std::weak_ptr<PolygonMeshInstance> envMap2_;
	std::weak_ptr<PolygonMeshInstance> envMap3_;
	std::weak_ptr<BLAS> envMap1Blas_;
	std::weak_ptr<BLAS> envMap2Blas_;
	std::weak_ptr<BLAS> envMap3Blas_;

	// PBR�e�X�g�p��2���
	std::weak_ptr<PolygonMeshInstance> pbrTest_;
	std::weak_ptr<PolygonMeshInstance> pbrTest2_;
	std::weak_ptr<BLAS> pbrTestBlas_;

	// �R�[�l���{�b�N�X�p
	std::weak_ptr<BLAS> cornellBoxWhiteBlas_;
	std::weak_ptr<BLAS> cornellBoxGreenBlas_;
	std::weak_ptr<BLAS> cornellBoxRedBlas_;
	std::weak_ptr<PolygonMeshInstance> cornellBoxWhite_;
	std::weak_ptr<PolygonMeshInstance> cornellBoxGreen_;
	std::weak_ptr<PolygonMeshInstance> cornellBoxRed_;

	float cameraAngle;
	int invMapIndex_;
	int objectIndex_;

	std::shared_ptr<Character> player_;

public:

	/*===== �����o�֐� =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};