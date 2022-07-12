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

class PolygonMeshInstance;

// �|���S���C���X�^���X�̎Q�Ƃ�ۑ�����N���X
class PolygonInstanceRegister : public Singleton<PolygonInstanceRegister> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<PolygonMeshInstance>> instance;

	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDesc;

	using XMMATRIX = DirectX::XMMATRIX;


public:

	/*===== �����o�֐� =====*/

	// Ins�𐶐�����B
	int CreateInstance(const int& BlasIndex, const UINT& InstanceID);

	// �ړ�(���������Z)�֐�
	void AddTrans(const int& Index, const float& X, const float& Y, const float Z);
	void AddTrans(const int& Index, const Vec3& Pos);

	// �ړ�(�����Ɉړ�)�֐�
	void ChangeTrans(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeTrans(const int& Index, const Vec3& Pos);
	void ChangeTrans(const int& Index, DirectX::XMMATRIX Trans);
	DirectX::XMMATRIX GetTrans(const int& Index);
	Vec3 GetPos(const int& Index);

	// ��](���W�A���A���������Z)�֐�
	void AddRotate(const int& Index, const float& X, const float& Y, const float Z);
	void AddRotate(const int& Index, const Vec3& Rot);
	void AddRotate(const int& Index, const DirectX::XMMATRIX& Rot);
	DirectX::XMMATRIX GetRotate(const int& Index);
	Vec3 GetRotVec3(const int& Index);

	// ��](���W�A���A��������)�֐�
	void ChangeRotate(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeRotate(const int& Index, const Vec3& Rot);
	void ChangeRotate(const int& Index, DirectX::XMMATRIX Rot);

	// �g�k(���������Z)�֐�
	void AddScale(const int& Index, const float& X, const float& Y, const float Z);
	void AddScale(const int& Index, const Vec3& Scale);
	DirectX::XMMATRIX GetScale(const int& Index);
	Vec3 GetScaleVec3(const int& Index);

	// �g�k(��������)�֐�
	void ChangeScale(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeScale(const int& Index, const Vec3& Scale);
	void ChangeScale(const int& Index, DirectX::XMMATRIX Scale);

	// �e�s���ݒ�B
	void SetParentInstance(const int& Index, const int& ParentIndex);

	// BLAS��Index��������B
	UINT GetBLASIndex(const int& Index);

	// �C���X�^���X�̃��[���h�s������߂�B
	void CalWorldMat();

	// ���W�X�^�[��Data���擾����֐��B
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc.data(); };

	// ���W�X�^�[�̃T�C�Y���擾����֐��B
	UINT GetRegisterSize() { return UINT(instance.size()); }

	// hlsl���ł̓����𔻒f����p�̎��ʎq
	enum SHADER_ID {

		DEF = 0,			// �ʏ�̃��C
		AS = 1,			// ��C�U���p
		TEXCOLOR = 2,		// �e�N�X�`���̐F�����̂܂ܕԂ�
		REFLECTION = 3,	// ���˂�����B
		COMPLETE_REFLECTION = 4,	// ���S���˂�����B
		LIGHT = 5,		// ���C�g�p�̃��C �e�N�X�`���̐F�����̂܂ܕԂ��A�V���h�E�Ƃ̓����蔻����s��Ȃ��B
		REFRACTION = 6,	// ���ܗp�̃��C
		INVISIBILITY = 7,	// �s���̃I�u�W�F�N�g
		DEF_GI = 8,		// �ʏ�̃��C GI���s���B

	};

};