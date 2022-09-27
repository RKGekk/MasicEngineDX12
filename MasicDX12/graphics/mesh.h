#pragma once

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3d12.h>

#include <map>
#include <memory>
#include <string>

class CommandList;
class IndexBuffer;
class Material;
class VertexBuffer;
class Visitor;

class Mesh {
public:
	using BufferMap = std::map<uint32_t, std::shared_ptr<VertexBuffer>>;

	Mesh();
	~Mesh() = default;

	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitive_toplogy);
	D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

	void SetVertexBuffer(uint32_t slot_ID, const std::shared_ptr<VertexBuffer>& vertex_buffer);
	std::shared_ptr<VertexBuffer> GetVertexBuffer(uint32_t slot_ID) const;
	const BufferMap& GetVertexBuffers() const;

	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer);
	std::shared_ptr<IndexBuffer> GetIndexBuffer();

	size_t GetIndexCount() const;
	size_t GetVertexCount() const;

	void SetMaterial(std::shared_ptr<Material> material);
	std::shared_ptr<Material> GetMaterial() const;

	void SetAABB(const DirectX::BoundingBox& aabb);
	const DirectX::BoundingBox& GetAABB() const;
	const DirectX::BoundingSphere& GetSphere() const;

	const std::string& GetName() const;
	void SetName(std::string name);

private:
	BufferMap m_vertex_buffers;
	std::shared_ptr<IndexBuffer> m_index_buffer;
	std::shared_ptr<Material> m_material;

	D3D12_PRIMITIVE_TOPOLOGY m_primitive_topology;
	DirectX::BoundingBox m_AABB;
	DirectX::BoundingSphere m_sphere;

	std::string m_name;
};