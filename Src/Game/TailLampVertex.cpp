#include "TailLampVertex.h"

TailLampVertex::TailLampVertex()
{

	/*===== �R���X�g���N�^ =====*/

	isActive_ = false;

}

void TailLampVertex::Init()
{

	/*===== ���������� =====*/

	isActive_ = false;
	startScaleDownTimer_ = 0;

}

void TailLampVertex::Generate(std::array<Vec3, 4> Vertex, int TextureHandle)
{

	/*===== �������� =====*/

	isActive_ = true;
	startScaleDownTimer_ = START_SCALE_DOWN_TIMER;
	vertex_ = Vertex;

	//// BLAS�𐶐�����B
	//{
	//	// ���f���f�[�^���������ށB
	//	ModelDataManager::ObjectData data;

	//	// ���_���������ށB
	//	const int VERTEX_SIZE = 4;
	//	for (int index = 0; index < VERTEX_SIZE; ++index) {

	//		ModelDataManager::Vertex vert;

	//		vert.pos_ = Vertex[index];
	//		vert.normal_ = Vertex[0].Cross(Vertex[1]).GetNormal();
	//		vert.uv_ = Vec2(0.5f, 0.5f);

	//		data.vertex_.emplace_back(vert);

	//	}

	//	// ���_�C���f�b�N�X���������ށB
	//	{
	//		data.index_.emplace_back(0);
	//		data.index_.emplace_back(1);
	//		data.index_.emplace_back(2);

	//		data.index_.emplace_back(2);
	//		data.index_.emplace_back(3);
	//		data.index_.emplace_back(0);
	//	}

	//	// ���̑������������ށB
	//	data.material_.textureHandle_ = TextureHandle;

	//	// BLAS�𐶐�����B
	//	blas_ = BLASRegister::Ins()->GenerateData(data, true);

	//}

	//// Instance�𐶐�����B
	//ins_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::Ins()->TEXCOLOR);

}

void TailLampVertex::Update()
{

	/*===== �X�V���� =====*/

	// ��莞�ԂŒ��_���k��������^�C�}�[���X�V�B
	if (0 < startScaleDownTimer_) {
		--startScaleDownTimer_;
	}
	else {

		// ���_���k��������B
		{
			// 0�Ԗڂ�2�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[2] - vertex_[0]).GetNormal();
			float length = Vec3(vertex_[2] - vertex_[0]).Length() / 2.0f;
			vertex_[0] += dir * (length / 5.0f);
		}
		{
			// 1�Ԗڂ�3�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[3] - vertex_[1]).GetNormal();
			float length = Vec3(vertex_[3] - vertex_[1]).Length() / 2.0f;
			vertex_[1] += dir * (length / 5.0f);
		}
		{
			// 2�Ԗڂ�0�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[0] - vertex_[2]).GetNormal();
			float length = Vec3(vertex_[0] - vertex_[2]).Length() / 2.0f;
			vertex_[2] += dir * (length / 5.0f);
		}
		{
			// 3�Ԗڂ�1�Ԗڂ̕�����
			Vec3 dir = Vec3(vertex_[1] - vertex_[3]).GetNormal();
			float length = Vec3(vertex_[1] - vertex_[3]).Length() / 2.0f;
			vertex_[3] += dir * (length / 5.0f);
		}

		// �����̍������ȉ��ɂȂ�����I��
		if (Vec3(vertex_[0] - vertex_[1]).Length() < 1.0f) {

			Init();

		}

	}

}
