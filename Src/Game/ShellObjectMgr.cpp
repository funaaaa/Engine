#include "ShellObjectMgr.h"
#include "ShellObject.h"

void ShellObjectMgr::Setting()
{

	/*===== ���O�������� ======*/

	for (auto& index : shell_) {

		index = std::make_shared<ShellObject>();

	}

}

int ShellObjectMgr::AddObject(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, const int& CharaInsIndex)
{

	/*===== �I�u�W�F�N�g�ǉ� =====*/

	for (auto& index : shell_) {

		if (index->GetIsActive()) continue;

		index->Generate(Pos, ForwardVec, CharaRotY, ShellID, CharaInsIndex);

		return static_cast<int>(&index - &shell_[0]);

	}

	return -1;

}

void ShellObjectMgr::DestroyObject(const int& Index)
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
