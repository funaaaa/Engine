#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "Vec.h"

class ConvertGimmickInfoFromBlender {

private:

	/*===== �����o�ϐ� =====*/

	DirectX::XMMATRIX matTrans_;
	DirectX::XMMATRIX matRot_;
	DirectX::XMMATRIX matScale_;
	std::vector<Vec3> vertex_;


public:

	/*===== �����o�֐� =====*/

	void Convert(std::string ModelPath, std::string ModelName, const DirectX::XMMATRIX& MatTrans, const DirectX::XMMATRIX& MatRot, const DirectX::XMMATRIX& MatScale);

	// �w�肵���C���f�b�N�X�̒��_���擾�B
	Vec3 GetVertex(const int& Index);

	int GetVertexSize() { return static_cast<int>(vertex_.size()); }

};