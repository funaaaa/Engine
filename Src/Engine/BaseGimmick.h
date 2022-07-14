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

	ID gimmickID_;	// �M�~�b�N��ID
	int blasIndex_;	// �I�u�W�F�N�g��BLAS�̃C���f�b�N�X
	int insIndex_;	// �I�u�W�F�N�g��INSTANCE�̃C���f�b�N�X
	bool isActive_;	// �L�����t���O
	OBB collider_;	// �����蔻��pOBB


public:

	/*===== �����o�֐� =====*/

	// ����������
	virtual void Setting(const ID& GimmickID, const UINT& InstanceID) = 0;

	// �X�V����
	virtual void Update() = 0;

	// �L�����B
	inline void Activate() { isActive_ = true; }
	// �������B
	inline void Invalidate() { isActive_ = false; }

	// �Q�b�^
	inline const int& GetBLASIndex() { return blasIndex_; }
	inline const int& GetINSTANCEIndex() { return insIndex_; }
	inline const bool& GetIsActive() { return isActive_; }
	inline OBB& GetOBB() { return collider_; }

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