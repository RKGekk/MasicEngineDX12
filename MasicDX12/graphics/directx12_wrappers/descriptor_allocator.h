#pragma once

#include "descriptor_allocation.h"

#include <directx/d3dx12.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

class DescriptorAllocatorPage;
class Device;

class DescriptorAllocator {
public:
	DescriptorAllocation Allocate(uint32_t num_descriptors = 1u);

	void ReleaseStaleDescriptors();

	DescriptorAllocator(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors_per_heap = 256u);
	virtual ~DescriptorAllocator();

private:
	using DescriptorHeapPool = std::vector<std::shared_ptr<DescriptorAllocatorPage>>;

	std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();

	Device& m_device;
	D3D12_DESCRIPTOR_HEAP_TYPE m_heap_type;
	uint32_t m_num_descriptors_per_heap;

	DescriptorHeapPool m_heap_pool;
	std::set<size_t> m_available_heaps;

	std::mutex m_allocation_mutex;
};