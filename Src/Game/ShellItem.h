#pragma once
#include "BaseItem.h"

class ShellItem : public BaseItem {

private:

	/*===== �����o�ϐ� =====*/

	int behindShellIndex_;	// ���ɕێ����ꂽ�b���̃C���f�b�N�X

public:

	enum class PARAM_ID {

		BEHIND,				// ���ɕێ�
		FORWARD_THROW,		// �O�ɓ�����
		BEHIND_THROW,		// ���ɕێ�

	};


public:

	/*===== �����o�֐� =====*/

	ShellItem();
	void Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance)override;
	void Update()override;
	void Use(const float& CharaRotY = 0, const int ParamID = 0)override;

};