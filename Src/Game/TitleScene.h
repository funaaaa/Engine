#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include <memory>

class Character;
class PolygonMeshInstance;
class BLAS;

// タイトルシーン
class TitleScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	Sprite title_;

	// コーネルボックス用
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

	/*===== メンバ関数 =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};