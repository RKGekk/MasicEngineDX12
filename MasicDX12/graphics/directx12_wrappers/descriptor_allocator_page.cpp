#include "descriptor_allocator_page.h"

#include "command_queue.h"
#include "device.h"
#include "../tools/com_exception.h"

DescriptorAllocatorPage::DescriptorAllocatorPage(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors) : m_device(device), m_heap_type(type), m_num_descriptors_in_heap(num_descriptors) {
    auto d3d12_device = m_device.GetD3D12Device();

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.Type = m_heap_type;
    heap_desc.NumDescriptors = m_num_descriptors_in_heap;

    HRESULT hr = d3d12_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_d3d12_descriptor_heap.GetAddressOf()));
    ThrowIfFailed(hr);

    m_base_descriptor = m_d3d12_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
    m_descriptor_handle_increment_size = d3d12_device->GetDescriptorHandleIncrementSize(m_heap_type);
    m_num_free_handles = m_num_descriptors_in_heap;

    AddNewBlock(0, m_num_free_handles);
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorAllocatorPage::GetHeapType() const {
    return m_heap_type;
}

uint32_t DescriptorAllocatorPage::NumFreeHandles() const {
    return m_num_free_handles;
}

bool DescriptorAllocatorPage::HasSpace(uint32_t num_descriptors) const {
    return m_free_list_by_size.lower_bound(num_descriptors) != m_free_list_by_size.end();
}

void DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t num_descriptors) {
    auto offset_it = m_free_list_by_offset.emplace(offset, num_descriptors);
    auto size_it = m_free_list_by_size.emplace(num_descriptors, offset_it.first);
    offset_it.first->second.FreeListBySizeIt = size_it;
}

DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t num_descriptors) {
    std::lock_guard<std::mutex> lock(m_allocation_mutex);

    if (num_descriptors > m_num_free_handles) {
        return DescriptorAllocation();
    }

    auto smallest_block_it = m_free_list_by_size.lower_bound(num_descriptors);
    if (smallest_block_it == m_free_list_by_size.end()) {
        return DescriptorAllocation();
    }

    auto block_size = smallest_block_it->first;
    auto offset_it = smallest_block_it->second;
    auto offset = offset_it->first;

    m_free_list_by_size.erase(smallest_block_it);
    m_free_list_by_offset.erase(offset_it);

    auto new_offset = offset + num_descriptors;
    auto new_size = block_size - num_descriptors;

    if (new_size > 0) {
        AddNewBlock(new_offset, new_size);
    }

    m_num_free_handles -= num_descriptors;

    D3D12_CPU_DESCRIPTOR_HANDLE desc_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_base_descriptor, offset, m_descriptor_handle_increment_size);
    return DescriptorAllocation(desc_handle, num_descriptors, m_descriptor_handle_increment_size, shared_from_this());
}

uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
    return static_cast<uint32_t>(handle.ptr - m_base_descriptor.ptr) / m_descriptor_handle_increment_size;
}

void DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptor) {
    auto offset = ComputeOffset(descriptor.GetDescriptorHandle());

    std::lock_guard<std::mutex> lock(m_allocation_mutex);
    m_stale_descriptors.emplace(offset, descriptor.GetNumHandles());
}

void DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t num_descriptors) {
    auto next_block_it = m_free_list_by_offset.upper_bound(offset);
    auto prev_block_it = next_block_it;

    if (prev_block_it != m_free_list_by_offset.begin()) {
        --prev_block_it;
    }
    else {
        prev_block_it = m_free_list_by_offset.end();
    }

    m_num_free_handles += num_descriptors;

    if (prev_block_it != m_free_list_by_offset.end() && offset == prev_block_it->first + prev_block_it->second.Size) {
        offset = prev_block_it->first;
        num_descriptors += prev_block_it->second.Size;

        m_free_list_by_size.erase(prev_block_it->second.FreeListBySizeIt);
        m_free_list_by_offset.erase(prev_block_it);
    }

    if (next_block_it != m_free_list_by_offset.end() && offset + num_descriptors == next_block_it->first) {
        num_descriptors += next_block_it->second.Size;

        m_free_list_by_size.erase(next_block_it->second.FreeListBySizeIt);
        m_free_list_by_offset.erase(next_block_it);
    }

    AddNewBlock(offset, num_descriptors);
}

void DescriptorAllocatorPage::ReleaseStaleDescriptors() {
    std::lock_guard<std::mutex> lock(m_allocation_mutex);

    while (!m_stale_descriptors.empty()) {
        auto& stale_descriptor = m_stale_descriptors.front();
        auto offset = stale_descriptor.Offset;
        auto num_descriptors = stale_descriptor.Size;

        FreeBlock(offset, num_descriptors);

        m_stale_descriptors.pop();
    }
}