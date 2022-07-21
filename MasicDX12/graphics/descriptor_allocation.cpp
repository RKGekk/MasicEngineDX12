#include "descriptor_allocation.h"
#include "descriptor_allocator_page.h"

#include <cassert>
#include <cstdint>

bool DescriptorAllocation::IsValid() const {
    return !IsNull();
}

DescriptorAllocation::DescriptorAllocation() : m_descriptor{ 0 }, m_num_handles(0u), m_descriptor_size(0u), m_page(nullptr) {}

DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t num_handles, uint32_t descriptor_size, std::shared_ptr<DescriptorAllocatorPage> page) : m_descriptor(descriptor), m_num_handles(num_handles), m_descriptor_size(descriptor_size), m_page(page) {}

DescriptorAllocation::~DescriptorAllocation() {
    Free();
}

DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation) noexcept : m_descriptor(allocation.m_descriptor), m_num_handles(allocation.m_num_handles), m_descriptor_size(allocation.m_descriptor_size), m_page(std::move(allocation.m_page)) {
    allocation.m_descriptor.ptr = 0u;
    allocation.m_num_handles = 0u;
    allocation.m_descriptor_size = 0u;
}

DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept {
    Free();

    m_descriptor = other.m_descriptor;
    m_num_handles = other.m_num_handles;
    m_descriptor_size = other.m_descriptor_size;
    m_page = std::move(other.m_page);

    other.m_descriptor.ptr = 0u;
    other.m_num_handles = 0u;
    other.m_descriptor_size = 0u;

    return *this;
}

void DescriptorAllocation::Free() {
    if (!IsNull() && m_page) {
        m_page->Free(std::move(*this));

        m_descriptor.ptr = 0u;
        m_num_handles = 0u;
        m_descriptor_size = 0u;
        m_page.reset();
    }
}

bool DescriptorAllocation::IsNull() const {
    return m_descriptor.ptr == 0u;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset) const {
    assert(offset < m_num_handles);
    SIZE_T ptr = m_descriptor.ptr + (m_descriptor_size * offset);
    D3D12_CPU_DESCRIPTOR_HANDLE hdl{};
    hdl.ptr = ptr;
    return hdl;
}

uint32_t DescriptorAllocation::GetNumHandles() const {
    return m_num_handles;
}

std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocation::GetDescriptorAllocatorPage() const {
    return m_page;
}