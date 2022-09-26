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

// GLTFモデルクラス
class GLTF {

private:

	/*===== メンバ変数 =====*/

	// データ引き抜き用構造体。
	struct VertexAttributeVisitor {
		std::vector<UINT> indexBuffer;
		std::vector<Vec3> positionBuffer;
		std::vector<Vec3> normalBuffer;
		std::vector<Vec2> texcoordBuffer;

		std::vector<DirectX::XMUINT4> jointBuffer;
		std::vector<DirectX::XMFLOAT4> weightBuffer;
	};

	// 各階層や関節を表現するノードクラス。
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

	// ポリゴン情報。
	class Mesh {
	public:
		UINT indexStart;
		UINT vertexStart;
		UINT indexCount;
		UINT vertexCount;
		UINT materialIndex;
	};

	// マテリアル。
	class Material {
	public:
		Material() : name_(), textureIndex_(-1), diffuseColor_(1, 1, 1) { }
		std::wstring name_;
		int textureIndex_;
		Vec3 diffuseColor_;
	};

	// 同じノードに関連するポリゴンメッシュを束ねたデータ。
	class MeshGroup {
	public:
		std::vector<Mesh> m_meshes;
		int m_nodeIndex;
		friend class DxrModel;
	};

	// ノード
	std::vector<int> rootNodes_;
	std::vector<std::shared_ptr<Node>> nodes_;

	// メッシュデータ
	std::vector<MeshGroup> meshGroup_;

	// 各データ
	VertexAttributeVisitor vertexInfo_;
	std::vector<Material> materials_;
	std::vector<int> texture_;


public:

	/*===== メンバ関数 =====*/

	void LoadModel(std::wstring DirectoryPath, std::wstring ModelName);


private:

	void LoadNode(const tinygltf::Model& Model);
	void LoadMesh(const tinygltf::Model& Model, VertexAttributeVisitor& Visitor);
	void LoadMaterial(const tinygltf::Model& Model);
	std::wstring ConvertFromUTF8(const std::string& String);
	DirectX::XMVECTOR MakeFloat3(const double* In);
	DirectX::XMVECTOR MakeQuat(const double* In);

};