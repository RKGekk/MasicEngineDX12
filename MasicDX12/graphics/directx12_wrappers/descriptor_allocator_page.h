#pragma once

#include "descriptor_allocation.h"

#include <d3d12.h>
#include <wrl.h>
#include <directx/d3dx12.h>

#include <map>
#include <memory>
#include <mutex>
#include <queue>

class Device;

class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage> {
public:
	D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;
	bool HasSpace(uint32_t num_descriptors) const;
	uint32_t NumFreeHandles() const;

	DescriptorAllocation Allocate(uint32_t num_descriptors);
	void Free(DescriptorAllocation&& descriptor_handle);
	void ReleaseStaleDescriptors();

protected:
	DescriptorAllocatorPage(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors);
	virtual ~DescriptorAllocatorPage() = default;

	uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void AddNewBlock(uint32_t offset, uint32_t num_descriptors);
	void FreeBlock(uint32_t offset, uint32_t num_descriptors);

private:
	struct FreeBlockInfo;
	struct StaleDescriptorInfo;

	using OffsetType = uint32_t;
	using SizeType = uint32_t;
	using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;
	using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;
	using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

	struct FreeBlockInfo {
		FreeBlockInfo(SizeType size) : Size(size) {}

		SizeType Size;
		FreeListBySize::iterator FreeListBySizeIt;
	};

	struct StaleDescriptorInfo {
		StaleDescriptorInfo(OffsetType offset, SizeType size) : Offset(offset), Size(size) {}

		OffsetType Offset;
		SizeType Size;
	};

	Device& m_device;

	FreeListByOffset m_free_list_by_offset;
	FreeListBySize m_free_list_by_size;
	StaleDescriptorQueue m_stale_descriptors;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3d12_descriptor_heap;
	D3D12_DESCRIPTOR_HEAP_TYPE m_heap_type;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_base_descriptor;
	uint32_t m_descriptor_handle_increment_size;
	uint32_t m_num_descriptors_in_heap;
	uint32_t m_num_free_handles;

	std::mutex m_allocation_mutex;
};