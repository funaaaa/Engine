#pragma once
#include "Singleton.h"
#include "Vec.h"
#include "FHelper.h"

#include <DirectXMath.h>
#include <array>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

class PolygonMeshInstance;
class BLAS;

// �|���S���C���X�^���X�̎Q�Ƃ�ۑ�����N���X
class PolygonInstanceRegister : public Singleton<PolygonInstanceRegister> {

public:

	/*===== �����o�ϐ� =====*/

	static const int MAX_INSTANCE = 256;	// �C���X�^���X�̍ő吔�B


private:

	std::array<std::shared_ptr<PolygonMeshInstance>, MAX_INSTANCE> instance_;

	std::array<D3D12_RAYTRACING_INSTANCE_DESC, MAX_INSTANCE> instanceDesc_;

	using XMMATRIX = DirectX::XMMATRIX;


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O����
	void Setting();

	// Ins�𐶐�����B
	std::weak_ptr<PolygonMeshInstance> CreateInstance(std::weak_ptr<BLAS> Blas_, const UINT& ShaderID, bool HaveMeshCollisionData = false);

	// �C���X�^���X�̃��[���h�s������߂�B
	void CalWorldMat();

	// �C���X�^���X��j���B
	void DestroyInstance(std::weak_ptr<PolygonMeshInstance> Instance);
	void DestroyInstance(const int& Index);

	// �C���X�^���X��\��
	void Display(std::weak_ptr<PolygonMeshInstance> Instance);
	void Display(const int& Index);

	// �C���X�^���X���\��
	void NonDisplay(std::weak_ptr<PolygonMeshInstance> Instance);
	void NonDisplay(const int& Index);

	// ���W�X�^�[��Data���擾����֐��B
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc_.data(); };

	// ���W�X�^�[�̃T�C�Y���擾����֐��B
	UINT GetRegisterSize() { return UINT(instance_.size()); }

	// ���b�V���̓����蔻��f�[�^���擾�B
	const std::vector<FHelper::CheckHitPorygon>& GetMeshCollisionData(const int& Index);

	// hlsl���ł̓����𔻒f����p�̎��ʎq
	enum SHADER_ID {

		DEF = 0,			// �ʏ�̃��C
		AS = 1,				// ��C�U���p
		TEXCOLOR = 2,		// �e�N�X�`���̐F�����̂܂ܕԂ�
		LIGHT = 5,			// ���C�g�p�̃��C �e�N�X�`���̐F�����̂܂ܕԂ��A�V���h�E�Ƃ̓����蔻����s��Ȃ��B
		REFRACTION = 6,		// ���ܗp�̃��C
		INVISIBILITY = 7,	// �s���̃I�u�W�F�N�g
		DEF_GI = 8,			// �ʏ�̃��C GI���s���B
		DEF_GI_TIREMASK = 9,// �ʏ�̃��C GI �^�C����
		ALPHA = 10,			// ������ �萔�o�b�t�@��o�^����K�v�L��B
		ADD = 11,			// ���Z���� �萔�o�b�t�@��o�^����K�v�L��B

	};

};