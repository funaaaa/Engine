#pragma once
#include "Singleton.h"
#include <vector>
#include <memory>

class RaytracingOutput;
class RayPiplineBakeAO;
class RayRootsignature;

// AO���e�N�X�`���Ƀx�C�N
class BakeAO : public Singleton<BakeAO> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<RaytracingOutput>> bakeTex;		// �������ݐ�e�N�X�`��
	std::shared_ptr<RayPiplineBakeAO> pipline;					// �g�p����p�C�v���C��
	std::shared_ptr<RayRootsignature> rootsignature;			// �g�p���郋�[�g�V�O�l�`��


public:

	/*===== �����o�ϐ� =====*/

	// �x�C�N�����s�B
	void ExecutionBake(const int& TexCount);

	// �x�C�N���ʂ̃e�N�X�`�����擾�B
	std::vector<std::shared_ptr<RaytracingOutput>>& GetBakeTex() { return bakeTex; }

};