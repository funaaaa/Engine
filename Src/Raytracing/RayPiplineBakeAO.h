#pragma once
#include "RaytracingPipline.h"
#include <vector>
#include <memory>

class RaytracingOutput;

// AO�x�C�N�p�p�C�v���C��
class RayPiplineBakeAO : public RaytracingPipline {

public:

	/*===== �����o�֐� =====*/

	// �x�C�N�pUAV���������ނ��߂̃V�F�[�_�[���R�[�h�ݒ菈���B
	void ConstructionShaderTableBakeAO(std::vector<std::shared_ptr<RaytracingOutput>> BakeTargets, const int& DispatchX, const int& DispatchY);

};