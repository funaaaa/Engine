#include "ConvertGimmickInfoFromBlender.h"
#include "ModelDataManager.h"
#include "FHelper.h"

void ConvertGimmickInfoFromBlender::Convert(std::string ModelPath, std::string ModelName, const DirectX::XMMATRIX& MatTrans, const DirectX::XMMATRIX& MatRot, const DirectX::XMMATRIX& MatScale)
{

	/*===== ���_�ϊ� =====*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	ModelDataManager::ObjectData dataBuff;

	// ���f�������[�h�B
	ModelDataManager::Ins()->LoadObj(ModelPath, ModelName, dataBuff, false);

	// ���_��ۑ��B
	for (auto& index : dataBuff.vertex_) {

		vertex_.emplace_back(index.pos_);

	}

	// ���_��ϊ��B
	matTrans_ = MatTrans;
	matRot_ = MatRot;
	matScale_ = MatScale;

	// ���[���h�s��𐶐��B
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	matWorld *= matScale_;
	matWorld *= MatRot;
	matWorld *= MatTrans;

	// ���_�Ƀ��[���h�s����|����B
	for (auto& index : vertex_) {

		index = FHelper::MulMat(index, matWorld);

	}

}

Vec3 ConvertGimmickInfoFromBlender::GetVertex(int Index)
{

	/*===== ���_���擾 =====*/

	// �ő�T�C�Y�B
	const int VERTEX_SIZE = static_cast<int>(vertex_.size());

	// �C���f�b�N�X���͈͊O��������B
	if (Index < 0 || VERTEX_SIZE <= Index) assert(0);

	return vertex_[Index];
}
