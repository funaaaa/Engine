#pragma once
#include "BaseObject3D.h"

class LightCamera;

class Object3D : public BaseObject3D {

public:

	// ��������
	void Generate(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string modelFileName, LPCWSTR textureFileName, bool isSmoothing);

	// �`�揈��
	void Draw();

	// �p�C�v���C�����X�V���鏈��
	void UpdatePiplineID(int piplineID);

	// �C���f�b�N�X�T�C�Y���擾����
	inline int GetIndexSize() { return index.size(); }

	// ���_�R���e�i���擾����
	inline vector<Vertex> GetVertex() { return vertex; }

	// ���_�C���f�b�N�X�R���e�i���擾����
	inline vector<unsigned short> GetIndex() { return index; }

};