#include "upload_buffer.h"

#include <directx/d3dx12.h>

#include "device.h"
#include "../tools/com_exception.h"
#include "../tools/math_utitity.h"

#include <new>

UploadBuffer::UploadBuffer(Device& device, size_t page_size) : m_device(device), m_page_size(page_size) {}

UploadBuffer::~UploadBuffer() {}

size_t UploadBuffer::GetPageSize() const {
    return m_page_size;
}

UploadBuffer::Allocation UploadBuffer::Allocate(size_t size_in_bytes, size_t alignment) {
    if (size_in_bytes > m_page_size) {
        throw std::bad_alloc();
    }

    if (!m_current_page || !m_current_page->HasSpace(size_in_bytes, alignment)) {
        m_current_page = RequestPage();
    }

    return m_current_page->Allocate(size_in_bytes, alignment);
}

std::shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage() {
    std::shared_ptr<Page> page;

    if (!m_available_pages.empty()) {
        page = m_available_pages.front();
        m_available_pages.pop_front();
    }
    else {
        page = std::make_shared<Page>(m_device, m_page_size);
        m_page_pool.push_back(page);
    }

    return page;
}

void UploadBuffer::Reset() {
    m_current_page.reset();
    m_available_pages = m_page_pool;

    for (auto page : m_available_pages) {
        page->Reset();
    }
}

UploadBuffer::Page::Page(Device& device, size_t size_in_bytes) : m_device(device), m_page_size(size_in_bytes), m_offset(0u), m_cpu_ptr(nullptr), m_gpu_ptr(D3D12_GPU_VIRTUAL_ADDRESS(0ul)) {
    auto d3d12_device = m_device.GetD3D12Device();
    D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC resource = CD3DX12_RESOURCE_DESC::Buffer(m_page_size);
    HRESULT hr = d3d12_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &resource, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_d3d12_resource.GetAddressOf()));
    ThrowIfFailed(hr);

    m_d3d12_resource->SetName(L"Upload Buffer (Page)");

    m_gpu_ptr = m_d3d12_resource->GetGPUVirtualAddress();
    m_d3d12_resource->Map(0, nullptr, &m_cpu_ptr);
}

UploadBuffer::Page::~Page() {
    m_d3d12_resource->Unmap(0u, nullptr);
    m_cpu_ptr = nullptr;
    m_gpu_ptr = D3D12_GPU_VIRTUAL_ADDRESS(0ul);
}

bool UploadBuffer::Page::HasSpace(size_t size_in_bytes, size_t alignment) const {
    size_t aligned_size = Math::AlignUp(size_in_bytes, alignment);
    size_t aligned_offset = Math::AlignUp(m_offset, alignment);

    return aligned_offset + aligned_size <= m_page_size;
}

UploadBuffer::Allocation UploadBuffer::Page::Allocate(size_t size_in_bytes, size_t alignment) {
    if (!HasSpace(size_in_bytes, alignment)) {
        throw std::bad_alloc();
    }

    size_t aligned_size = Math::AlignUp(size_in_bytes, alignment);
    m_offset = Math::AlignUp(m_offset, alignment);

    Allocation allocation;
    allocation.CPU = static_cast<uint8_t*>(m_cpu_ptr) + m_offset;
    allocation.GPU = m_gpu_ptr + m_offset;

    m_offset += aligned_size;

    return allocation;
}

void UploadBuffer::Page::Reset() {
    m_offset = 0;
}