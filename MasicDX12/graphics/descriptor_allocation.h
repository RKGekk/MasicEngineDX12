#pragma once

#include <d3d12.h>

#include <cstdint>
#include <memory>


class DescriptorAllocatorPage;

class DescriptorAllocation {
public:
	DescriptorAllocation();
	DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t num_handles, uint32_t descriptor_size, std::shared_ptr<DescriptorAllocatorPage> page);
	~DescriptorAllocation();

	DescriptorAllocation(const DescriptorAllocation&) = delete;
	DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;

	DescriptorAllocation(DescriptorAllocation&& allocation) noexcept;
	DescriptorAllocation& operator=(DescriptorAllocation&& other) noexcept;

	bool IsNull() const;
	bool IsValid() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;
	uint32_t GetNumHandles() const;
	std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const;

private:
	void Free();

	D3D12_CPU_DESCRIPTOR_HANDLE m_descriptor;
	uint32_t m_num_handles;
	uint32_t m_descriptor_size;
	std::shared_ptr<DescriptorAllocatorPage> m_page;
};