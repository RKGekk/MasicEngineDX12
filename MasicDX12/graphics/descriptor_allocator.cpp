#include "descriptor_allocator.h"

#include "descriptor_allocator_page.h"

#include <algorithm>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

struct MakeAllocatorPage : public DescriptorAllocatorPage {
public:
    MakeAllocatorPage(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors) : DescriptorAllocatorPage(device, type, num_descriptors) {}

    virtual ~MakeAllocatorPage() {}
};

DescriptorAllocator::DescriptorAllocator(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors_per_heap) : m_device(device), m_heap_type(type), m_num_descriptors_per_heap(num_descriptors_per_heap) {}

DescriptorAllocator::~DescriptorAllocator() {}

std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateAllocatorPage() {
    std::shared_ptr<DescriptorAllocatorPage> new_page = std::make_shared<MakeAllocatorPage>(m_device, m_heap_type, m_num_descriptors_per_heap);

    m_heap_pool.emplace_back(new_page);
    m_available_heaps.insert(m_heap_pool.size() - 1);

    return new_page;
}

DescriptorAllocation DescriptorAllocator::Allocate(uint32_t num_descriptors) {
    std::lock_guard<std::mutex> lock(m_allocation_mutex);

    DescriptorAllocation allocation;

    auto iter = m_available_heaps.begin();
    while (iter != m_available_heaps.end()) {
        auto allocator_page = m_heap_pool[*iter];

        allocation = allocator_page->Allocate(num_descriptors);

        if (allocator_page->NumFreeHandles() == 0) {
            iter = m_available_heaps.erase(iter);
        }
        else {
            ++iter;
        }

        if (!allocation.IsNull()) {
            break;
        }
    }

    if (allocation.IsNull()) {
        m_num_descriptors_per_heap = std::max(m_num_descriptors_per_heap, num_descriptors);
        auto newPage = CreateAllocatorPage();

        allocation = newPage->Allocate(num_descriptors);
    }

    return allocation;
}

void DescriptorAllocator::ReleaseStaleDescriptors() {
    std::lock_guard<std::mutex> lock(m_allocation_mutex);

    for (size_t i = 0; i < m_heap_pool.size(); ++i) {
        auto page = m_heap_pool[i];

        page->ReleaseStaleDescriptors();

        if (page->NumFreeHandles() > 0) {
            m_available_heaps.insert(i);
        }
    }
}