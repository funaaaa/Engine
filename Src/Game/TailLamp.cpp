#include "TailLamp.h"
#include "ModelDataManager.h"
#include "BLAS.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "TextureManager.h"
#include "TailLampVertex.h"

TailLamp::TailLamp()
{

	/*===== コンストラクタ =====*/

	// BLASを生成
	{
		// モデルデータを書き込む。
		ModelDataManager::ObjectData data;

		// 頂点を書き込む。
		const int VERTEX_SIZE = 8;
		for (int index = 0; index < VERTEX_SIZE; ++index) {

			ModelDataManager::Vertex vert;

			vert.pos_ = Vec3();
			vert.normal_ = Vec3(1, 0, 0);
			vert.uv_ = Vec2(0.5f, 0.5f);

			data.vertex_.emplace_back(vert);

		}

		// 頂点インデックスを書き込む。
		{
			// 上面
			data.index_.emplace_back(1);
			data.index_.emplace_back(6);
			data.index_.emplace_back(5);
			data.index_.emplace_back(6);
			data.index_.emplace_back(1);
			data.index_.emplace_back(2);

			// 右面
			data.index_.emplace_back(2);
			data.index_.emplace_back(7);
			data.index_.emplace_back(6);
			data.index_.emplace_back(7);
			data.index_.emplace_back(2);
			data.index_.emplace_back(3);

			// 下面
			data.index_.emplace_back(0);
			data.index_.emplace_back(4);
			data.index_.emplace_back(7);
			data.index_.emplace_back(7);
			data.index_.emplace_back(3);
			data.index_.emplace_back(0);

			// 左面
			data.index_.emplace_back(0);
			data.index_.emplace_back(5);
			data.index_.emplace_back(4);
			data.index_.emplace_back(5);
			data.index_.emplace_back(0);
			data.index_.emplace_back(1);

		}

		// その他情報を書き込む。
		data.material_.textureHandle_ = TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/green.png");
		data.textureHandle_ = data.material_.textureHandle_;

		// BLASを生成する。
		blas_ = BLASRegister::Ins()->GenerateData(data, true);
	}

	// INSTANCEを生成
	ins_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::Ins()->DEF_TAILLAMP);

	// その他変数を初期化
	isActive_ = false;

}

void TailLamp::Generate(std::weak_ptr<TailLampVertex> VertexStart, std::weak_ptr<TailLampVertex> VertexEnd, int TextureColor)
{

	/*===== 生成処理 =====*/

	isActive_ = true;

	// 頂点を設定
	vertexStart_ = VertexStart;
	vertexEnd_ = VertexEnd;

	// 各参照カウントをインクリメント
	++vertexStart_.lock()->referenceCount_;
	++vertexEnd_.lock()->referenceCount_;

	// テクスチャを設定。
	blas_.lock()->ChangeBaseTexture(TextureColor);
	blas_.lock()->GetMaterial().alpha_ = 1.0f;
	blas_.lock()->IsChangeMaterial();

	// インスタンスを表示
	PolygonInstanceRegister::Ins()->Display(ins_);

}

void TailLamp::Update()
{

	/*===== 更新処理 =====*/

	// BLASを更新。
	for (int index = 0; index < 4; ++index) {
		blas_.lock()->ChangeVertexPosition(index, vertexStart_.lock()->GetVertex()[index]);
	}
	for (int index = 0; index < 4; ++index) {
		blas_.lock()->ChangeVertexPosition(index + 4, vertexEnd_.lock()->GetVertex()[index]);
	}

	blas_.lock()->Update();

	// 頂点の縮小が完了したかをチェック。
	bool isFinishStart = vertexStart_.lock()->GetIsFinishScaleDown();
	bool isFinishEnd = vertexEnd_.lock()->GetIsFinishScaleDown();

	// どちらも完了していたら、参照カウントをデクリメントして初期化。非表示にする。
	if (isFinishStart && isFinishEnd) {

		--vertexStart_.lock()->referenceCount_;
		--vertexEnd_.lock()->referenceCount_;

		isActive_ = false;
		PolygonInstanceRegister::Ins()->NonDisplay(ins_);

	}

}