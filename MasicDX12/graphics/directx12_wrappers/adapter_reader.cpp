#include "adapter_reader.h"

#include "../tools/com_exception.h"

#include <dxgidebug.h>

AdapterReader::AdapterReader() : m_gpu_preference(DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE), m_use_warp(false) {}

AdapterReader::AdapterReader(DXGI_GPU_PREFERENCE gpu_preference) : m_gpu_preference(gpu_preference), m_use_warp(false) {}

AdapterReader::AdapterReader(bool use_warp) : m_gpu_preference(DXGI_GPU_PREFERENCE_UNSPECIFIED), m_use_warp(use_warp) {}

AdapterReader::AdapterReader(DXGI_GPU_PREFERENCE gpu_preference, bool use_warp) : m_gpu_preference(gpu_preference), m_use_warp(use_warp) {}

AdapterReader::~AdapterReader() {}

void AdapterReader::Initialize() {
    f_create_factory();
    f_fill_adapters();
}

bool AdapterReader::CheckTearingSupport() {
    BOOL allow_tearing = FALSE;
    HRESULT hr = m_dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
    if (FAILED(hr)) allow_tearing = FALSE;

    return allow_tearing == TRUE;
}

AdapterData::AdapterDataPtr AdapterReader::GetAdapter() {
    AdapterData::AdapterDataPtr dxgi_adapter;

    SIZE_T max_dedicated_video_memory = 0u;
    size_t sz = m_adapters.size();
    for (size_t i = 0u; i < sz; ++i) {
        AdapterData::AdapterDataPtr adapter_data = m_adapters[i];
        SIZE_T dedicated_video_memory = adapter_data->GetDedicatedVideoMemory();
        bool is_hw = adapter_data->GetHardwareSupport();
        bool is_gt_mem = dedicated_video_memory > max_dedicated_video_memory;
        bool is_dx12_cap = adapter_data->GetDX12Capability();
        if (is_hw && is_gt_mem && is_dx12_cap) {
            max_dedicated_video_memory = dedicated_video_memory;
            dxgi_adapter = adapter_data;
        }
    }

    return dxgi_adapter;
}

Microsoft::WRL::ComPtr<IDXGIFactory6> AdapterReader::GetDXGIFactory() {
    return m_dxgi_factory;
}

void AdapterReader::f_create_factory() {
    UINT create_factory_flags = 0u;
#if defined(_DEBUG)
    create_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    HRESULT hr = CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(m_dxgi_factory.GetAddressOf()));
    ThrowIfFailed(hr);
}

void AdapterReader::f_fill_adapters() {
    if (m_use_warp) {
        m_adapters.reserve(1u);
        Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter;
        HRESULT hr = m_dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(dxgi_adapter.GetAddressOf()));
        ThrowIfFailed(hr);
        m_adapters.push_back(std::make_shared<AdapterData>(dxgi_adapter));
        return;
    }
    m_adapters.reserve(f_get_adapters_count(m_gpu_preference));
    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter1;
    for (UINT i = 0u; m_dxgi_factory->EnumAdapterByGpuPreference(i, m_gpu_preference, IID_PPV_ARGS(dxgi_adapter1.ReleaseAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND; ++i) {
        m_adapters.push_back(std::make_shared<AdapterData>(dxgi_adapter1));
    }
}

UINT AdapterReader::f_get_adapters_count(DXGI_GPU_PREFERENCE gpu_preference) {
    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter1;
    UINT res = 0u;
    while (m_dxgi_factory->EnumAdapterByGpuPreference(res, gpu_preference, IID_PPV_ARGS(dxgi_adapter1.ReleaseAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND) {
        ++res;
    }
    return res;
}

AdapterData::AdapterData(Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter) {
    Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgi_adapter4;
    HRESULT hr = pAdapter.As(&dxgi_adapter4);
    ThrowIfFailed(hr);
    m_pAdapter = dxgi_adapter4;

    DXGI_ADAPTER_DESC1 dxgi_adapter_desc1;
    m_pAdapter->GetDesc1(&dxgi_adapter_desc1);

    m_description = dxgi_adapter_desc1;

    hr = D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr);
    m_is_dx12_cap = SUCCEEDED(hr);
    m_is_hw = (dxgi_adapter_desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
    m_dedicated_video_memory = dxgi_adapter_desc1.DedicatedVideoMemory;

    m_outputs.reserve(f_get_output_count());
    UINT index = 0u;
    Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;
    while (m_pAdapter->EnumOutputs(index++, pOutput.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
        m_outputs.push_back({ pOutput });
    }
}

AdapterData::~AdapterData() {}

UINT AdapterData::f_get_output_count() {
    UINT index = 0u;
    Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;
    while (m_pAdapter->EnumOutputs(index, pOutput.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
        ++index;
    }
    return index;
}

OutputData::OutputData(Microsoft::WRL::ComPtr<IDXGIOutput> pOutput) {
    m_pOutput = pOutput;
    HRESULT hr = pOutput->GetDesc(&m_description);
    ThrowIfFailed(hr);

    unsigned int numModes = 0;
    hr = m_pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    ThrowIfFailed(hr);

    m_modes.resize(numModes);
    hr = m_pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, m_modes.data());
    ThrowIfFailed(hr);
}

OutputData::~OutputData() {}

Microsoft::WRL::ComPtr<IDXGIAdapter4> AdapterData::GetDXGIAdapter() {
    return m_pAdapter;
}

const DXGI_ADAPTER_DESC1& AdapterData::GetDescription() const {
    return m_description;
}

bool AdapterData::GetDX12Capability() const {
    return m_is_dx12_cap;
}

bool AdapterData::GetHardwareSupport() const {
    return m_is_hw;
}

SIZE_T AdapterData::GetDedicatedVideoMemory() const {
    return m_dedicated_video_memory;
}

std::vector<OutputData>& AdapterData::GetOutputs() {
    return m_outputs;
}

const std::wstring AdapterData::GetTextDescription() const {
    return m_description.Description;
}
