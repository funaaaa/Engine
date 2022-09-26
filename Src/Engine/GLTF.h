#pragma once
#include "Vec.h"
#include <vector>
#include <wtypes.h>
#include <memory>
#include <string>
#include <DirectXMath.h>

namespace tinygltf {
class Model;
}

// GLTF���f���N���X
class GLTF {

private:

	/*===== �����o�ϐ� =====*/

	// �f�[�^���������p�\���́B
	struct VertexAttributeVisitor {
		std::vector<UINT> indexBuffer;
		std::vector<Vec3> positionBuffer;
		std::vector<Vec3> normalBuffer;
		std::vector<Vec2> texcoordBuffer;

		std::vector<DirectX::XMUINT4> jointBuffer;
		std::vector<DirectX::XMFLOAT4> weightBuffer;
	};

	// �e�K�w��֐߂�\������m�[�h�N���X�B
	class Node {
	public:
		Node() {};
		~Node() {};

	public:
		std::wstring name;
		DirectX::XMVECTOR translation;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR scale;
		DirectX::XMMATRIX mtxLocal;
		DirectX::XMMATRIX mtxWorld;
		std::vector<int> children;
		Node* parent = nullptr;
		int meshIndex = -1;
	};

	// �|���S�����B
	class Mesh {
	public:
		UINT indexStart;
		UINT vertexStart;
		UINT indexCount;
		UINT vertexCount;
		UINT materialIndex;
	};

	// �}�e���A���B
	class Material {
	public:
		Material() : name_(), textureIndex_(-1), diffuseColor_(1, 1, 1) { }
		std::wstring name_;
		int textureIndex_;
		Vec3 diffuseColor_;
	};

	// �����m�[�h�Ɋ֘A����|���S�����b�V���𑩂˂��f�[�^�B
	class MeshGroup {
	public:
		std::vector<Mesh> m_meshes;
		int m_nodeIndex;
		friend class DxrModel;
	};

	// �m�[�h
	std::vector<int> rootNodes_;
	std::vector<std::shared_ptr<Node>> nodes_;

	// ���b�V���f�[�^
	std::vector<MeshGroup> meshGroup_;

	// �e�f�[�^
	VertexAttributeVisitor vertexInfo_;
	std::vector<Material> materials_;
	std::vector<int> texture_;


public:

	/*===== �����o�֐� =====*/

	void LoadModel(std::wstring DirectoryPath, std::wstring ModelName);


private:

	void LoadNode(const tinygltf::Model& Model);
	void LoadMesh(const tinygltf::Model& Model, VertexAttributeVisitor& Visitor);
	void LoadMaterial(const tinygltf::Model& Model);
	std::wstring ConvertFromUTF8(const std::string& String);
	DirectX::XMVECTOR MakeFloat3(const double* In);
	DirectX::XMVECTOR MakeQuat(const double* In);

};