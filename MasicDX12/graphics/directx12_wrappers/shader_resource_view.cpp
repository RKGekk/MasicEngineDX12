#include "shader_resource_view.h"

#include "device.h"
#include "resource.h"

#include <cassert>

std::shared_ptr<Resource> ShaderResourceView::GetResource() const {
    return m_resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE ShaderResourceView::GetDescriptorHandle() const {
    return m_descriptor.GetDescriptorHandle();
}

ShaderResourceView::ShaderResourceView(Device& device, const std::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv) : m_device(device), m_resource(resource) {
    assert(resource || srv);

    auto d3d12_resource = m_resource ? m_resource->GetD3D12Resource() : nullptr;
    auto d3d12_device = m_device.GetD3D12Device();

    m_descriptor = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    d3d12_device->CreateShaderResourceView(d3d12_resource.Get(), srv, m_descriptor.GetDescriptorHandle());
}