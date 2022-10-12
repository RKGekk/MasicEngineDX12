#include "vertex_buffer.h"

#include <directx_old/d3dx12.h>

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetVertexBufferView() const {
    return m_vertex_buffer_view;
}

size_t VertexBuffer::GetNumVertices() const {
    return m_num_vertices;
}

size_t VertexBuffer::GetVertexStride() const {
    return m_vertex_stride;
}

VertexBuffer::VertexBuffer(Device& device, size_t num_vertices, size_t vertex_stride) : Buffer(device, CD3DX12_RESOURCE_DESC::Buffer(num_vertices * vertex_stride)), m_num_vertices(num_vertices), m_vertex_stride(vertex_stride), m_vertex_buffer_view{} {
    CreateVertexBufferView();
}

VertexBuffer::VertexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_vertices, size_t vertex_stride) : Buffer(device, resource), m_num_vertices(num_vertices), m_vertex_stride(vertex_stride), m_vertex_buffer_view{} {
    CreateVertexBufferView();
}

VertexBuffer::~VertexBuffer() {}

void VertexBuffer::CreateVertexBufferView() {
    m_vertex_buffer_view.BufferLocation = m_d3d12_resource->GetGPUVirtualAddress();
    m_vertex_buffer_view.SizeInBytes = static_cast<UINT>(m_num_vertices * m_vertex_stride);
    m_vertex_buffer_view.StrideInBytes = static_cast<UINT>(m_vertex_stride);
}