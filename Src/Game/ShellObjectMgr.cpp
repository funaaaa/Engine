#include "ShellObjectMgr.h"
#include "ShellObject.h"

void ShellObjectMgr::Setting()
{

	/*===== ���O�������� ======*/

	for (auto& index : shell_) {

		index = std::make_shared<ShellObject>();

	}

}

int ShellObjectMgr::AddObject(const Vec3& Pos, const Vec3& ForwardVec, float CharaRotY, int ShellID, std::weak_ptr<PolygonMeshInstance> CharaInstance)
{

	/*===== �I�u�W�F�N�g�ǉ� =====*/

	for (auto& index : shell_) {

		if (index->GetIsActive()) continue;

		index->Generate(Pos, ForwardVec, CharaRotY, ShellID, CharaInstance);

		return static_cast<int>(&index - &shell_[0]);

	}

	return -1;

}

void ShellObjectMgr::DestroyObject(int Index)
{

	/*===== �I�u�W�F�N�g�j�� =====*/

	shell_[Index]->Destroy();

}

void ShellObjectMgr::Update(std::weak_ptr<BaseStage> StageData)
{

	/*===== �X�V���� =====*/

	for (auto& index : shell_) {

		// �L��������Ă�����
		if (index->GetIsActive()) {

			index->Update(StageData);

		}
		else {

			index->Destroy();

		}

	}

}

void ShellObjectMgr::ChangeStatus(const int Index, const Vec3& ForwardVec, int ShellID)
{

	/*===== �w��̃C���f�b�N�X�̃X�e�[�^�X���X�V =====*/

	shell_[Index]->ChangeStatus(ForwardVec, ShellID);

}

bool ShellObjectMgr::Collider(std::weak_ptr<OBB> CharaOBB, const int IndexToSkipCollision)
{

	/*===== �����蔻�� =====*/

	for (auto& index : shell_) {

		if (!index->GetIsActive()) continue;
		if (static_cast<int>(&index - &shell_[0]) == IndexToSkipCollision) continue;

		bool isHit = index->CheckHitOBB(CharaOBB);

		if (!isHit) continue;

		index->Destroy();

		return true;

	}

	return false;
}
