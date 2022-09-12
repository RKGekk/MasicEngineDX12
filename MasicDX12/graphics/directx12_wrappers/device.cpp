#include "device.h"

#include "adapter_reader.h"
#include "byte_address_buffer.h"
#include "command_list.h"
#include "command_queue.h"
#include "constant_buffer.h"
#include "constant_buffer_view.h"
#include "descriptor_allocator.h"
#include "../gui.h"
#include "index_buffer.h"
#include "pipeline_state_object.h"
#include "resource_state_tracker.h"
#include "root_signature.h"
#include "shader.h"
#include "shader_resource_view.h"
#include "structured_buffer.h"
#include "swap_chain.h"
#include "texture.h"
#include "unordered_access_view.h"
#include "vertex_buffer.h"
#include "../tools/com_exception.h"
#include "../tools/math_utitity.h"

#include <dxgidebug.h>

#pragma region Class adapters for std::make_shared

class MakeGUI : public GUI {
public:
    MakeGUI(Device& device, HWND hWnd, const RenderTarget& render_target) : GUI(device, hWnd, render_target) {}

    virtual ~MakeGUI() {}
};

class MakeUnorderedAccessView : public UnorderedAccessView {
public:
    MakeUnorderedAccessView(Device& device, const std::shared_ptr<Resource>& resource, const std::shared_ptr<Resource>& counter_resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav) : UnorderedAccessView(device, resource, counter_resource, uav) {}

    virtual ~MakeUnorderedAccessView() {}
};

class MakeShaderResourceView : public ShaderResourceView {
public:
    MakeShaderResourceView(Device& device, const std::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv) : ShaderResourceView(device, resource, srv) {}

    virtual ~MakeShaderResourceView() {}
};

class MakeConstantBufferView : public ConstantBufferView {
public:
    MakeConstantBufferView(Device& device, const std::shared_ptr<ConstantBuffer>& constant_buffer, size_t offset) : ConstantBufferView(device, constant_buffer, offset) {}

    virtual ~MakeConstantBufferView() {}
};

class MakeTexture : public Texture {
public:
    MakeTexture(Device& device, const D3D12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE* clear_value) : Texture(device, resource_desc, clear_value) {}
    MakeTexture(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clear_value) : Texture(device, resource, clear_value) {}

    virtual ~MakeTexture() {}
};

class MakeStructuredBuffer : public StructuredBuffer {
public:
    MakeStructuredBuffer(Device& device, size_t num_elements, size_t element_size) : StructuredBuffer(device, num_elements, element_size) {}
    MakeStructuredBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_elements, size_t element_size) : StructuredBuffer(device, resource, num_elements, element_size) {}

    virtual ~MakeStructuredBuffer() {}
};

class MakeVertexBuffer : public VertexBuffer {
public:
    MakeVertexBuffer(Device& device, size_t num_vertices, size_t vertex_stride) : VertexBuffer(device, num_vertices, vertex_stride) {}
    MakeVertexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_vertices, size_t vertex_stride) : VertexBuffer(device, resource, num_vertices, vertex_stride) {}

    virtual ~MakeVertexBuffer() {}
};

class MakeIndexBuffer : public IndexBuffer {
public:
    MakeIndexBuffer(Device& device, size_t num_indices, DXGI_FORMAT index_format) : IndexBuffer(device, num_indices, index_format) {}
    MakeIndexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_indices, DXGI_FORMAT index_format) : IndexBuffer(device, resource, num_indices, index_format) {}

    virtual ~MakeIndexBuffer() = default;
};

class MakeConstantBuffer : public ConstantBuffer {
public:
    MakeConstantBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource) : ConstantBuffer(device, resource) {}

    virtual ~MakeConstantBuffer() = default;
};

class MakeByteAddressBuffer : public ByteAddressBuffer {
public:
    MakeByteAddressBuffer(Device& device, const D3D12_RESOURCE_DESC& desc) : ByteAddressBuffer(device, desc) {}
    MakeByteAddressBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource) : ByteAddressBuffer(device, resource) {}

    virtual ~MakeByteAddressBuffer() = default;
};

class MakeDescriptorAllocator : public DescriptorAllocator {
public:
    MakeDescriptorAllocator(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors_per_heap = 256) : DescriptorAllocator(device, type, num_descriptors_per_heap) {}

    virtual ~MakeDescriptorAllocator() {}
};

class MakeSwapChain : public SwapChain {
public:
    MakeSwapChain(Device& device, HWND hWnd, DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM) : SwapChain(device, hWnd, back_buffer_format) {}

    virtual ~MakeSwapChain() {}
};

class MakeCommandQueue : public CommandQueue {
public:
    MakeCommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type) : CommandQueue(device, type) {}

    virtual ~MakeCommandQueue() {}
};

class MakeDevice : public Device {
public:
    MakeDevice(std::shared_ptr<AdapterData> adapter) : Device(adapter) {}

    virtual ~MakeDevice() {}
};
#pragma endregion

void Device::EnableDebugLayer() {
    Microsoft::WRL::ComPtr<ID3D12Debug1> debug_interface;
    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug_interface.GetAddressOf()));
    ThrowIfFailed(hr);
    debug_interface->EnableDebugLayer();
}

void Device::ReportLiveObjects() {

    IDXGIDebug1* dxgi_debug;
    DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgi_debug));

    dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
    dxgi_debug->Release();
}

std::shared_ptr<Device> Device::Create(std::shared_ptr<AdapterData> adapter) {
    return std::make_shared<MakeDevice>(adapter);
}

std::wstring Device::GetDescription() const {
    return m_adapter->GetTextDescription();
}

Device::Device(std::shared_ptr<AdapterData> adapter) : m_adapter(adapter) {
    auto dxgi_adapter = m_adapter->GetDXGIAdapter();

    HRESULT hr = D3D12CreateDevice(dxgi_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_d3d12_device.GetAddressOf()));
    ThrowIfFailed(hr);

    {
        Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfo_queue;
        if (SUCCEEDED(m_d3d12_device.As(&pInfo_queue))) {
            pInfo_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            pInfo_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            pInfo_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

            D3D12_MESSAGE_SEVERITY Severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;

            HRESULT hr = pInfo_queue->PushStorageFilter(&NewFilter);
            ThrowIfFailed(hr);
        }
    }

    m_direct_command_queue = std::make_unique<MakeCommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
    m_compute_command_queue = std::make_unique<MakeCommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
    m_copy_command_queue = std::make_unique<MakeCommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
        m_descriptor_allocators[i] = std::make_unique<MakeDescriptorAllocator>(*this, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
    }

    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data;
        feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        HRESULT hr = m_d3d12_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE));
        if (FAILED(hr)) {
            feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
        m_highest_root_signature_version = feature_data.HighestVersion;
    }

    m_heap_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    m_minimum_heap_size = m_heap_alignment;

    D3D12_FEATURE_DATA_D3D12_OPTIONS featureSupport{};
    hr = m_d3d12_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureSupport, sizeof(featureSupport));
    ThrowIfFailed(hr);

    switch (featureSupport.ResourceHeapTier) {
        case D3D12_RESOURCE_HEAP_TIER_1: m_supports_universal_heaps = false; break;
        default: m_supports_universal_heaps = true; break;
    }
}

Device::~Device() {}

CommandQueue& Device::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) {
    CommandQueue* command_queue;
    switch (type) {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            command_queue = m_direct_command_queue.get();
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            command_queue = m_compute_command_queue.get();
            break;
        case D3D12_COMMAND_LIST_TYPE_COPY:
            command_queue = m_copy_command_queue.get();
            break;
        default:
            command_queue = nullptr;
    }

    return *command_queue;
}

Microsoft::WRL::ComPtr<ID3D12Device5> Device::GetD3D12Device() const {
    return m_d3d12_device;
}

D3D_ROOT_SIGNATURE_VERSION Device::GetHighestRootSignatureVersion() const {
    return m_highest_root_signature_version;
}

void Device::Flush() {
    m_direct_command_queue->Flush();
    m_compute_command_queue->Flush();
    m_copy_command_queue->Flush();
}

DescriptorAllocation Device::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors) {
    return m_descriptor_allocators[type]->Allocate(num_descriptors);
}

UINT Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const {
    return m_d3d12_device->GetDescriptorHandleIncrementSize(type);
}

void Device::ReleaseStaleDescriptors() {
    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
        m_descriptor_allocators[i]->ReleaseStaleDescriptors();
    }
}

std::shared_ptr<AdapterData> Device::GetAdapter() const {
    return m_adapter;
}

std::shared_ptr<SwapChain> Device::CreateSwapChain(HWND hWnd, DXGI_FORMAT back_buffer_format) {
    std::shared_ptr<SwapChain> swap_chain;
    swap_chain = std::make_shared<MakeSwapChain>(*this, hWnd, back_buffer_format);

    return swap_chain;
}

std::shared_ptr<GUI> Device::CreateGUI(HWND hWnd, const RenderTarget& render_target) {
    std::shared_ptr<GUI> gui = std::make_shared<MakeGUI>(*this, hWnd, render_target);
    return gui;
}

std::shared_ptr<ConstantBuffer> Device::CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
    std::shared_ptr<ConstantBuffer> constant_buffer = std::make_shared<MakeConstantBuffer>(*this, resource);
    return constant_buffer;
}

std::shared_ptr<ByteAddressBuffer> Device::CreateByteAddressBuffer(size_t buffer_size) {
    buffer_size = Math::AlignUp(buffer_size, 4);
    std::shared_ptr<ByteAddressBuffer> buffer = std::make_shared<MakeByteAddressBuffer>(*this, CD3DX12_RESOURCE_DESC::Buffer(buffer_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
    return buffer;
}

std::shared_ptr<ByteAddressBuffer> Device::CreateByteAddressBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
    std::shared_ptr<ByteAddressBuffer> buffer = std::make_shared<MakeByteAddressBuffer>(*this, resource);
    return buffer;
}

std::shared_ptr<StructuredBuffer> Device::CreateStructuredBuffer(size_t num_elements, size_t element_size) {
    std::shared_ptr<StructuredBuffer> structured_buffer = std::make_shared<MakeStructuredBuffer>(*this, num_elements, element_size);
    return structured_buffer;
}

std::shared_ptr<StructuredBuffer> Device::CreateStructuredBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_elements, size_t element_size) {
    std::shared_ptr<StructuredBuffer> structured_buffer = std::make_shared<MakeStructuredBuffer>(*this, resource, num_elements, element_size);
    return structured_buffer;
}

std::shared_ptr<IndexBuffer> Device::CreateIndexBuffer(size_t num_indices, DXGI_FORMAT index_format) {
    std::shared_ptr<IndexBuffer> index_buffer = std::make_shared<MakeIndexBuffer>(*this, num_indices, index_format);
    return index_buffer;
}

std::shared_ptr<IndexBuffer> Device::CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_indices, DXGI_FORMAT index_format) {
    std::shared_ptr<IndexBuffer> index_buffer = std::make_shared<MakeIndexBuffer>(*this, resource, num_indices, index_format);
    return index_buffer;
}

std::shared_ptr<VertexBuffer> Device::CreateVertexBuffer(size_t num_vertices, size_t vertex_stride) {
    std::shared_ptr<VertexBuffer> vertex_buffer = std::make_shared<MakeVertexBuffer>(*this, num_vertices, vertex_stride);
    return vertex_buffer;
}

std::shared_ptr<VertexBuffer> Device::CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_vertices, size_t vertex_stride) {
    std::shared_ptr<VertexBuffer> vertex_buffer = std::make_shared<MakeVertexBuffer>(*this, resource, num_vertices, vertex_stride);
    return vertex_buffer;
}

std::shared_ptr<Texture> Device::CreateTexture(const D3D12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE* clear_value) {
    std::shared_ptr<Texture> texture = std::make_shared<MakeTexture>(*this, resource_desc, clear_value);
    return texture;
}

std::shared_ptr<Texture> Device::CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clear_value) {
    std::shared_ptr<Texture> texture = std::make_shared<MakeTexture>(*this, resource, clear_value);
    return texture;
}

std::shared_ptr<RootSignature> Device::CreateRootSignature(const std::string& name, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& root_signature_desc) {
    std::shared_ptr<RootSignature> root_signature = std::make_shared<RootSignature>(*this, name, root_signature_desc);
    return root_signature;
}

std::shared_ptr<GraphicsPipelineState> Device::CreateGraphicsPipelineState(const std::string& name, std::shared_ptr<RootSignature> root_signature, std::shared_ptr<VertexShader> vertex_shader, std::shared_ptr<PixelShader> pixel_shader, std::shared_ptr<Shader> domain_shader, std::shared_ptr<Shader> hull_shader, std::shared_ptr<Shader> geometry_shader) {
    std::shared_ptr<GraphicsPipelineState> pso = std::make_shared<GraphicsPipelineState>(*this, name, root_signature);
    pso->SetVertexShader(vertex_shader);
    pso->SetPixelShader(pixel_shader);
    pso->SetDomainShader(domain_shader);
    pso->SetHullShader(hull_shader);
    pso->SetGeometryShader(geometry_shader);
    return pso;
}

std::shared_ptr<ComputePipelineState> Device::CreateComputePipelineState(const std::string& name, std::shared_ptr<RootSignature> root_signature, std::shared_ptr<Shader> compute_shader) {
    std::shared_ptr<ComputePipelineState> pso = std::make_shared<ComputePipelineState>(*this, name, root_signature);
    pso->SetComputeShader(compute_shader);
    return pso;
}

std::shared_ptr<ConstantBufferView> Device::CreateConstantBufferView(const std::shared_ptr<ConstantBuffer>& constant_buffer, size_t offset) {
    std::shared_ptr<ConstantBufferView> constant_buffer_view = std::make_shared<MakeConstantBufferView>(*this, constant_buffer, offset);
    return constant_buffer_view;
}

std::shared_ptr<ShaderResourceView> Device::CreateShaderResourceView(const std::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv) {
    std::shared_ptr<ShaderResourceView> shader_resource_view = std::make_shared<MakeShaderResourceView>(*this, resource, srv);
    return shader_resource_view;
}

std::shared_ptr<UnorderedAccessView> Device::CreateUnorderedAccessView(const std::shared_ptr<Resource>& resource, const std::shared_ptr<Resource>& counter_resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav) {
    std::shared_ptr<UnorderedAccessView> unorderedAccessView = std::make_shared<MakeUnorderedAccessView>(*this, resource, counter_resource, uav);
    return unorderedAccessView;
}

DXGI_SAMPLE_DESC Device::GetMultisampleQualityLevels(DXGI_FORMAT format, UINT num_samples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags) const {
    DXGI_SAMPLE_DESC sample_desc = { 1, 0 };

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS quality_levels;
    quality_levels.Format = format;
    quality_levels.SampleCount = 1;
    quality_levels.Flags = flags;
    quality_levels.NumQualityLevels = 0;

    bool loop = true;

    while (loop) {
        HRESULT hr = m_d3d12_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &quality_levels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));

        bool samp_count = quality_levels.SampleCount <= num_samples;
        bool feature_supported = SUCCEEDED(hr);
        bool num_quality = quality_levels.NumQualityLevels > 0;
        loop = samp_count && feature_supported && num_quality;

        if (!loop) break;

        sample_desc.Count = quality_levels.SampleCount;
        sample_desc.Quality = quality_levels.NumQualityLevels - 1;
        quality_levels.SampleCount *= 2;
    }

    return sample_desc;
}

bool Device::SupportsUniversalHeaps() const {
    return m_supports_universal_heaps;
}

uint64_t Device::MinimumHeapSize() const {
    return m_minimum_heap_size;
}

uint64_t Device::MandatoryHeapAlignment() const {
    return m_heap_alignment;
}

uint64_t Device::NodeMask() const {
    return m_node_mask;
}
