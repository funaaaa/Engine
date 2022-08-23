#pragma once
#include "Singleton.h"
#include "Vec.h"

#include <DirectXMath.h>
#include <array>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

class PolygonMeshInstance;

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
	int CreateInstance(const int& BlasIndex, const UINT& ShaderID);

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
	Vec3 GetRotateVec3(const int& Index);

	// ��](���W�A���A��������)�֐�
	void ChangeRotate(const int& Index, const float& X, const float& Y, const float Z);
	void ChangeRotate(const int& Index, const Vec3& Rot);
	void ChangeRotate(const int& Index, DirectX::XMMATRIX Rot);

	// �g�k(���������Z)�֐�
	void AddScale(const int& Index, const float& X, const float& Y, const float Z);
	void AddScale(const int& Index, const Vec3& Scale);
	DirectX::XMMATRIX GetScale(const int& Index);

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

	// �e�q�֌W���l���������[���h���W�n�ł̍��W���擾�B
	Vec3 GetWorldPos(const int& Index);

	// �C���X�^���X��j���B
	void DestroyInstance(const int& Index);

	// �C���X�^���X��\��
	void Display(const int& Index);

	// �C���X�^���X���\��
	void NonDisplay(const int& Index);

	// ���W�X�^�[��Data���擾����֐��B
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDesc_.data(); };

	// ���W�X�^�[�̃T�C�Y���擾����֐��B
	UINT GetRegisterSize() { return UINT(instance_.size()); }

	// hlsl���ł̓����𔻒f����p�̎��ʎq
	enum SHADER_ID {

		DEF = 0,			// �ʏ�̃��C
		AS = 1,				// ��C�U���p
		TEXCOLOR = 2,		// �e�N�X�`���̐F�����̂܂ܕԂ�
		REFLECTION = 3,		// ���˂�����B
		COMPLETE_REFLECTION = 4,	// ���S���˂�����B
		LIGHT = 5,			// ���C�g�p�̃��C �e�N�X�`���̐F�����̂܂ܕԂ��A�V���h�E�Ƃ̓����蔻����s��Ȃ��B
		REFRACTION = 6,		// ���ܗp�̃��C
		INVISIBILITY = 7,	// �s���̃I�u�W�F�N�g
		DEF_GI = 8,			// �ʏ�̃��C GI���s���B
		DEF_GI_TIREMASK = 9,// �ʏ�̃��C GI �^�C����
		ALPHA = 10,			// ������ �萔�o�b�t�@��o�^����K�v�L��B

	};

};