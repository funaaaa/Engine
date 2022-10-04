#pragma once
#include "Vec.h"
#include <memory>

class OBB;
class BaseStage;
class PolygonMeshInstance;
class BLAS;

class ShellObject {

public:

	enum class SHELL_ID {

		FORWARD_THROW,	// 投げられている状態
		BEHIND_THROW,
		BEHIND,	// 後ろに保持されている状態

	};

private:

	/*===== メンバ変数 =====*/

	// モデルデータ関係
	std::weak_ptr<BLAS> blas_;							// BLASのインデックス
	std::weak_ptr<PolygonMeshInstance> instance;		// INSTANCEのインデックス
	std::weak_ptr<PolygonMeshInstance> charaInstance;	// 保持しているキャラのINSTANCEのインデックス

	// オブジェクト固有のパラメーター
	Vec3 pos_;				// 座標
	Vec3 prevPos_;			// 前フレーム座標
	Vec3 forwardVec_;		// 移動方向ベクトル
	Vec3 size_;				// サイズ
	float rotY_;			// Y軸回転
	float speed_;			// 移動速度
	float gravity_;			// 重力
	int exitTimer_;			// 消滅までの時間
	bool isActive_;			// 有効化フラグ
	bool onGround_;			// 接地フラグ
	bool isDestroyed_;		// インスタンスが破棄済みかどうか

	SHELL_ID shellID_;		// 甲羅のID

	std::shared_ptr<OBB> obb_;		// 当たり判定用OBB

	const float SPEED = 50.0f;
	const float SCALE = 20.0f;
	const float MAX_GRAV = 10.0f;	// 重力の最大量
	const float ADD_GRAV = 1.0f;	// 重力の加算量
	const int EXIT_TIMER = 180;		// 消滅までの時間


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	ShellObject();

	// 破棄処理
	void Destroy();

	// 生成処理
	void Generate(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, std::weak_ptr<PolygonMeshInstance> CharaInstance);

	// 更新処理
	void Update(std::weak_ptr<BaseStage> StageData);

	// OBBとの当たり判定 キャラクターとの当たり判定で使用する。
	bool CheckHitOBB(std::weak_ptr<OBB> CharaOBB);

	bool GetIsActive() { return isActive_; }

private:

	// 当たり判定
	void CheckHit(std::weak_ptr<BaseStage> StageData);

};