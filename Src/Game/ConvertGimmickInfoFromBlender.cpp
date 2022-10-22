#include "ConvertGimmickInfoFromBlender.h"
#include "ModelDataManager.h"
#include "FHelper.h"

void ConvertGimmickInfoFromBlender::Convert(std::string ModelPath, std::string ModelName, const DirectX::XMMATRIX& MatTrans, const DirectX::XMMATRIX& MatRot, const DirectX::XMMATRIX& MatScale)
{

	/*===== 頂点変換 =====*/

	// 読み込んだデータを一時保存する用のバッファ。
	ModelDataManager::ObjectData dataBuff;

	// モデルをロード。
	ModelDataManager::Ins()->LoadObj(ModelPath, ModelName, dataBuff, false);

	// 頂点を保存。
	for (auto& index : dataBuff.vertex_) {

		vertex_.emplace_back(index.pos_);

	}

	// 頂点を変換。
	matTrans_ = MatTrans;
	matRot_ = MatRot;
	matScale_ = MatScale;

	// ワールド行列を生成。
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	matWorld *= matScale_;
	matWorld *= MatRot;
	matWorld *= MatTrans;

	// 頂点にワールド行列を掛ける。
	for (auto& index : vertex_) {

		index = FHelper::MulMat(index, matWorld);

	}

}

Vec3 ConvertGimmickInfoFromBlender::GetVertex(int Index)
{

	/*===== 頂点を取得 =====*/

	// 最大サイズ。
	const int VERTEX_SIZE = static_cast<int>(vertex_.size());

	// インデックスが範囲外だったら。
	if (Index < 0 || VERTEX_SIZE <= Index) assert(0);

	return vertex_[Index];
}
