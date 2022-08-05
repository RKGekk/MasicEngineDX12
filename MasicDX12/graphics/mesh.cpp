#include "mesh.h"

#include "directx12_wrappers/command_list.h"
#include "directx12_wrappers/index_buffer.h"
#include "directx12_wrappers/vertex_buffer.h"

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

void Mesh::Draw(CommandList& command_list, uint32_t instance_count, uint32_t start_instance) {
    command_list.SetPrimitiveTopology(GetPrimitiveTopology());

    for (auto vertex_buffer : m_vertex_buffers) {
        command_list.SetVertexBuffer(vertex_buffer.first, vertex_buffer.second);
    }

    auto index_count = GetIndexCount();
    auto vertex_count = GetVertexCount();

    if (index_count > 0) {
        command_list.SetIndexBuffer(m_index_buffer);
        command_list.DrawIndexed(index_count, instance_count, 0u, 0u, start_instance);
    }
    else if (vertex_count > 0) {
        command_list.Draw(vertex_count, instance_count, 0u, start_instance);
    }
}

void Mesh::SetAABB(const DirectX::BoundingBox& aabb) {
    m_AABB = aabb;
}

const DirectX::BoundingBox& Mesh::GetAABB() const {
    return m_AABB;
}