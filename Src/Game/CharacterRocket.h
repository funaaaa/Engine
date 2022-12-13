#pragma once
#include "Vec.h"
#include <memory>
#include <array>

class BLAS;
class PolygonMeshInstance;

// �L�����N�^�[�̉����A�C�e���̃��P�b�g���������邽�߂̃I�u�W�F�N�g�Ɋւ���@�\���܂Ƃ߂��N���X
class CharacterRocket {

private:

	/*===== �����o�ϐ� =====*/

	std::array<std::weak_ptr<BLAS>, 4> rocketBlas_;					// ���P�b�g�̌`��f�[�^
	std::array<std::weak_ptr<PolygonMeshInstance>, 4 > rocketIns_;	// ���P�b�g�̕`��f�[�^
	float rocketEasingTimer_;										// ���P�b�g�̃C�[�W���O�S�ʂɎg�p����^�C�}�[
	float rocketRotationY_;											// ���P�b�g��Y����]�����ʁB
	float rocketAddRotationY_;										// ���P�b�g��Y����]�ʁB���Y����]������B
	bool isDisplayRocket_;											// ���P�b�g��`�悷�邩�̃t���O�B

public:

	// �X�V�����ɕK�v�ȓ��͏��
	struct UpdateInData {
		Vec3 pos_;
		Vec3 forwardVec_;
		Vec3 upVec_;
		bool isPlayer_;
		bool isGetItem_;
		bool isUseItem_;
		bool hasItem_;
	};


public:

	CharacterRocket();
	void Init();
	void Update(const UpdateInData& InData);

};