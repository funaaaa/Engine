#include <d3d12.h>
#include "GLTF.h"
#include <wrl.h>
#include "TextureManager.h"
#include <filesystem>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE  // 書き出しを使用しないため。
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

bool LoadFile(std::vector<char>& out, const std::wstring& fileName)
{
	std::ifstream infile(fileName, std::ios::binary);
	if (!infile) {
		return false;
	}

	out.resize(infile.seekg(0, std::ios::end).tellg());
	infile.seekg(0, std::ios::beg).read(out.data(), out.size());
	return true;
}

#include "FString.h"
void GLTF::LoadModel(std::wstring Path) {

	/*===== GLTFのモデルをロード =====*/

	std::filesystem::path filePath(Path);
	std::vector<char> buffer;
	LoadFile(buffer, Path);

	// 絶対パスを取得。
	std::string baseDir;
	if (filePath.is_relative()) {
		auto current = std::filesystem::current_path();
		current /= filePath;
		current.swap(filePath);
	}
	baseDir = filePath.parent_path().string();

	// ファイルを開く。
	model_ = std::make_shared<tinygltf::Model>();
	std::string err, warn;
	tinygltf::TinyGLTF loader;
	bool result = false;
	if (filePath.extension() == L".glb") {
		result = loader.LoadBinaryFromMemory(model_.get(), &err, &warn,
			reinterpret_cast<const uint8_t*>(buffer.data()),
			uint32_t(buffer.size()), baseDir);
	}
	if (!warn.empty()) {
		OutputDebugStringA(warn.c_str());
	}
	if (!err.empty()) {
		OutputDebugStringA(err.c_str());
	}
	if (!result) {
		assert(0);
	}

	// ルートノードを抜き取る。
	const auto& scene = model_->scenes[0];
	for (const auto& nodeIndex : scene.nodes) {
		rootNodes_.emplace_back(nodeIndex);
	}
	LoadNode(*model_);

	// メッシュの情報を取得。
	LoadMesh(*model_, vertexInfo_);

	// マテリアル情報を取得。
	LoadMaterial(*model_);

	// テクスチャをロード。
	for (auto& texture : model_->textures) {
		fileName_ = FString::StringToWString(model_->images[texture.source].name);
		auto view = model_->bufferViews[model_->images[texture.source].bufferView];
		auto offsetBytes = view.byteOffset;
		const void* src = &model_->buffers[view.buffer].data[offsetBytes];
		UINT64 size = view.byteLength;

		auto t = TextureManager::Ins()->LoadTexture(Path, src, size);
		texture_ = t;
	}

}

void GLTF::LoadNode(const tinygltf::Model& inModel)
{

	/*===== ノードをロード =====*/

	for (auto& inNode : inModel.nodes) {
		nodes_.emplace_back(new Node());
		auto node = nodes_.back();

		node->name = ConvertFromUTF8(inNode.name);
		if (!inNode.translation.empty()) {
			node->translation = MakeFloat3(inNode.translation.data());
		}
		if (!inNode.scale.empty()) {
			node->scale = MakeFloat3(inNode.scale.data());
		}
		if (!inNode.rotation.empty()) {
			node->rotation = MakeQuat(inNode.rotation.data());
		}
		for (auto& c : inNode.children) {
			node->children.push_back(c);
		}
		node->meshIndex = inNode.mesh;
	}
}

void GLTF::LoadMesh(const tinygltf::Model& inModel, VertexAttributeVisitor& visitor)
{

	/*===== メッシュをロード =====*/

	auto& indexBuffer = visitor.indexBuffer;
	auto& positionBuffer = visitor.positionBuffer;
	auto& normalBuffer = visitor.normalBuffer;
	auto& texcoordBuffer = visitor.texcoordBuffer;
	auto& jointBuffer = visitor.jointBuffer;
	auto& weightBuffer = visitor.weightBuffer;
	for (auto& inMesh : inModel.meshes) {
		meshGroup_.emplace_back(MeshGroup());
		auto& meshgrp = meshGroup_.back();

		for (auto& primitive : inMesh.primitives) {
			auto indexStart = static_cast<UINT>(indexBuffer.size());
			auto vertexStart = static_cast<UINT>(positionBuffer.size());
			UINT indexCount = 0, vertexCount = 0;
			bool hasSkin = false;

			const auto& notfound = primitive.attributes.end();
			auto attr = primitive.attributes.find("POSITION");
			if (attr != notfound) {
				auto& acc = inModel.accessors[attr->second];
				auto& view = inModel.bufferViews[acc.bufferView];
				auto offsetBytes = acc.byteOffset + view.byteOffset;
				const auto* src = reinterpret_cast<const Vec3*>(&(inModel.buffers[view.buffer].data[offsetBytes]));

				vertexCount = UINT(acc.count);
				for (UINT i = 0; i < vertexCount; ++i) {
					positionBuffer.push_back(src[i]);
				}
			}
			attr = primitive.attributes.find("NORMAL");
			if (attr != notfound) {
				auto& acc = inModel.accessors[attr->second];
				auto& view = inModel.bufferViews[acc.bufferView];
				auto offsetBytes = acc.byteOffset + view.byteOffset;
				const auto* src = reinterpret_cast<const Vec3*>(&(inModel.buffers[view.buffer].data[offsetBytes]));

				vertexCount = UINT(acc.count);
				for (UINT i = 0; i < vertexCount; ++i) {
					normalBuffer.push_back(src[i]);
				}
			}
			attr = primitive.attributes.find("TEXCOORD_0");
			if (attr != notfound) {
				auto& acc = inModel.accessors[attr->second];
				auto& view = inModel.bufferViews[acc.bufferView];
				auto offsetBytes = acc.byteOffset + view.byteOffset;
				const auto* src = reinterpret_cast<const Vec2*>(&(inModel.buffers[view.buffer].data[offsetBytes]));

				for (UINT i = 0; i < vertexCount; ++i) {
					texcoordBuffer.push_back(src[i]);
				}
			}
			else {
				// UV データが無い場合には、他のものと合わせるべくゼロで埋めておく。
				for (UINT i = 0; i < vertexCount; ++i) {
					texcoordBuffer.push_back(Vec2(0.0f, 0.0f));
				}
			}

			// スキニング用のジョイント(インデックス)番号とウェイト値を読み取る。
			attr = primitive.attributes.find("JOINTS_0");
			if (attr != notfound) {
				auto& acc = inModel.accessors[attr->second];
				auto& view = inModel.bufferViews[acc.bufferView];
				auto offsetBytes = acc.byteOffset + view.byteOffset;
				const auto* src = reinterpret_cast<const uint16_t*>(&(inModel.buffers[view.buffer].data[offsetBytes]));

				for (UINT i = 0; i < vertexCount; ++i) {
					auto idx = i * 4;
					auto v = DirectX::XMUINT4(
						src[idx + 0], src[idx + 1], src[idx + 2], src[idx + 3]);
					jointBuffer.push_back(v);
				}
			}
			attr = primitive.attributes.find("WEIGHTS_0");
			if (attr != notfound) {
				auto& acc = inModel.accessors[attr->second];
				auto& view = inModel.bufferViews[acc.bufferView];
				auto offsetBytes = acc.byteOffset + view.byteOffset;
				const auto* src = reinterpret_cast<const DirectX::XMFLOAT4*>(&(inModel.buffers[view.buffer].data[offsetBytes]));

				for (UINT i = 0; i < vertexCount; ++i) {
					weightBuffer.push_back(src[i]);
				}
			}

			//　インデックスバッファ用。
			{
				auto& acc = inModel.accessors[primitive.indices];
				const auto& view = inModel.bufferViews[acc.bufferView];
				const auto& buffer = inModel.buffers[view.buffer];
				indexCount = UINT(acc.count);
				auto offsetBytes = acc.byteOffset + view.byteOffset;
				if (acc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
					auto src = reinterpret_cast<const uint32_t*>(&(buffer.data[offsetBytes]));

					for (size_t index = 0; index < acc.count; index++)
					{
						indexBuffer.push_back(src[index]);
					}
				}
				if (acc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
					auto src = reinterpret_cast<const uint16_t*>(&(buffer.data[offsetBytes]));
					for (size_t index = 0; index < acc.count; index++)
					{
						indexBuffer.push_back(src[index]);
					}
				}
			}

			meshgrp.m_meshes.emplace_back(Mesh());
			auto& mesh = meshgrp.m_meshes.back();
			mesh.indexStart = indexStart;
			mesh.vertexStart = vertexStart;
			mesh.indexCount = indexCount;
			mesh.vertexCount = vertexCount;
			mesh.materialIndex = primitive.material;
		}
	}

	for (UINT nodeIndex = 0; nodeIndex < UINT(inModel.nodes.size()); ++nodeIndex) {
		auto meshIndex = inModel.nodes[nodeIndex].mesh;
		if (meshIndex < 0) {
			continue;
		}
		meshGroup_[meshIndex].m_nodeIndex = nodeIndex;
	}

}

void GLTF::LoadMaterial(const tinygltf::Model& inModel)
{

	/*===== マテリアルをロード =====*/

	for (const auto& inMaterial : inModel.materials) {

		material_.name_ = ConvertFromUTF8(inMaterial.name);

		material_.baseColor_ = Vec3(inMaterial.pbrMetallicRoughness.baseColorFactor[0], inMaterial.pbrMetallicRoughness.baseColorFactor[1], inMaterial.pbrMetallicRoughness.baseColorFactor[2]);

		// BaseColorは一旦真っ白にする。
		material_.baseColor_ = Vec3(1, 1, 1);

		material_.metalness_ = inMaterial.pbrMetallicRoughness.metallicFactor;
		material_.roughness_ = inMaterial.pbrMetallicRoughness.roughnessFactor;
		material_.specular_ = 0.5f;

	}
}

std::wstring GLTF::ConvertFromUTF8(const std::string& String)
{
	DWORD dwRet = MultiByteToWideChar(CP_UTF8, 0, String.c_str(), -1, NULL, 0);
	std::vector<wchar_t> buf(dwRet);
	dwRet = MultiByteToWideChar(CP_UTF8, 0, String.c_str(), -1, buf.data(), int(buf.size() - 1));
	return std::wstring(buf.data());
}

DirectX::XMVECTOR GLTF::MakeFloat3(const double* In) {
	DirectX::XMFLOAT3 v;
	v.x = static_cast<float>(In[0]);
	v.y = static_cast<float>(In[1]);
	v.z = static_cast<float>(In[2]);
	return DirectX::XMLoadFloat3(&v);
}

DirectX::XMVECTOR GLTF::MakeQuat(const double* In) {
	DirectX::XMFLOAT4 v;
	v.x = static_cast<float>(In[0]);
	v.y = static_cast<float>(In[1]);
	v.z = static_cast<float>(In[2]);
	v.w = static_cast<float>(In[3]);
	return DirectX::XMLoadFloat4(&v);
}