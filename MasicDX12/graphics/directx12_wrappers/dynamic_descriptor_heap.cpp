#include "dynamic_descriptor_heap.h"

#include "command_list.h"
#include "device.h"
#include "root_signature.h"
#include "../tools/com_exception.h"

DynamicDescriptorHeap::DynamicDescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint32_t num_descriptors_per_heap) :
    m_device(device),
    m_descriptor_heap_type(heap_type),
    m_num_descriptors_per_heap(num_descriptors_per_heap),
    m_descriptor_table_bit_mask(0u),
    m_stale_descriptor_table_bit_mask(0u),
    m_stale_cbv_bit_mask(0u),
    m_stale_srv_bit_mask(0u),
    m_stale_uav_bit_mask(0u),
    m_current_cpu_descriptor_handle(D3D12_DEFAULT),
    m_current_gpu_descriptor_handle(D3D12_DEFAULT),
    m_num_free_handles(0u) {

    m_descriptor_handle_increment_size = m_device.GetDescriptorHandleIncrementSize(heap_type);
    m_descriptor_handle_cache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(m_num_descriptors_per_heap);
}

DynamicDescriptorHeap::~DynamicDescriptorHeap() {}

void DynamicDescriptorHeap::ParseRootSignature(const std::shared_ptr<RootSignature>& root_signature) {
    assert(root_signature);

    m_stale_descriptor_table_bit_mask = 0u;

    const auto& root_signature_desc = root_signature->GetRootSignatureDesc().Desc_1_1;

    m_descriptor_table_bit_mask = root_signature->GetDescriptorTableBitMask(m_descriptor_heap_type);
    uint32_t descriptor_table_bit_mask = m_descriptor_table_bit_mask;

    uint32_t current_offset = 0u;
    DWORD root_index;
    while (_BitScanForward(&root_index, descriptor_table_bit_mask) && root_index < root_signature_desc.NumParameters) {
        uint32_t num_descriptors = root_signature->GetNumDescriptors(root_index);

        DescriptorTableCache& descriptor_table_cache = m_descriptor_table_cache[root_index];
        descriptor_table_cache.NumDescriptors = num_descriptors;
        descriptor_table_cache.BaseDescriptor = m_descriptor_handle_cache.get() + current_offset;

        current_offset += num_descriptors;

        descriptor_table_bit_mask ^= (1 << root_index);
    }

    assert(current_offset <= m_num_descriptors_per_heap && "The root signature requires more than the maximum number of descriptors per descriptor heap. Consider increasing the maximum number of descriptors per descriptor heap.");
}

void DynamicDescriptorHeap::StageDescriptors(uint32_t root_parameter_index, uint32_t offset, uint32_t num_descriptors, const D3D12_CPU_DESCRIPTOR_HANDLE src_descriptor) {
    if (num_descriptors > m_num_descriptors_per_heap || root_parameter_index >= MAX_DESCRIPTOR_TABLES) {
        throw std::bad_alloc();
    }

    DescriptorTableCache& descriptor_table_cache = m_descriptor_table_cache[root_parameter_index];

    if ((offset + num_descriptors) > descriptor_table_cache.NumDescriptors) {
        throw std::length_error("Number of descriptors exceeds the number of descriptors in the descriptor table.");
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* dst_descriptor = (descriptor_table_cache.BaseDescriptor + offset);
    for (uint32_t i = 0; i < num_descriptors; ++i) {
        dst_descriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(src_descriptor, i, m_descriptor_handle_increment_size);
    }

    m_stale_descriptor_table_bit_mask |= (1 << root_parameter_index);
}

void DynamicDescriptorHeap::StageInlineCBV(uint32_t root_parameter_index, D3D12_GPU_VIRTUAL_ADDRESS buffer_location) {
    assert(root_parameter_index < MAX_DESCRIPTOR_TABLES);

    m_inline_cbv[root_parameter_index] = buffer_location;
    m_stale_cbv_bit_mask |= (1 << root_parameter_index);
}

void DynamicDescriptorHeap::StageInlineSRV(uint32_t root_parameter_index, D3D12_GPU_VIRTUAL_ADDRESS buffer_location) {
    assert(root_parameter_index < MAX_DESCRIPTOR_TABLES);

    m_inline_srv[root_parameter_index] = buffer_location;
    m_stale_srv_bit_mask |= (1 << root_parameter_index);
}

void DynamicDescriptorHeap::StageInlineUAV(uint32_t root_paramter_index, D3D12_GPU_VIRTUAL_ADDRESS buffer_location) {
    assert(root_paramter_index < MAX_DESCRIPTOR_TABLES);

    m_inline_uav[root_paramter_index] = buffer_location;
    m_stale_uav_bit_mask |= (1 << root_paramter_index);
}

uint32_t DynamicDescriptorHeap::ComputeStaleDescriptorCount() const {
    uint32_t num_stale_descriptors = 0u;
    DWORD i;
    DWORD stale_descriptors_bit_mask = m_stale_descriptor_table_bit_mask;

    while (_BitScanForward(&i, stale_descriptors_bit_mask)) {
        num_stale_descriptors += m_descriptor_table_cache[i].NumDescriptors;
        stale_descriptors_bit_mask ^= (1 << i);
    }

    return num_stale_descriptors;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::RequestDescriptorHeap() {
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor_heap;
    if (!m_available_descriptor_heaps.empty()) {
        descriptor_heap = m_available_descriptor_heaps.front();
        m_available_descriptor_heaps.pop();
    }
    else {
        descriptor_heap = CreateDescriptorHeap();
        m_descriptor_heap_pool.push(descriptor_heap);
    }

    return descriptor_heap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::CreateDescriptorHeap() {
    auto d3d12_device = m_device.GetD3D12Device();

    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {};
    descriptor_heap_desc.Type = m_descriptor_heap_type;
    descriptor_heap_desc.NumDescriptors = m_num_descriptors_per_heap;
    descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor_heap;
    HRESULT hr = d3d12_device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(descriptor_heap.GetAddressOf()));
    ThrowIfFailed(hr);

    return descriptor_heap;
}

void DynamicDescriptorHeap::CommitDescriptorTables(CommandList& command_list, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> set_func) {
    uint32_t num_descriptors_to_commit = ComputeStaleDescriptorCount();

    if (num_descriptors_to_commit > 0) {
        auto d3d12_device = m_device.GetD3D12Device();
        auto d3d12_graphics_command_list = command_list.GetD3D12CommandList().Get();
        assert(d3d12_graphics_command_list != nullptr);

        if (!m_current_descriptor_heap || m_num_free_handles < num_descriptors_to_commit) {
            m_current_descriptor_heap = RequestDescriptorHeap();
            m_current_cpu_descriptor_handle = m_current_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
            m_current_gpu_descriptor_handle = m_current_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
            m_num_free_handles = m_num_descriptors_per_heap;

            command_list.SetDescriptorHeap(m_descriptor_heap_type, m_current_descriptor_heap.Get());
            m_stale_descriptor_table_bit_mask = m_descriptor_table_bit_mask;
        }

        DWORD root_index;
        while (_BitScanForward(&root_index, m_stale_descriptor_table_bit_mask)) {
            UINT num_src_descriptors = m_descriptor_table_cache[root_index].NumDescriptors;
            D3D12_CPU_DESCRIPTOR_HANDLE* pSrc_descriptor_handles = m_descriptor_table_cache[root_index].BaseDescriptor;

            D3D12_CPU_DESCRIPTOR_HANDLE pDest_descriptor_range_starts[] = { m_current_cpu_descriptor_handle };
            UINT pDest_descriptor_range_sizes[] = { num_src_descriptors };

            d3d12_device->CopyDescriptors(1u, pDest_descriptor_range_starts, pDest_descriptor_range_sizes, num_src_descriptors, pSrc_descriptor_handles, nullptr, m_descriptor_heap_type);

            set_func(d3d12_graphics_command_list, root_index, m_current_gpu_descriptor_handle);

            m_current_cpu_descriptor_handle.Offset(num_src_descriptors, m_descriptor_handle_increment_size);
            m_current_gpu_descriptor_handle.Offset(num_src_descriptors, m_descriptor_handle_increment_size);
            m_num_free_handles -= num_src_descriptors;

            m_stale_descriptor_table_bit_mask ^= (1 << root_index);
        }
    }
}

void DynamicDescriptorHeap::CommitInlineDescriptors(CommandList& command_list, const D3D12_GPU_VIRTUAL_ADDRESS* buffer_locations, uint32_t& bit_mask, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)> set_func) {
    if (bit_mask != 0) {
        auto  d3d12_graphics_command_list = command_list.GetD3D12CommandList().Get();
        DWORD root_index;
        while (_BitScanForward(&root_index, bit_mask)) {
            set_func(d3d12_graphics_command_list, root_index, buffer_locations[root_index]);
            bit_mask ^= (1 << root_index);
        }
    }
}

void DynamicDescriptorHeap::CommitStagedDescriptorsForDraw(CommandList& command_list) {
    CommitDescriptorTables(command_list, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
    CommitInlineDescriptors(command_list, m_inline_cbv, m_stale_cbv_bit_mask, &ID3D12GraphicsCommandList::SetGraphicsRootConstantBufferView);
    CommitInlineDescriptors(command_list, m_inline_srv, m_stale_srv_bit_mask, &ID3D12GraphicsCommandList::SetGraphicsRootShaderResourceView);
    CommitInlineDescriptors(command_list, m_inline_uav, m_stale_uav_bit_mask, &ID3D12GraphicsCommandList::SetGraphicsRootUnorderedAccessView);
}

void DynamicDescriptorHeap::CommitStagedDescriptorsForDispatch(CommandList& command_list) {
    CommitDescriptorTables(command_list, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
    CommitInlineDescriptors(command_list, m_inline_cbv, m_stale_cbv_bit_mask, &ID3D12GraphicsCommandList::SetComputeRootConstantBufferView);
    CommitInlineDescriptors(command_list, m_inline_srv, m_stale_srv_bit_mask, &ID3D12GraphicsCommandList::SetComputeRootShaderResourceView);
    CommitInlineDescriptors(command_list, m_inline_uav, m_stale_uav_bit_mask, &ID3D12GraphicsCommandList::SetComputeRootUnorderedAccessView);
}

D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::CopyDescriptor(CommandList& comand_list, D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor) {
    if (!m_current_descriptor_heap || m_num_free_handles < 1) {
        m_current_descriptor_heap = RequestDescriptorHeap();
        m_current_cpu_descriptor_handle = m_current_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        m_current_gpu_descriptor_handle = m_current_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        m_num_free_handles = m_num_descriptors_per_heap;

        comand_list.SetDescriptorHeap(m_descriptor_heap_type, m_current_descriptor_heap.Get());

        m_stale_descriptor_table_bit_mask = m_descriptor_table_bit_mask;
    }

    auto d3d12_device = m_device.GetD3D12Device();

    D3D12_GPU_DESCRIPTOR_HANDLE hGPU = m_current_gpu_descriptor_handle;
    d3d12_device->CopyDescriptorsSimple(1u, m_current_cpu_descriptor_handle, cpu_descriptor, m_descriptor_heap_type);

    m_current_cpu_descriptor_handle.Offset(1, m_descriptor_handle_increment_size);
    m_current_gpu_descriptor_handle.Offset(1, m_descriptor_handle_increment_size);
    m_num_free_handles -= 1;

    return hGPU;
}

void DynamicDescriptorHeap::Reset() {
    m_available_descriptor_heaps = m_descriptor_heap_pool;
    m_current_descriptor_heap.Reset();
    m_current_cpu_descriptor_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    m_current_gpu_descriptor_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    m_num_free_handles = 0u;
    m_descriptor_table_bit_mask = 0u;
    m_stale_descriptor_table_bit_mask = 0u;
    m_stale_cbv_bit_mask = 0u;
    m_stale_srv_bit_mask = 0u;
    m_stale_uav_bit_mask = 0u;

    for (int i = 0; i < MAX_DESCRIPTOR_TABLES; ++i) {
        m_descriptor_table_cache[i].Reset();
        m_inline_cbv[i] = 0ull;
        m_inline_srv[i] = 0ull;
        m_inline_uav[i] = 0ull;
    }
}