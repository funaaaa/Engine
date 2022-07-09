#pragma once
#include <string>
#include <vector>
#include <wtypes.h>
#include "PolygonInstanceRegister.h"
#include "OBB.h"

class BaseGimmick {

public:

	/*===== �����o�ϐ� =====*/

	// �M�~�b�N��ID
	enum class ID {

		BOOST,		// �����M�~�b�N

	};

protected:

	ID gimmickID;	// �M�~�b�N��ID
	int blasIndex;	// �I�u�W�F�N�g��BLAS�̃C���f�b�N�X
	int insIndex;	// �I�u�W�F�N�g��INSTANCE�̃C���f�b�N�X
	bool isActive;	// �L�����t���O
	OBB collider;	// �����蔻��pOBB


public:

	/*===== �����o�֐� =====*/

	// ����������
	virtual void Setting(const ID& GimmickID, const UINT& InstanceID) = 0;

	// �X�V����
	virtual void Update() = 0;

	// �L�����B
	inline void Activate() { isActive = true; }
	// �������B
	inline void Invalidate() { isActive = false; }

	// �Q�b�^
	inline const int& GetBLASIndex() { return blasIndex; }
	inline const int& GetINSTANCEIndex() { return insIndex; }

	// �ړ��֌W
	void AddTrans(const Vec3& Trans);
	void ChangeTrans(const Vec3& Trans);

	// �T�C�Y�֌W
	void AddScale(const Vec3& Scale);
	void ChangeScale(const Vec3& Scale);

	// ��]�֌W
	void AddRotate(const Vec3& Rotate);
	void ChangeRotate(const Vec3& Rotate);

};