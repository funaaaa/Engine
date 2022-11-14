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

public:

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
	struct Material {
		Vec3 baseColor_;
		float metalness_;
		float specular_;
		float roughness_;
		std::wstring name_;
		Material() :baseColor_(Vec3()), metalness_(0.5f), specular_(0.5f), roughness_(0.5f) {};
	};

	// 同じノードに関連するポリゴンメッシュを束ねたデータ。
	class MeshGroup {
	public:
		std::vector<Mesh> m_meshes;
		int m_nodeIndex;
		friend class DxrModel;
	};

private:

	// ノード
	std::vector<int> rootNodes_;
	std::vector<std::shared_ptr<Node>> nodes_;

	// メッシュデータ
	std::vector<MeshGroup> meshGroup_;

	// 各データ
	VertexAttributeVisitor vertexInfo_;
	Material material_;
	int texture_;

	// テクスチャ保存用
	std::wstring fileName_;
	std::shared_ptr<tinygltf::Model> model_;


public:

	/*===== メンバ関数 =====*/

	void LoadModel(std::wstring Path);

	// ゲッタ
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