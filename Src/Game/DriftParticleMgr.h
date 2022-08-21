#pragma once
#include <memory>
#include <array>
#include "Singleton.h"
#include "Vec.h"
#include "ConstBuffers.h"

class DriftParticle;

// �h���t�g���̃p�[�e�B�N���Ǘ��N���X
class DriftParticleMgr : public Singleton<DriftParticleMgr> {

private:

	static const int DRIFT_PARTICLE_COUNT = 30;
	std::array<std::shared_ptr<DriftParticle>, DRIFT_PARTICLE_COUNT> driftParticle_;	// �h���t�g���̃p�[�e�B�N��
	
	const int GENERATE_DELAY = 3;
	int particleGenerateDelay_;		// �p�[�e�B�N���𐶐�����Ƃ��̒x��


public:

	// �Z�b�e�B���O
	void Setting();

	// ������
	void Init();

	// ��������
	void Generate(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData);

};