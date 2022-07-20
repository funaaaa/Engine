#include "FHelper.h"

Vec3 FHelper::CalBary(const Vec3& PosA, const Vec3& PosB, const Vec3& PosC, const Vec3& TargetPos)
{

	/*===== �d�S���W�����߂� =====*/

	Vec3 uvw = Vec3();

	// �O�p�`�̖ʐς����߂�B
	float areaABC = (PosC - PosA).Cross(PosB - PosA).Length() / 2.0f;

	// �d�S���W�����߂�B
	uvw.x_ = ((PosA - TargetPos).Cross(PosB - TargetPos).Length() / 2.0f) / areaABC;
	uvw.y_ = ((PosB - TargetPos).Cross(PosC - TargetPos).Length() / 2.0f) / areaABC;
	uvw.z_ = ((PosC - TargetPos).Cross(PosA - TargetPos).Length() / 2.0f) / areaABC;

	return uvw;
}

bool FHelper::RayToModelCollision(RayToModelCollisionData CollisionData, Vec3& ImpactPos, float& Distance, Vec3& HitNormal, Vec2& HitUV)
{

	/*----- target����|���S���𔲂���� -----*/

	struct CheckHitVertex {

		Vec3 pos_;
		Vec3 normal_;
		Vec2 uv_;

	};

	// ���C�Ƃ̓����蔻��p�̃|���S���\����
	struct CheckHitPorygon {
		bool isActive_ = true;
		CheckHitVertex p1_;
		CheckHitVertex p2_;
		CheckHitVertex p3_;
	};

	// �������Ă���|���S���̃f�[�^��ۑ�����悤�ϐ�	�Փ˒n�_�A�����A�Փ˖ʂ̖@��
	struct HitPorygonData
	{
		Vec3 pos_;
		float distance_;
		Vec3 normal_;
		CheckHitPorygon targetPolygon_;
	};

	std::vector<HitPorygonData> hitPolygon{};

	std::vector<CheckHitPorygon> targetPolygon;		//�|���S���ۑ��p�R���e�i

	// target�̃|���S�����ɍ��킹�ă��T�C�Y
	targetPolygon.resize(static_cast<unsigned __int64>(static_cast<float>(CollisionData.targetVertex_.size()) / 3.0f));

	// �|���S���̒��g����
	for (auto& index : targetPolygon) {

		// ���_1
		index.p1_.pos_ = CollisionData.targetVertex_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3)])];
		index.p1_.normal_ = CollisionData.targetNormal_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3)])];
		index.p1_.uv_ = CollisionData.targetUV_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3)])];
		// ���_2
		index.p2_.pos_ = CollisionData.targetVertex_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 1)])];
		index.p2_.normal_ = CollisionData.targetNormal_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 1)])];
		index.p2_.uv_ = CollisionData.targetUV_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 1)])];
		// ���_3
		index.p3_.pos_ = CollisionData.targetVertex_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 2)])];
		index.p3_.normal_ = CollisionData.targetNormal_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 2)])];
		index.p3_.uv_ = CollisionData.targetUV_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 2)])];
		// �L�����t���O
		index.isActive_ = true;
	}

	/*----- �ۑ������|���S���̒��_���W�Ƀ��[���h�ϊ��s��������� -----*/
	// ���[���h�s��
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	matWorld *= CollisionData.matScale_;
	matWorld *= CollisionData.matRot_;
	matWorld *= CollisionData.matTrans_;
	for (auto& index : targetPolygon) {
		// ���_��ϊ�
		index.p1_.pos_ = DirectX::XMVector3Transform(index.p1_.pos_.ConvertXMVECTOR(), matWorld);
		index.p2_.pos_ = DirectX::XMVector3Transform(index.p2_.pos_.ConvertXMVECTOR(), matWorld);
		index.p3_.pos_ = DirectX::XMVector3Transform(index.p3_.pos_.ConvertXMVECTOR(), matWorld);
		// �@������]�s�񕪂����ϊ�
		index.p1_.normal_ = XMVector3Transform(index.p1_.normal_.ConvertXMVECTOR(), CollisionData.matRot_);
		index.p1_.normal_.Normalize();
		index.p2_.normal_ = XMVector3Transform(index.p2_.normal_.ConvertXMVECTOR(), CollisionData.matRot_);
		index.p2_.normal_.Normalize();
		index.p3_.normal_ = XMVector3Transform(index.p3_.normal_.ConvertXMVECTOR(), CollisionData.matRot_);
		index.p3_.normal_.Normalize();
	}

	/*----- ���C�̕����Ɩ@�������������Ȃ珜�O -----*/
	for (auto& index : targetPolygon) {
		// �܂���1�ڂ̒��_���`�F�b�N
		if (-0.0001f < index.p1_.normal_.Dot(CollisionData.rayDir_)) {
			// �����܂ŗ����犮�S�ɔ��Α��������Ă���̂ŁA�폜�t���O�����Ă�
			index.isActive_ = false;
			continue;
		}
	}

	/*----- �|���S����3���_�Ƌ��E���̓����蔻����s���A���ȏ㋗�����������ꍇ�͏������΂� -----*/

	/*----- �|���S�����Ƃɓ����蔻�� -----*/
	// ���������̓|���S�����ƂɌv�Z����
	for (auto& index : targetPolygon) {
		/*----- ���C�ƕ��ʂ̏Փ˓_���v�Z���� -----*/

		// �|���S��������������Ă����玟�̏�����
		if (!index.isActive_) continue;

		// ���C�̊J�n�n�_���畽�ʂɂ��낵�������̒��������߂�
		Vec3 planeNorm = -index.p1_.normal_;
		float rayToOriginLength = CollisionData.rayPos_.Dot(planeNorm);
		float planeToOriginLength = index.p1_.pos_.Dot(planeNorm);
		// ���_���畽�ʂɂ��낵�������̒���
		float perpendicularLine = rayToOriginLength - planeToOriginLength;

		// �O�p�֐��𗘗p���Ď��_����Փ˓X�܂ł̋��������߂�
		float dist = planeNorm.Dot(CollisionData.rayDir_);
		float impDistance = perpendicularLine / -dist;

		// �Փ˒n�_
		Vec3 impactPoint = CollisionData.rayPos_ + CollisionData.rayDir_ * impDistance;

		/*----- �Փ˓_���|���S���̓����ɂ��邩�𒲂ׂ� -----*/
		Vec3 m;

		/* ��1�{�� */
		Vec3 P1ToImpactPos = (impactPoint - index.p1_.pos_).GetNormal();
		Vec3 P1ToP2 = (index.p2_.pos_ - index.p1_.pos_).GetNormal();
		Vec3 P1ToP3 = (index.p3_.pos_ - index.p1_.pos_).GetNormal();

		// �Փ˓_�ƕ�1�̓���
		float impactDot = P1ToImpactPos.Dot(P1ToP2);
		// �_1�Ɠ_3�̓���
		float P1Dot = P1ToP2.Dot(P1ToP3);

		// �Փ˓_�ƕ�1�̓��ς��_1�Ɠ_3�̓��ς�菬����������A�E�g
		if (impactDot < P1Dot) {
			index.isActive_ = false;
			continue;
		}

		/* ��2�{�� */
		Vec3 P2ToImpactPos = (impactPoint - index.p2_.pos_).GetNormal();
		Vec3 P2ToP3 = (index.p3_.pos_ - index.p2_.pos_).GetNormal();
		Vec3 P2ToP1 = (index.p1_.pos_ - index.p2_.pos_).GetNormal();

		// �Փ˓_�ƕ�2�̓���
		impactDot = P2ToImpactPos.Dot(P2ToP3);
		// �_2�Ɠ_1�̓���
		float P2Dot = P2ToP3.Dot(P2ToP1);

		// �Փ˓_�ƕ�2�̓��ς��_2�Ɠ_1�̓��ς�菬����������A�E�g
		if (impactDot < P2Dot) {
			index.isActive_ = false;
			continue;
		}

		/* ��3�{�� */
		Vec3 P3ToImpactPos = (impactPoint - index.p3_.pos_).GetNormal();
		Vec3 P3ToP1 = (index.p1_.pos_ - index.p3_.pos_).GetNormal();
		Vec3 P3ToP2 = (index.p2_.pos_ - index.p3_.pos_).GetNormal();

		// �Փ˓_�ƕ�3�̓���
		impactDot = P3ToImpactPos.Dot(P3ToP1);
		// �_3�Ɠ_2�̓���
		float P3Dot = P3ToP1.Dot(P3ToP2);

		// �Փ˓_�ƕ�3�̓��ς��_3�Ɠ_2�̓��ς�菬����������A�E�g
		if (impactDot < P3Dot) {
			index.isActive_ = false;
			continue;
		}

		/* �����܂ŗ�����|���S���ɏՓ˂��Ă�I */
		HitPorygonData hitPorygonData;
		hitPorygonData.pos_ = impactPoint;
		hitPorygonData.distance_ = impDistance;
		hitPorygonData.normal_ = index.p1_.normal_;
		hitPorygonData.targetPolygon_ = index;
		hitPolygon.push_back(hitPorygonData);
	}

	// hitPorygon�̒l��1�ȏゾ�����狗�����ŏ��̗v�f������
	if (1 <= hitPolygon.size()) {
		// �������ŏ��̗v�f������
		int min = 0;
		float minDistance = 100000;
		for (auto& index : hitPolygon) {
			if (fabs(index.distance_) < fabs(minDistance)) {
				minDistance = index.distance_;
				min = &index - &hitPolygon[0];
			}
		}

		// ���������ŏ��l��������return
		ImpactPos = hitPolygon[min].pos_;
		Distance = hitPolygon[min].distance_;
		HitNormal = hitPolygon[min].normal_;

		// �d�S���W�����߂�B
		Vec3 bary = FHelper::CalBary(hitPolygon[min].targetPolygon_.p1_.pos_, hitPolygon[min].targetPolygon_.p2_.pos_, hitPolygon[min].targetPolygon_.p3_.pos_, ImpactPos);

		Vec3 baryBuff = bary;

		// UVW�̒l�������̂ŏC���B
		bary.x_ = baryBuff.y_;
		bary.y_ = baryBuff.z_;
		bary.z_ = baryBuff.x_;

		Vec2 uv = Vec2();

		// �d�S���W����UV�����߂�B
		uv.x_ += hitPolygon[min].targetPolygon_.p1_.uv_.x_ * bary.x_;
		uv.x_ += hitPolygon[min].targetPolygon_.p2_.uv_.x_ * bary.y_;
		uv.x_ += hitPolygon[min].targetPolygon_.p3_.uv_.x_ * bary.z_;
		
		uv.y_ += hitPolygon[min].targetPolygon_.p1_.uv_.y_ * bary.x_;
		uv.y_ += hitPolygon[min].targetPolygon_.p2_.uv_.y_ * bary.y_;
		uv.y_ += hitPolygon[min].targetPolygon_.p3_.uv_.y_ * bary.z_;

		HitUV = uv;

		return true;
	}
	else {
		ImpactPos = Vec3{ -1,-1,-1 };
		Distance = -1;
		HitNormal = Vec3{ -1,-1,-1 };
		return false;
	}
}
