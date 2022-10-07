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

	std::weak_ptr<PolygonMeshInstance> envMap1_;
	std::weak_ptr<PolygonMeshInstance> envMap2_;
	std::weak_ptr<PolygonMeshInstance> envMap3_;
	std::weak_ptr<BLAS> envMap1Blas_;
	std::weak_ptr<BLAS> envMap2Blas_;
	std::weak_ptr<BLAS> envMap3Blas_;

	std::weak_ptr<PolygonMeshInstance> pbrTest_;
	std::weak_ptr<PolygonMeshInstance> pbrTest2_;
	std::weak_ptr<BLAS> pbrTestBlas_;

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