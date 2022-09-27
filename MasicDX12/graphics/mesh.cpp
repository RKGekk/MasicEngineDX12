#include "mesh.h"

#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/index_buffer.h"
#include "directx12_wrappers/vertex_buffer.h"

#include <utility>

Mesh::Mesh() : m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}

const Mesh::BufferMap& Mesh::GetVertexBuffers() const {
    return m_vertex_buffers;
}

void Mesh::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitive_toplogy) {
    m_primitive_topology = primitive_toplogy;
}

D3D12_PRIMITIVE_TOPOLOGY Mesh::GetPrimitiveTopology() const {
    return m_primitive_topology;
}

void Mesh::SetVertexBuffer(uint32_t slot_ID, const std::shared_ptr<VertexBuffer>& vertex_buffer) {
    m_vertex_buffers[slot_ID] = vertex_buffer;
}

std::shared_ptr<VertexBuffer> Mesh::GetVertexBuffer(uint32_t slot_ID) const {
    auto iter = m_vertex_buffers.find(slot_ID);
    auto vertex_buffer = iter != m_vertex_buffers.end() ? iter->second : nullptr;

    return vertex_buffer;
}

void Mesh::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer) {
    m_index_buffer = index_buffer;
}

std::shared_ptr<IndexBuffer> Mesh::GetIndexBuffer() {
    return m_index_buffer;
}

size_t Mesh::GetIndexCount() const {
    size_t index_count = 0;
    if (m_index_buffer) {
        index_count = m_index_buffer->GetNumIndices();
    }

    return index_count;
}

size_t Mesh::GetVertexCount() const {
    size_t vertex_count = 0u;

    BufferMap::const_iterator iter = m_vertex_buffers.cbegin();
    if (iter != m_vertex_buffers.cend()) {
        vertex_count = iter->second->GetNumVertices();
    }

    return vertex_count;
}

void Mesh::SetMaterial(std::shared_ptr<Material> material) {
    m_material = material;
}

std::shared_ptr<Material> Mesh::GetMaterial() const {
    return m_material;
}

void Mesh::SetAABB(const DirectX::BoundingBox& aabb) {
    m_AABB = aabb;
    DirectX::BoundingSphere::CreateFromBoundingBox(m_sphere, m_AABB);
}

const DirectX::BoundingBox& Mesh::GetAABB() const {
    return m_AABB;
}

const DirectX::BoundingSphere& Mesh::GetSphere() const {
    return m_sphere;
}

const std::string& Mesh::GetName() const {
    return m_name;
}

void Mesh::SetName(std::string name) {
    m_name = std::move(name);
}
