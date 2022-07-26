#include "ShellObjectMgr.h"
#include "ShellObject.h"

void ShellObjectMgr::Setting()
{

	/*===== ���O�������� ======*/



}

void ShellObjectMgr::AddObject(const Vec3& Pos, const Vec3& ForwardVec)
{

	/*===== �I�u�W�F�N�g�ǉ� =====*/

	carapace_.emplace_back(std::make_shared<ShellObject>());
	carapace_.back()->Generate(Pos, ForwardVec);

}

void ShellObjectMgr::Update()
{

	/*===== �X�V���� =====*/

	for (auto& index : carapace_) {

		index->Update();

	}

	for (auto& index : carapace_) {

		if (index->GetIsActive()) continue;

		carapace_.erase(carapace_.begin(), carapace_.begin() + (&index - &carapace_[0]));

	}

}
