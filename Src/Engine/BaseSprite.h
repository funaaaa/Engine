#pragma once
#include "BaseDrawData.h"
#include <array>

// �X�v���C�g��ՃN���X
class BaseSprite : public BaseDrawData {

protected:

	/*-- �����o�ϐ� --*/

	// ���_�f�[�^�֌W
	std::vector<Vertex> vertex_;								// ���_
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;			// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_;							// ���_�o�b�t�@�r���[

	// �萔�f�[�^�֌W
	ConstBufferDataB0 constBufferDataB0_;						// �萔 �s��⎋�_���W�A�J���[�Ȃ�
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> constBuffB0_;		// �萔�o�b�t�@

	// �f�o�b�Oor�����蔻��p
	Vec3 pos_;													// ���W


public:

	inline const std::vector<Vertex>& GetVertex() { return vertex_; }

};