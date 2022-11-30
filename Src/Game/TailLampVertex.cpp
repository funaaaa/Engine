#include "TailLampVertex.h"

TailLampVertex::TailLampVertex()
{

	/*===== コンストラクタ =====*/

	isActive_ = false;

}

void TailLampVertex::Init()
{

	/*===== 初期化処理 =====*/

	isActive_ = false;
	startScaleDownTimer_ = 0;

}

void TailLampVertex::Generate(std::array<Vec3, 4> Vertex, int TextureHandle)
{

	/*===== 生成処理 =====*/

	isActive_ = true;
	startScaleDownTimer_ = START_SCALE_DOWN_TIMER;
	vertex_ = Vertex;

	//// BLASを生成する。
	//{
	//	// モデルデータを書き込む。
	//	ModelDataManager::ObjectData data;

	//	// 頂点を書き込む。
	//	const int VERTEX_SIZE = 4;
	//	for (int index = 0; index < VERTEX_SIZE; ++index) {

	//		ModelDataManager::Vertex vert;

	//		vert.pos_ = Vertex[index];
	//		vert.normal_ = Vertex[0].Cross(Vertex[1]).GetNormal();
	//		vert.uv_ = Vec2(0.5f, 0.5f);

	//		data.vertex_.emplace_back(vert);

	//	}

	//	// 頂点インデックスを書き込む。
	//	{
	//		data.index_.emplace_back(0);
	//		data.index_.emplace_back(1);
	//		data.index_.emplace_back(2);

	//		data.index_.emplace_back(2);
	//		data.index_.emplace_back(3);
	//		data.index_.emplace_back(0);
	//	}

	//	// その他情報を書き込む。
	//	data.material_.textureHandle_ = TextureHandle;

	//	// BLASを生成する。
	//	blas_ = BLASRegister::Ins()->GenerateData(data, true);

	//}

	//// Instanceを生成する。
	//ins_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::Ins()->TEXCOLOR);

}

void TailLampVertex::Update()
{

	/*===== 更新処理 =====*/

	// 一定時間で頂点を縮小させるタイマーを更新。
	if (0 < startScaleDownTimer_) {
		--startScaleDownTimer_;
	}
	else {

		// 頂点を縮小させる。
		{
			// 0番目を2番目の方向へ
			Vec3 dir = Vec3(vertex_[2] - vertex_[0]).GetNormal();
			float length = Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f;
			vertex_[0] += dir * (length / 5.0f);
		}
		{
			// 1番目を3番目の方向へ
			Vec3 dir = Vec3(vertex_[3] - vertex_[1]).GetNormal();
			float length = Vec3(vertex_[3] - vertex_[1]).Length() / 2.0f;
			vertex_[1] += dir * (length / 5.0f);
		}
		{
			// 2番目を0番目の方向へ
			Vec3 dir = Vec3(vertex_[0] - vertex_[2]).GetNormal();
			float length = Vec3(vertex_[0] - vertex_[2]).Length() / 2.0f;
			vertex_[2] += dir * (length / 5.0f);
		}
		{
			// 3番目を1番目の方向へ
			Vec3 dir = Vec3(vertex_[1] - vertex_[3]).GetNormal();
			float length = Vec3(vertex_[1] - vertex_[3]).Length() / 2.0f;
			vertex_[3] += dir * (length / 5.0f);
		}

		// 距離の差が一定以下になったら終了
		if (Vec3(vertex_[0] - vertex_[1]).Length() < 1.0f) {

			Init();

		}

	}

}
