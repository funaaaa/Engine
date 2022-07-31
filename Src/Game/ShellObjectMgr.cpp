#include "ShellObjectMgr.h"
#include "ShellObject.h"

void ShellObjectMgr::Setting()
{

	/*===== 事前準備処理 ======*/

	for (auto& index : shell_) {

		index = std::make_shared<ShellObject>();

	}

}

int ShellObjectMgr::AddObject(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, const int& CharaInsIndex)
{

	/*===== オブジェクト追加 =====*/

	for (auto& index : shell_) {

		if (index->GetIsActive()) continue;

		index->Generate(Pos, ForwardVec, CharaRotY, ShellID, CharaInsIndex);

		return static_cast<int>(&index - &shell_[0]);

	}

	return -1;

}

void ShellObjectMgr::DestroyObject(const int& Index)
{

	/*===== オブジェクト破壊 =====*/

	shell_[Index]->Destroy();

}

void ShellObjectMgr::Update(std::weak_ptr<BaseStage> StageData)
{

	/*===== 更新処理 =====*/

	for (auto& index : shell_) {

		// 有効化されていたら
		if (index->GetIsActive()) {

			index->Update(StageData);

		}
		else {

			index->Destroy();

		}

	}

}
