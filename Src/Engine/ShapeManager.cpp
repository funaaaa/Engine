#include "ShapeManager.h"

//vector<ShapeData> ShapeManager::shapeData = {};
//
//void ShapeManager::LoadShape(int shapeID, Sprite& proSprite)
//{
//	//�}�`�������ς݂��ǂ������m�F����
//	bool isLoad = false;
//	int dataNum = 0;
//	for (int i = 0; i < shapeData.size(); ++i) {
//		if (shapeData.at(i).dataID == shapeID) {
//			isLoad = true;
//			dataNum = i;
//		}
//	}
//
//	//��������������}�`�𐶐����Ēl���R�s�[����
//	if (isLoad == false) {
//		//��������}�`�ɂ���ď����𕪂���
//		switch (shapeID) {
//		case ShapeDataCone:
//			CreateCone(proSprite);
//			break;
//		case ShapeDataCylinder:
//			CreateCylinder(proSprite);
//			break;
//		case ShapeDataSphere:
//			CreateSphere(proSprite);
//			break;
//		}
//	}
//	//�����ς݂�����������R�s�[����
//	else {
//		for (int i = 0; i < shapeData.at(dataNum).index.size(); ++i) {
//			proSprite.index.push_back(shapeData.at(dataNum).index.at(i));
//		}
//		for (int i = 0; i < shapeData.at(dataNum).vertex.size(); ++i) {
//			proSprite.vertex.push_back(shapeData.at(dataNum).vertex.at(i));
//		}
//	}
//}
//
//void ShapeManager::CreateCone(Sprite& proSprite)
//{
//	//shapedata�𐶐�
//	shapeData.push_back({});
//	shapeData.at(shapeData.size() - 1).dataID = ShapeDataCone;
//	//���_�o�b�t�@�̐���
//	int vertexCount = 30;			//���_��
//	//��ʂ̒��S�ȊO�̍��W���v�Z
//	for (int i = 0; i < vertexCount - 2; ++i) {
//		Vertex pos;
//		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (vertexCount - 2) * i),
//			1 * (cosf(XM_2PI / (vertexCount - 2) * i)),
//			0);
//		pos.uv = XMFLOAT2(0, 0);
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//		//prosprite�ւ̃R�s�[���s��
//		proSprite.vertex.push_back(pos);
//	}
//	Vertex circleCenterPos{};
//	circleCenterPos.pos = XMFLOAT3(0, 0, 0);
//	circleCenterPos.uv = XMFLOAT2(0, 0);
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).vertex.push_back(circleCenterPos);
//	//prosprite�ւ̃R�s�[���s��
//	proSprite.vertex.push_back(circleCenterPos);
//	Vertex topHeightPos{};
//	topHeightPos.pos = XMFLOAT3(0, 0, -1);
//	topHeightPos.uv = XMFLOAT2(0, 0);
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).vertex.push_back(topHeightPos);
//	//prosprite�ւ̃R�s�[���s��
//	proSprite.vertex.push_back(topHeightPos);
//
//	//��ʂ̃C���f�b�N�X�̐ݒ�
//	int i = 0;
//	for (; i < vertexCount - 3; ++i) {
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		//prosprite�ւ̃R�s�[���s��
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(vertexCount - 2);
//		proSprite.index.push_back(i + 1);
//	}
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	//prosprite�ւ̃R�s�[���s��
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(vertexCount - 2);
//	proSprite.index.push_back(0);
//	//���ʂ̃C���f�b�N�X�̐ݒ�
//	i = 0;
//	for (; i < vertexCount - 3; ++i) {
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//		//prosprite�ւ̃R�s�[���s��
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(i + 1);
//		proSprite.index.push_back(vertexCount - 1);
//	}
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//	//prosprite�ւ̃R�s�[���s��
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(vertexCount - 1);
//
//	//�@���x�N�g���̌v�Z
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//�O�p�`�ЂƂЂƂv�Z���Ă���
//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1�x�N�g��,p0->p2�x�N�g�����v�Z
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//�O�ʂ͗������琂���ȃx�N�g��
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//���K��
//		normal = XMVector3Normalize(normal);
//		//���߂��@���𒸓_�f�[�^�ɑ��
//		//shapedeta�֕ۑ�
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index0).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index1).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index2).normal, normal);
//		//prosprite�ւ̃R�s�[���s��
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//}
//
//void ShapeManager::CreateCylinder(Sprite& proSprite)
//{
//	//shapedata�𐶐�
//	shapeData.push_back({});
//	shapeData.at(shapeData.size() - 1).dataID = ShapeDataCylinder;
//	//���_�o�b�t�@�̐���
//	int vertexCount = 30;			//���_��
//	//��ʂ̍��W���v�Z
//	for (int i = 0; i < vertexCount / 2 - 1; ++i) {
//		Vertex pos;
//		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (vertexCount / 2 - 1) * i),
//			1 * (cosf(XM_2PI / (vertexCount / 2 - 1) * i)),
//			0);
//		pos.uv = XMFLOAT2(0, 0);
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//		//proSprite�ւ̃f�[�^�R�s�[���s��
//		proSprite.vertex.push_back(pos);
//	}
//	Vertex circleCenterPos{};
//	circleCenterPos.pos = XMFLOAT3(0, 0, 0);
//	circleCenterPos.uv = XMFLOAT2(0, 0);
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).vertex.push_back(circleCenterPos);
//	//proSprite�ւ̃f�[�^�R�s�[���s��
//	proSprite.vertex.push_back(circleCenterPos);
//	//�V�ʂ̍��W���v�Z
//	for (int i = 0; i < vertexCount / 2 - 1; ++i) {
//		Vertex pos;
//		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (vertexCount / 2 - 1) * i),
//			1 * (cosf(XM_2PI / (vertexCount / 2 - 1) * i)),
//			-1);
//		pos.uv = XMFLOAT2(0, 0);
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//		//proSprite�ւ̃f�[�^�R�s�[���s��
//		proSprite.vertex.push_back(pos);
//	}
//	Vertex topHeightPos{};
//	topHeightPos.pos = XMFLOAT3(0, 0, -1);
//	topHeightPos.uv = XMFLOAT2(0, 0);
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).vertex.push_back(topHeightPos);
//	//proSprite�ւ̃f�[�^�R�s�[���s��
//	proSprite.vertex.push_back(topHeightPos);
//
//	//��ʂ̃C���f�b�N�X�̐ݒ�
//	int i = 0;
//	for (; i < vertexCount / 2 - 2; ++i) {
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2 - 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		//proSprite�ւ̃f�[�^�R�s�[���s��
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(vertexCount / 2 - 1);
//		proSprite.index.push_back(i + 1);
//	}
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2 - 1);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	//proSprite�ւ̃f�[�^�R�s�[���s��
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(vertexCount / 2 - 1);
//	proSprite.index.push_back(0);
//	//�V�ʂ̃C���f�b�N�X�̐ݒ�
//	i = 0;
//	for (; i < vertexCount / 2 - 2; ++i) {
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1 + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//		//proSprite�ւ̃f�[�^�R�s�[���s��
//		proSprite.index.push_back(i + vertexCount / 2);
//		proSprite.index.push_back(i + 1 + vertexCount / 2);
//		proSprite.index.push_back(vertexCount - 1);
//	}
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//	//proSprite�ւ̃f�[�^�R�s�[���s��
//	proSprite.index.push_back(i + vertexCount / 2);
//	proSprite.index.push_back(vertexCount / 2);
//	proSprite.index.push_back(vertexCount - 1);
//	//���ʂ̃C���f�b�N�X�̐ݒ�
//	i = 0;
//	for (; i < vertexCount / 2 - 2; ++i) {
//		//shapedeta�֕ۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1 + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//		//proSprite�ւ̃f�[�^�R�s�[���s��
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(i + 1);
//		proSprite.index.push_back(i + vertexCount / 2);
//		proSprite.index.push_back(i + 1);
//		proSprite.index.push_back(i + 1 + vertexCount / 2);
//		proSprite.index.push_back(i + vertexCount / 2);
//	}
//	//shapedeta�֕ۑ�
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//	//proSprite�ւ̃f�[�^�R�s�[���s��
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(i + vertexCount / 2);
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(vertexCount / 2);
//	proSprite.index.push_back(i + vertexCount / 2);
//
//	//�@���x�N�g���̌v�Z
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//�O�p�`�ЂƂЂƂv�Z���Ă���
//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1�x�N�g��,p0->p2�x�N�g�����v�Z
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//�O�ʂ͗������琂���ȃx�N�g��
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//���K��
//		normal = XMVector3Normalize(normal);
//		//���߂��@���𒸓_�f�[�^�ɑ��
//		//shapedeta�֕ۑ�
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index0).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index1).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index2).normal, normal);
//		//proSprite�ւ̃f�[�^�R�s�[���s��
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//}
//
//void ShapeManager::CreateSphere(Sprite& proSprite)
//{
//	//shapedata�𐶐�
//	shapeData.push_back({});
//	shapeData.at(shapeData.size() - 1).dataID = ShapeDataSphere;
//	//���_�o�b�t�@�̐���
//	int vertexCount = 20;			//���_��
//	Vertex pos;
//	//��ʂ̍��W
//	pos.pos = XMFLOAT3(0, 0, -1);
//	pos.uv = XMFLOAT2(0, 0);
//	//shapeData�ɕۑ�
//	shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//	//proSprite�ɂ��f�[�^���R�s�[
//	proSprite.vertex.push_back(pos);
//	//���ʂ̍��W�̌v�Z
//	for (int i = 1; i < vertexCount; ++i) {
//		for (int j = 0; j < vertexCount; ++j) {
//			float circleRadius = (1 * sinf(XM_PI / vertexCount * i));
//			pos.pos = XMFLOAT3(circleRadius * sinf(XM_2PI / vertexCount * j),
//				circleRadius * (cosf(XM_2PI / vertexCount * j)),
//				-1 * (cosf((XM_PI / vertexCount * i)))
//			);
//			pos.uv = XMFLOAT2(0, 0);
//			//shapeData�ɕۑ�
//			shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//			//proSprite�ɂ��f�[�^���R�s�[
//			proSprite.vertex.push_back(pos);
//		}
//	}
//	//�V�ʂ̍��W
//	pos.pos = XMFLOAT3(0, 0, 1);
//	pos.uv = XMFLOAT2(0, 0);
//	//shapeData�ɕۑ�
//	shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//	//proSprite�ɂ��f�[�^���R�s�[
//	proSprite.vertex.push_back(pos);
//
//	//��ʂ̃C���f�b�N�X�̐ݒ�
//	int buff = 1;
//	for (int i = 0; i < vertexCount - 1; ++i) {
//		//shapeData�ɕۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(buff);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(0);
//		//proSprite�ɂ��f�[�^���R�s�[
//		proSprite.index.push_back(buff);
//		proSprite.index.push_back(buff + 1);
//		proSprite.index.push_back(0);
//		buff++;
//	}
//	//shapeData�ɕۑ�
//	shapeData.at(shapeData.size() - 1).index.push_back(buff);
//	shapeData.at(shapeData.size() - 1).index.push_back(1);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	//proSprite�ɂ��f�[�^���R�s�[
//	proSprite.index.push_back(buff);
//	proSprite.index.push_back(1);
//	proSprite.index.push_back(0);
//	buff++;
//	//���ʂ̃C���f�b�N�X�̐ݒ�
//	for (int i = 0; i < vertexCount - 2; ++i) {
//		for (int j = 0; j < vertexCount - 1; ++j) {
//			//shapeData�ɕۑ�
//			shapeData.at(shapeData.size() - 1).index.push_back(buff);
//			shapeData.at(shapeData.size() - 1).index.push_back(buff + 1);
//			shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1));
//			shapeData.at(shapeData.size() - 1).index.push_back(buff);
//			shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1));
//			shapeData.at(shapeData.size() - 1).index.push_back(buff - vertexCount);
//			//proSprite�ɂ��f�[�^���R�s�[
//			proSprite.index.push_back(buff);
//			proSprite.index.push_back(buff + 1);
//			proSprite.index.push_back(buff - (vertexCount - 1));
//			proSprite.index.push_back(buff);
//			proSprite.index.push_back(buff - (vertexCount - 1));
//			proSprite.index.push_back(buff - vertexCount);
//			buff++;
//		}
//		//shapeData�ɕۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(buff);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1));
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1) - vertexCount);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1) - vertexCount);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - vertexCount);
//		//proSprite�ɂ��f�[�^���R�s�[
//		proSprite.index.push_back(buff);
//		proSprite.index.push_back(buff - (vertexCount - 1));
//		proSprite.index.push_back(buff - (vertexCount - 1) - vertexCount);
//		proSprite.index.push_back(buff);
//		proSprite.index.push_back(buff - (vertexCount - 1) - vertexCount);
//		proSprite.index.push_back(buff - vertexCount);
//		buff++;
//	}
//	//�V�ʂ̃C���f�b�N�X�̐ݒ�
//	for (int i = vertexCount - 1; i > 0; --i) {
//		//shapeData�ɕۑ�
//		shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - i);
//		shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - i - 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1);
//		//proSprite�ɂ��f�[�^���R�s�[
//		proSprite.index.push_back(proSprite.vertex.size() - 1 - i);
//		proSprite.index.push_back(proSprite.vertex.size() - 1 - i - 1);
//		proSprite.index.push_back(proSprite.vertex.size() - 1);
//	}
//	//shapeData�ɕۑ�
//	shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - 1);
//	shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1);
//	shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - vertexCount);
//	//proSprite�ɂ��f�[�^���R�s�[
//	proSprite.index.push_back(proSprite.vertex.size() - 1 - 1);
//	proSprite.index.push_back(proSprite.vertex.size() - 1);
//	proSprite.index.push_back(proSprite.vertex.size() - 1 - vertexCount);
//
//	//�@���x�N�g���̌v�Z
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//�O�p�`�ЂƂЂƂv�Z���Ă���
//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1�x�N�g��,p0->p2�x�N�g�����v�Z
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//�O�ʂ͗������琂���ȃx�N�g��
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//���K��
//		normal = XMVector3Normalize(normal);
//		//���߂��@���𒸓_�f�[�^�ɑ��
//		//shapeData�ɕۑ�
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index0).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index1).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index2).normal, normal);
//		//proSprite�ɂ��f�[�^���R�s�[
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//}
