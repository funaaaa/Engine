#include "TailLampVertex.h"

TailLampVertex::TailLampVertex()
{

	/*===== �R���X�g���N�^ =====*/

	isActive_ = false;
	isFinishScaleDown_ = false;
	referenceCount_ = 0;

}

void TailLampVertex::Init()
{

	/*===== ���������� =====*/

	isActive_ = false;
	isFinishScaleDown_ = false;
	startScaleDownTimer_ = 0;
	referenceCount_ = 0;

}

void TailLampVertex::Generate(std::array<Vec3, 4> Vertex, float ScaleOffsetRate)
{

	/*===== �������� =====*/

	isActive_ = true;
	isFinishScaleDown_ = false;
	startScaleDownTimer_ = START_SCALE_DOWN_TIMER;
	vertex_ = Vertex;

	// ���S�_�����߂�B
	Vec3 centerPos = vertex_[0] + Vec3(vertex_[2] - vertex_[0]).GetNormal() * (Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f);

	// ���_���k��������B
	{
		// 0�Ԗڂ�2�Ԗڂ̕�����
		Vec3 dir = -Vec3(vertex_[2] - vertex_[0]).GetNormal();
		float length = Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f;
		vertex_[0] = centerPos + (dir * (length * ScaleOffsetRate));
	}
	{
		// 1�Ԗڂ�3�Ԗڂ̕�����
		Vec3 dir = -Vec3(vertex_[3] - vertex_[1]).GetNormal();
		float length = Vec3(vertex_[3] - vertex_[1]).Length() / 2.0f;
		vertex_[1] = centerPos + (dir * (length * ScaleOffsetRate));
	}
	{
		// 2�Ԗڂ�0�Ԗڂ̕�����
		Vec3 dir = -Vec3(vertex_[0] - vertex_[2]).GetNormal();
		float length = Vec3(vertex_[0] - vertex_[2]).Length() / 2.0f;
		vertex_[2] = centerPos + (dir * (length * ScaleOffsetRate));
	}
	{
		// 3�Ԗڂ�1�Ԗڂ̕�����
		Vec3 dir = -Vec3(vertex_[1] - vertex_[3]).GetNormal();
		float length = Vec3(vertex_[1] - vertex_[3]).Length() / 2.0f;
		vertex_[3] = centerPos + (dir * (length * ScaleOffsetRate));
	}

}

void TailLampVertex::Update()
{

	/*===== �X�V���� =====*/

	// ��莞�ԂŒ��_���k��������^�C�}�[���X�V�B
	if (0 < startScaleDownTimer_) {
		--startScaleDownTimer_;
	}
	else {

		float div = 20.0f;

		// ���_���k��������B
		{
			// 0�Ԗڂ�2�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[2] - vertex_[0]).GetNormal();
			float length = Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f;
			vertex_[0] += dir * (length / div);
		}
		{
			// 1�Ԗڂ�3�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[3] - vertex_[1]).GetNormal();
			float length = Vec3(vertex_[3] - vertex_[1]).Length() / 2.0f;
			vertex_[1] += dir * (length / div);
		}
		{
			// 2�Ԗڂ�0�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[0] - vertex_[2]).GetNormal();
			float length = Vec3(vertex_[0] - vertex_[2]).Length() / 2.0f;
			vertex_[2] += dir * (length / div);
		}
		{
			// 3�Ԗڂ�1�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[1] - vertex_[3]).GetNormal();
			float length = Vec3(vertex_[1] - vertex_[3]).Length() / 2.0f;
			vertex_[3] += dir * (length / div);
		}

		// �����̍������ȉ��ɂȂ�����k�����I������t���O�𗧂Ă�B
		if (Vec3(vertex_[0] - vertex_[1]).Length() < 1.0f || Vec3(vertex_[0] - vertex_[2]).Length() < 1.0f || Vec3(vertex_[0] - vertex_[3]).Length() < 1.0f) {

			isFinishScaleDown_ = true;

		}

		// �k�����I����Ă��āA���̒��_�Z�b�g���Q�Ƃ��Ă���e�[�������v�����݂��Ă��Ȃ������珉�����B
		if (isFinishScaleDown_ && referenceCount_ <= 0) {

			Init();

		}

	}

}
