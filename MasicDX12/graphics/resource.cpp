#include "resource.h"

#include "device.h"
#include "resource_state_tracker.h"
#include "../tools/com_exception.h"

Resource::Resource(Device& device, const D3D12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE* clear_value) : m_device(device) {
    auto d3d12Device = m_device.GetD3D12Device();

    if (clear_value) {
        m_d3d12_clear_value = std::make_unique<D3D12_CLEAR_VALUE>(*clear_value);
    }

    D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr = d3d12Device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COMMON, m_d3d12_clear_value.get(), IID_PPV_ARGS(m_d3d12_resource.GetAddressOf()));
    ThrowIfFailed(hr);

    ResourceStateTracker::AddGlobalResourceState(m_d3d12_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

    CheckFeatureSupport();
}

Resource::Resource(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clear_value) : m_device(device), m_d3d12_resource(resource) {
    if (clear_value) {
        m_d3d12_clear_value = std::make_unique<D3D12_CLEAR_VALUE>(*clear_value);
    }
    CheckFeatureSupport();
}

Device& Resource::GetDevice() const {
    return m_device;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Resource::GetD3D12Resource() const {
    return m_d3d12_resource;
}

D3D12_RESOURCE_DESC Resource::GetD3D12ResourceDesc() const {
    D3D12_RESOURCE_DESC res_desc = {};
    if (m_d3d12_resource) {
        res_desc = m_d3d12_resource->GetDesc();
    }

    return res_desc;
}

void Resource::SetName(const std::wstring& name) {
    m_resource_name = name;
    if (m_d3d12_resource && !m_resource_name.empty()) {
        m_d3d12_resource->SetName(m_resource_name.c_str());
    }
}

const std::wstring& Resource::GetName() const {
    return m_resource_name;
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 format_support) const {
    return (m_format_support.Support1 & format_support) != 0;
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const {
    return (m_format_support.Support2 & formatSupport) != 0;
}

void Resource::CheckFeatureSupport() {
    auto d3d12Device = m_device.GetD3D12Device();

    auto desc = m_d3d12_resource->GetDesc();
    m_format_support.Format = desc.Format;
    HRESULT hr = d3d12Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &m_format_support, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT));
    ThrowIfFailed(hr);
}