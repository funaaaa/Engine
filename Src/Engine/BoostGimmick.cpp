#include "BoostGimmick.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "ModelDataManager.h"
#include "HitGroupMgr.h"
#include "TextureManager.h"

void BoostGimmick::Setting(const ID& GimmickID, const UINT& InstanceID)
{

	/*===== �Z�b�e�B���O���� =====*/

	// �I�u�W�F�N�g�����[�h
	insIndex_ = InstanceID;
	blasIndex_ = PolygonInstanceRegister::Ins()->GetBLASIndex(InstanceID);

	// �e��ϐ����������B
	gimmickID_ = GimmickID;
	isActive_ = true;

	// OBB���Z�b�g�B
	collider_.Setting(blasIndex_, insIndex_);

}

void BoostGimmick::Update()
{

	/*===== �X�V���� =====*/

}

ModelDataManager::ObjectData BoostGimmick::GenerateVertex(OBB Obb)
{

	/*===== obb��Instance��񂩂璸�_���𐶐� =====*/

	ModelDataManager::ObjectData objectData;

	// ���_�𐶐��B �O�ʉE��
	ModelDataManager::Vertex vertex_;
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// �O�ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// �O�ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// �O�ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// �w�ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// �w�ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// �w�ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// �w�ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// �E�ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// �E�ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// �E�ʍ���
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// �E�ʍ���
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// ���ʉE��
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// ���ʉE��
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// ���ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// ���ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// ��ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// ��ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// ��ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// ��ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// ���ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// ���ʉE��
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// ���ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// ���ʍ���
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// �C���f�b�N�X��ݒ�B
	objectData.index_ = {
		// �O��
		0,1,3,
		1,2,3,
		// �w��
		4,5,7,
		5,6,7,
		// �E��
		8,9,11,
		9,10,11,
		// ����
		12,13,15,
		13,14,15,
		// ���
		16,17,19,
		17,18,19,
		// ����
		20,21,23,
		21,22,23,
	};

	return objectData;
}
