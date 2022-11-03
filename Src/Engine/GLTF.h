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

public:

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
		DirectX::XMVECTOR translation;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR scale;
		DirectX::XMMATRIX mtxLocal;
		DirectX::XMMATRIX mtxWorld;
		std::vector<int> children;
		std::wstring name;
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
	struct Material {
		Vec3 baseColor_;
		float metalness_;
		float specular_;
		float roughness_;
		std::wstring name_;
		Material() :baseColor_(Vec3()), metalness_(0.5f), specular_(0.5f), roughness_(0.5f) {};
	};

	// �����m�[�h�Ɋ֘A����|���S�����b�V���𑩂˂��f�[�^�B
	class MeshGroup {
	public:
		std::vector<Mesh> m_meshes;
		int m_nodeIndex;
		friend class DxrModel;
	};

private:

	// �m�[�h
	std::vector<int> rootNodes_;
	std::vector<std::shared_ptr<Node>> nodes_;

	// ���b�V���f�[�^
	std::vector<MeshGroup> meshGroup_;

	// �e�f�[�^
	VertexAttributeVisitor vertexInfo_;
	Material material_;
	int texture_;

	// �e�N�X�`���ۑ��p
	std::wstring fileName_;
	std::shared_ptr<tinygltf::Model> model_;


public:

	/*===== �����o�֐� =====*/

	void LoadModel(std::wstring Path);

	// �Q�b�^
	VertexAttributeVisitor GetVertexData() { return vertexInfo_; }
	int GetTextureIndex() { return texture_; }
	GLTF::Material GetMaterial() { return material_; }
	std::wstring GetFileName() { return fileName_; }


private:

	void LoadNode(const tinygltf::Model& Model);
	void LoadMesh(const tinygltf::Model& Model, VertexAttributeVisitor& Visitor);
	void LoadMaterial(const tinygltf::Model& Model);
	std::wstring ConvertFromUTF8(const std::string& String);
	DirectX::XMVECTOR MakeFloat3(const double* In);
	DirectX::XMVECTOR MakeQuat(const double* In);

};