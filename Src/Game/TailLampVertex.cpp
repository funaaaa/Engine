#include "TailLampVertex.h"

TailLampVertex::TailLampVertex()
{

	/*===== コンストラクタ =====*/

	isActive_ = false;
	isFinishScaleDown_ = false;
	referenceCount_ = 0;

}

void TailLampVertex::Init()
{

	/*===== 初期化処理 =====*/

	isActive_ = false;
	isFinishScaleDown_ = false;
	startScaleDownTimer_ = 0;
	referenceCount_ = 0;

}

void TailLampVertex::Generate(std::array<Vec3, 4> Vertex, float ScaleOffsetRate)
{

	/*===== 生成処理 =====*/

	isActive_ = true;
	isFinishScaleDown_ = false;
	startScaleDownTimer_ = START_SCALE_DOWN_TIMER;
	vertex_ = Vertex;

	// 中心点を求める。
	Vec3 centerPos = vertex_[0] + Vec3(vertex_[2] - vertex_[0]).GetNormal() * (Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f);

	// 頂点を縮小させる。
	{
		// 0番目を2番目の方向へ
		Vec3 dir = -Vec3(vertex_[2] - vertex_[0]).GetNormal();
		float length = Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f;
		vertex_[0] = centerPos + (dir * (length * ScaleOffsetRate));
	}
	{
		// 1番目を3番目の方向へ
		Vec3 dir = -Vec3(vertex_[3] - vertex_[1]).GetNormal();
		float length = Vec3(vertex_[3] - vertex_[1]).Length() / 2.0f;
		vertex_[1] = centerPos + (dir * (length * ScaleOffsetRate));
	}
	{
		// 2番目を0番目の方向へ
		Vec3 dir = -Vec3(vertex_[0] - vertex_[2]).GetNormal();
		float length = Vec3(vertex_[0] - vertex_[2]).Length() / 2.0f;
		vertex_[2] = centerPos + (dir * (length * ScaleOffsetRate));
	}
	{
		// 3番目を1番目の方向へ
		Vec3 dir = -Vec3(vertex_[1] - vertex_[3]).GetNormal();
		float length = Vec3(vertex_[1] - vertex_[3]).Length() / 2.0f;
		vertex_[3] = centerPos + (dir * (length * ScaleOffsetRate));
	}

}

void TailLampVertex::Update()
{

	/*===== 更新処理 =====*/

	// 一定時間で頂点を縮小させるタイマーを更新。
	if (0 < startScaleDownTimer_) {
		--startScaleDownTimer_;
	}
	else {

		float div = 20.0f;

		// 頂点を縮小させる。
		{
			// 0番目を2番目の方向へ
			Vec3 dir = Vec3(vertex_[2] - vertex_[0]).GetNormal();
			float length = Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f;
			vertex_[0] += dir * (length / div);
		}
		{
			// 1番目を3番目の方向へ
			Vec3 dir = Vec3(vertex_[3] - vertex_[1]).GetNormal();
			float length = Vec3(vertex_[3] - vertex_[1]).Length() / 2.0f;
			vertex_[1] += dir * (length / div);
		}
		{
			// 2番目を0番目の方向へ
			Vec3 dir = Vec3(vertex_[0] - vertex_[2]).GetNormal();
			float length = Vec3(vertex_[0] - vertex_[2]).Length() / 2.0f;
			vertex_[2] += dir * (length / div);
		}
		{
			// 3番目を1番目の方向へ
			Vec3 dir = Vec3(vertex_[1] - vertex_[3]).GetNormal();
			float length = Vec3(vertex_[1] - vertex_[3]).Length() / 2.0f;
			vertex_[3] += dir * (length / div);
		}

		// 距離の差が一定以下になったら縮小が終わったフラグを立てる。
		if (Vec3(vertex_[0] - vertex_[1]).Length() < 1.0f || Vec3(vertex_[0] - vertex_[2]).Length() < 1.0f || Vec3(vertex_[0] - vertex_[3]).Length() < 1.0f) {

			isFinishScaleDown_ = true;

		}

		// 縮小が終わっていて、この頂点セットを参照しているテールランプが存在していなかったら初期化。
		if (isFinishScaleDown_ && referenceCount_ <= 0) {

			Init();

		}

	}

}
