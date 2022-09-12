#pragma once

#include <directx/d3dx12.h>
#include <wrl.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <queue>

class Device;
class CommandList;
class RootSignature;

class DynamicDescriptorHeap {
public:
	DynamicDescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint32_t num_descriptors_per_heap = 1024u);

	virtual ~DynamicDescriptorHeap();

	void StageDescriptors(uint32_t root_parameter_index, uint32_t offset, uint32_t num_descriptors, const D3D12_CPU_DESCRIPTOR_HANDLE src_descriptors);
	void StageInlineCBV(uint32_t root_parameter_index, D3D12_GPU_VIRTUAL_ADDRESS buffer_location);
	void StageInlineSRV(uint32_t root_parameter_index, D3D12_GPU_VIRTUAL_ADDRESS buffer_location);
	void StageInlineUAV(uint32_t root_paramter_index, D3D12_GPU_VIRTUAL_ADDRESS buffer_location);

	void CommitStagedDescriptorsForDraw(CommandList& command_list);
	void CommitStagedDescriptorsForDispatch(CommandList& command_list);

	D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(CommandList& comand_list, D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor);

	void ParseRootSignature(const std::shared_ptr<RootSignature>& root_signature);

	void Reset();

protected:
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

	uint32_t ComputeStaleDescriptorCount() const;

	void CommitDescriptorTables(CommandList& command_list, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> set_func);
	void CommitInlineDescriptors(CommandList& command_list, const D3D12_GPU_VIRTUAL_ADDRESS* buffer_locations, uint32_t& bit_mask, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)> set_func);

	static const uint32_t MAX_DESCRIPTOR_TABLES = 32u;

	struct DescriptorTableCache {
		DescriptorTableCache() : NumDescriptors(0u), BaseDescriptor(nullptr) {}

		void Reset() {
			NumDescriptors = 0u;
			BaseDescriptor = nullptr;
		}

		uint32_t NumDescriptors;
		D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor;
	};

	Device& m_device;

	D3D12_DESCRIPTOR_HEAP_TYPE m_descriptor_heap_type;

	uint32_t m_num_descriptors_per_heap;
	uint32_t m_descriptor_handle_increment_size;

	std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> m_descriptor_handle_cache;

	DescriptorTableCache m_descriptor_table_cache[MAX_DESCRIPTOR_TABLES];

	D3D12_GPU_VIRTUAL_ADDRESS m_inline_cbv[MAX_DESCRIPTOR_TABLES];
	D3D12_GPU_VIRTUAL_ADDRESS m_inline_srv[MAX_DESCRIPTOR_TABLES];
	D3D12_GPU_VIRTUAL_ADDRESS m_inline_uav[MAX_DESCRIPTOR_TABLES];

	uint32_t m_descriptor_table_bit_mask;

	uint32_t m_stale_descriptor_table_bit_mask;
	uint32_t m_stale_cbv_bit_mask;
	uint32_t m_stale_srv_bit_mask;
	uint32_t m_stale_uav_bit_mask;

	using DescriptorHeapPool = std::queue<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>;

	DescriptorHeapPool m_descriptor_heap_pool;
	DescriptorHeapPool m_available_descriptor_heaps;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_current_descriptor_heap;
	CD3DX12_GPU_DESCRIPTOR_HANDLE m_current_gpu_descriptor_handle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_current_cpu_descriptor_handle;

	uint32_t m_num_free_handles;
};