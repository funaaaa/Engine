#pragma once
#include "Singleton.h"
#include "Vec.h"

#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include <wrl.h>

class PorygonMeshInstance;

// �|���S���C���X�^���X�̎Q�Ƃ�ۑ�����N���X
class PorygonInstanceRegister : public Singleton<PorygonInstanceRegister> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<PorygonMeshInstance>> instance;

	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDesc;

	using XMMATRIX = DirectX::XMMATRIX;


public:

	/*===== �����o�֐� =====*/

	// Ins�𐶐�����B
	int CreateInstance(const int& BlasIndex, const UINT& instanceID);

	// �ړ�(���������Z)�֐�
	void AddTrans(const int& Index, const float& X, const float& Y, const float Z);
	void AddTrans(const int& Index, const Vec3& Pos);

	// �ړ�(�����Ɉړ�)�֐�
	void ChangeTrans(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeTrans(const int& Index, const Vec3& Pos);
	DirectX::XMMATRIX GetTrans(const int& Index);

	// ��](���W�A���A���������Z)�֐�
	void AddRotate(const int& Index, const float& X, const float& Y, const float Z);
	void AddRotate(const int& Index, const Vec3& Rot);
	DirectX::XMMATRIX GetRotate(const int& Index);

	// ��](���W�A���A��������)�֐�
	void ChangeRotate(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeRotate(const int& Index, const Vec3& Rot);

	// �g�k(���������Z)�֐�
	void AddScale(const int& Index, const float& X, const float& Y, const float Z);
	void AddScale(const int& Index, const Vec3& Scale);
	DirectX::XMMATRIX GetScale(const int& Index);

	// �g�k(��������)�֐�
	void ChangeScale(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeScale(const int& Index, const Vec3& Scale);

	// �C���X�^���X�̃��[���h�s������߂�B
	void CalWorldMat();

	// ���W�X�^�[��Data���擾����֐��B
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc.data(); };

	// ���W�X�^�[�̃T�C�Y���擾����֐��B
	UINT GetRegisterSize() { return UINT(instance.size()); }

	// hlsl���ł̓����𔻒f����p�̎��ʎq
	enum{
	
		SHADER_ID_DEF = 0,			// �ʏ�̃��C
		SHADER_ID_AS = 1,			// ��C�U���p
		SHADER_ID_TEXCOLOR = 2,		// �e�N�X�`���̐F�����̂܂ܕԂ�
		SHADER_ID_REFLECTION = 3,	// ���˂�����B
		SHADER_ID_COMPLETE_REFLECTION = 4,	// ���S���˂�����B

	};

};