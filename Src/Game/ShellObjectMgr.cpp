#include "ShellObjectMgr.h"
#include "ShellObject.h"

void ShellObjectMgr::Setting()
{

	/*===== 事前準備処理 ======*/

	for (auto& index : shell_) {

		index = std::make_shared<ShellObject>();

	}

}

int ShellObjectMgr::AddObject(const Vec3& Pos, const Vec3& ForwardVec, float CharaRotY, int ShellID, std::weak_ptr<PolygonMeshInstance> CharaInstance)
{

	/*===== オブジェクト追加 =====*/

	for (auto& index : shell_) {

		if (index->GetIsActive()) continue;

		index->Generate(Pos, ForwardVec, CharaRotY, ShellID, CharaInstance);

		return static_cast<int>(&index - &shell_[0]);

	}

	return -1;

}

void ShellObjectMgr::DestroyObject(int Index)
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

void ShellObjectMgr::ChangeStatus(const int Index, const Vec3& ForwardVec, int ShellID)
{

	/*===== 指定のインデックスのステータスを更新 =====*/

	shell_[Index]->ChangeStatus(ForwardVec, ShellID);

}

bool ShellObjectMgr::Collider(std::weak_ptr<OBB> CharaOBB, const int IndexToSkipCollision)
{

	/*===== 当たり判定 =====*/

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
