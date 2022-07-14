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
	ModelDataManager::Vertex vertex;
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// �O�ʉE��
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// �O�ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// �O�ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// �w�ʉE��
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// �w�ʉE��
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// �w�ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// �w�ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// �E�ʉE��
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// �E�ʉE��
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// �E�ʍ���
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// �E�ʍ���
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// ���ʉE��
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// ���ʉE��
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// ���ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// ���ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// ��ʉE��
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// ��ʉE��
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// ��ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// ��ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// ���ʉE��
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// ���ʉE��
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// ���ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// ���ʍ���
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// �C���f�b�N�X��ݒ�B
	objectData.index = {
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
