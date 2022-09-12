#include "index_buffer.h"

#include <directx/d3dx12.h>
#include <cassert>

D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetIndexBufferView() const {
    return m_index_buffer_view;
}

size_t IndexBuffer::GetNumIndices() const {
    return m_num_indices;
}

DXGI_FORMAT IndexBuffer::GetIndexFormat() const {
    return m_index_format;
}

IndexBuffer::IndexBuffer(Device& device, size_t num_indices, DXGI_FORMAT index_format) : Buffer(device, CD3DX12_RESOURCE_DESC::Buffer(num_indices* (index_format == DXGI_FORMAT_R16_UINT ? 2 : 4))), m_num_indices(num_indices), m_index_format(index_format), m_index_buffer_view{} {
    assert(index_format == DXGI_FORMAT_R16_UINT || index_format == DXGI_FORMAT_R32_UINT);
    CreateIndexBufferView();
}

IndexBuffer::IndexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_indices, DXGI_FORMAT index_format) : Buffer(device, resource), m_num_indices(num_indices), m_index_format(index_format), m_index_buffer_view{} {
    assert(index_format == DXGI_FORMAT_R16_UINT || index_format == DXGI_FORMAT_R32_UINT);
    CreateIndexBufferView();
}

void IndexBuffer::CreateIndexBufferView() {
    UINT bufferSize = m_num_indices * (m_index_format == DXGI_FORMAT_R16_UINT ? 2 : 4);

    m_index_buffer_view.BufferLocation = m_d3d12_resource->GetGPUVirtualAddress();
    m_index_buffer_view.SizeInBytes = bufferSize;
    m_index_buffer_view.Format = m_index_format;
}