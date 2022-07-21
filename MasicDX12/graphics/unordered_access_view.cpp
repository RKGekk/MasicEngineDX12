#include "unordered_access_view.h"

#include "device.h"
#include "resource.h"

#include <cassert>

UnorderedAccessView::UnorderedAccessView(Device& device, const std::shared_ptr<Resource>& resource, const std::shared_ptr<Resource>& counter_resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav) : m_device(device), m_resource(resource), m_counter_resource(counter_resource) {
    assert(m_resource || uav);

    auto d3d12_device = m_device.GetD3D12Device();
    auto d3d12_resource = m_resource ? m_resource->GetD3D12Resource() : nullptr;
    auto d3d12_counter_resource = m_counter_resource ? m_counter_resource->GetD3D12Resource() : nullptr;

    if (m_resource) {
        auto d3d12_resource_desc = m_resource->GetD3D12ResourceDesc();

        assert((d3d12_resource_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0);
    }

    m_descriptor = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    d3d12_device->CreateUnorderedAccessView(d3d12_resource.Get(), d3d12_counter_resource.Get(), uav, m_descriptor.GetDescriptorHandle());
}

std::shared_ptr<Resource> UnorderedAccessView::GetResource() const {
    return m_resource;
}

std::shared_ptr<Resource> UnorderedAccessView::GetCounterResource() const {
    return m_counter_resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessView::GetDescriptorHandle() const {
    return m_descriptor.GetDescriptorHandle();
}