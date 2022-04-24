#pragma once
#include "DirectXBase.h"
#include "Singleton.h"

#include <DirectXMath.h>
#include <vector>

using namespace std;
using namespace DirectX;

// �|���S���C���X�^���X�̎Q�Ƃ�ۑ�����N���X
class PorygonInstanceRegister : public Singleton<PorygonInstanceRegister> {

private:

	/*===== �����o�ϐ� =====*/

	vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescRegister;


public:

	/*===== �����o�֐� =====*/

	// ���W�X�^�[�ɃZ�b�g����֐��B
	UINT SetRegister(D3D12_RAYTRACING_INSTANCE_DESC instanceDesc);

	// �ړ��s����X�V�B
	void AddTrans(const XMMATRIX& transMat, const UINT& id);

	// ���W�X�^�[��Data���擾����֐��B
	D3D12_RAYTRACING_INSTANCE_DESC* GetData() { return instanceDescRegister.data(); }

	// ���W�X�^�[�̃T�C�Y���擾����֐��B
	inline const UINT& GetRegisterSize() { return UINT(instanceDescRegister.size()); }

};