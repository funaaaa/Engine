#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include "Vec.h"

// �`��f�[�^�N���X�̊��N���X
class BaseDrawData {

public:

	/*-- �����o�֐� --*/

	// �X�P�[�����X�V (�w�肵���傫���ɂȂ�B)
	void ChangeScale(Vec3 Amount);
	void ChangeScale(float X, float Y, float Z);
	// ��]�ʂ��X�V (�w�肵���ʕ���]�ʂ��������B)
	void AddRotation(Vec3 Amount);
	void AddRotation(float X, float Y, float Z);
	// ��]�ʂ����Z�B
	void ChangeRotation(Vec3 Amount);
	// ��]�ʂ�������
	void InitRotation();
	// ��]�ʂ���
	void AssignmentRotationMat(DirectX::XMMATRIX Amount);
	// ���W���X�V (�w�肵���ʒu�Ɉړ��B)
	void ChangePosition(Vec3 Amount);
	void ChangePosition(float X, float Y, float Z);
	// ��]�s��ɉ�]�s���������B
	void MulRotationMat(DirectX::XMMATRIX RotationMat);
	// ���W���X�V (�w�肵���ʂ𑫂��B)
	void ChangePositionAdd(Vec3 Amount);
	void ChangePositionAdd(float X, float Y, float Z);
	// �e��s������B
	void AssignmentWorldMatrix(const DirectX::XMMATRIX& PosMat, const DirectX::XMMATRIX& ScaleMat, const DirectX::XMMATRIX& RotationMat);
	// ��ʂɔ�\��
	void DoNotDisplay();
	// ��ʂɕ\��
	void DisplayOnScreen();
	// isDisplay��Ԃ�
	bool GetIsDisplay();
	// �w�肳�ꂽ�C���f�b�N�X�̃e�N�X�`��ID��؂�ւ���B(�C���f�b�N�X���I�[�o�[�t���[���A���_�[�t���[���Ă�����ő吔or�擪���w�肷��B)
	void ChangeTextureID(int TextureID, int Index);
	// �e�N�X�`��ID��ǉ�����B
	void AddTextureID(int TextureID);
	// �e�N�X�`��ID���N���A�B
	void ClearTextureID();

protected:

	/*-- �����o�ϐ� --*/

	// ��{�f�[�^
	bool isDisplay_ = true;			// �`�悷�邩�ǂ���
	std::vector<int> textureID_;		// �e�N�X�`����ID�i���o�[
	int projectionID_;				// �ǂ̓��e�����邩��ID
	int piplineID_;					// �g�p����p�C�v���C���̖��O

	// �e��s��
	DirectX::XMMATRIX scaleMat_;		// �X�P�[�����O�s��
	DirectX::XMMATRIX rotationMat_;	// ��]�s��(�W�ϒl)
	DirectX::XMMATRIX positionMat_;	// ���W�s��

	// �f�o�b�O�p�̍��W
	Vec3 pos_;						// ���W




	/*-- �e��\���� --*/

public:

	// ���_�f�[�^�\����
	struct Vertex {
		Vec3 pos_;			// xyz���W
		Vec3 normal_;		// �@��
		Vec2 uv_;			// uv���W
	};

	// �s��p�A�N�Z�b�T
	inline DirectX::XMMATRIX GetScaleMat() { return scaleMat_; }
	inline DirectX::XMMATRIX GetRotationMat() { return rotationMat_; }
	inline DirectX::XMMATRIX GetPositionMat() { return positionMat_; }

	// ���W�f�[�^���擾
	inline Vec3 GetPos() { return pos_; }

	// �e�N�X�`��ID���擾
	inline int GetTextureID(int Index) { return textureID_[Index]; }

protected:

	// �V�F�[�_�[�ɓn���s��̃f�[�^
	struct MatrixData {
		DirectX::XMMATRIX world_;		// ��]��������ړ��������肷��s��
		DirectX::XMMATRIX viewproj;		// �r���[�ƃv���W�F�N�V���������s��
	};

	// �萔�o�b�t�@�\����b0
	struct ConstBufferDataB0 {
		MatrixData mat;					// �s��
		DirectX::XMFLOAT4 color = { 1,1,1,1 };			// �F�f�[�^
		Vec3 eye_;						// �J�������W
		float pad1_;
	};


	/*-- �����o�֐� --*/

	// �萔�o�b�t�@�\����b0�̃}�b�v����
	void MapConstDataB0(ConstBufferDataB0* ConstMap, const ConstBufferDataB0& ConstBufferDataB0);


public:

	// GSParticle�p�̒萔�o�b�t�@�\����b0
	struct GSConstBufferDataB0 {
		DirectX::XMMATRIX mat;			// �s��
		DirectX::XMFLOAT4 color;		// �F�f�[�^
	};


};