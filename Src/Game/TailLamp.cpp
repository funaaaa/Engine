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

	/*===== �R���X�g���N�^ =====*/

	// BLAS�𐶐�
	{
		// ���f���f�[�^���������ށB
		ModelDataManager::ObjectData data;

		// ���_���������ށB
		const int VERTEX_SIZE = 8;
		for (int index = 0; index < VERTEX_SIZE; ++index) {

			ModelDataManager::Vertex vert;

			vert.pos_ = Vec3();
			vert.normal_ = Vec3(1, 0, 0);
			vert.uv_ = Vec2(0.5f, 0.5f);

			data.vertex_.emplace_back(vert);

		}

		// ���_�C���f�b�N�X���������ށB
		{
			// ���
			data.index_.emplace_back(1);
			data.index_.emplace_back(6);
			data.index_.emplace_back(5);
			data.index_.emplace_back(6);
			data.index_.emplace_back(1);
			data.index_.emplace_back(2);

			// �E��
			data.index_.emplace_back(2);
			data.index_.emplace_back(7);
			data.index_.emplace_back(6);
			data.index_.emplace_back(7);
			data.index_.emplace_back(2);
			data.index_.emplace_back(3);

			// ����
			data.index_.emplace_back(0);
			data.index_.emplace_back(4);
			data.index_.emplace_back(7);
			data.index_.emplace_back(7);
			data.index_.emplace_back(3);
			data.index_.emplace_back(0);

			// ����
			data.index_.emplace_back(0);
			data.index_.emplace_back(5);
			data.index_.emplace_back(4);
			data.index_.emplace_back(5);
			data.index_.emplace_back(0);
			data.index_.emplace_back(1);

		}

		// ���̑������������ށB
		data.material_.textureHandle_ = TextureManager::Ins()->LoadTexture(L"Resource/Game/Stage/MugenStage/green.png");
		data.textureHandle_ = data.material_.textureHandle_;

		// BLAS�𐶐�����B
		blas_ = BLASRegister::Ins()->GenerateData(data, true);
	}

	// INSTANCE�𐶐�
	ins_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::Ins()->DEF_TAILLAMP);

	// ���̑��ϐ���������
	isActive_ = false;

}

void TailLamp::Generate(std::weak_ptr<TailLampVertex> VertexStart, std::weak_ptr<TailLampVertex> VertexEnd, int TextureColor)
{

	/*===== �������� =====*/

	isActive_ = true;

	// ���_��ݒ�
	vertexStart_ = VertexStart;
	vertexEnd_ = VertexEnd;

	// �e�Q�ƃJ�E���g���C���N�������g
	++vertexStart_.lock()->referenceCount_;
	++vertexEnd_.lock()->referenceCount_;

	// �e�N�X�`����ݒ�B
	blas_.lock()->ChangeBaseTexture(TextureColor);
	blas_.lock()->GetMaterial().alpha_ = 1.0f;
	blas_.lock()->IsChangeMaterial();

	// �C���X�^���X��\��
	PolygonInstanceRegister::Ins()->Display(ins_);

}

void TailLamp::Update()
{

	/*===== �X�V���� =====*/

	// BLAS���X�V�B
	for (int index = 0; index < 4; ++index) {
		blas_.lock()->ChangeVertexPosition(index, vertexStart_.lock()->GetVertex()[index]);
	}
	for (int index = 0; index < 4; ++index) {
		blas_.lock()->ChangeVertexPosition(index + 4, vertexEnd_.lock()->GetVertex()[index]);
	}

	blas_.lock()->Update();

	// ���_�̏k�����������������`�F�b�N�B
	bool isFinishStart = vertexStart_.lock()->GetIsFinishScaleDown();
	bool isFinishEnd = vertexEnd_.lock()->GetIsFinishScaleDown();

	// �ǂ�����������Ă�����A�Q�ƃJ�E���g���f�N�������g���ď������B��\���ɂ���B
	if (isFinishStart && isFinishEnd) {

		--vertexStart_.lock()->referenceCount_;
		--vertexEnd_.lock()->referenceCount_;

		isActive_ = false;
		PolygonInstanceRegister::Ins()->NonDisplay(ins_);

	}

}