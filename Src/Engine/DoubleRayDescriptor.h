#pragma once
#include <array>
#include <memory>
#include <wtypes.h>

class RayDescriptor;
class DoubleResourceWrapper;

// RayDescriptor���d�o�b�t�@�ɂ���p�̃N���X
class DoubleRayDescriptor {

private:

	/*===== �����o�ϐ� =====*/

	std::array<std::shared_ptr<RayDescriptor>, 2> rayDescriptor_;

	
public:

	/*===== �����o�֐� =====*/

	// SRV�𐶐��B
	void CreateSRV(std::weak_ptr<DoubleResourceWrapper> Resource, UINT NumElements, UINT FirstElement, UINT Stride);

	// GPU�A�h���X���擾
	std::weak_ptr<RayDescriptor> GetDescriptor(int Index);

};