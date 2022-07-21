#include "constant_buffer_view.h"

#include "constant_buffer.h"
#include "device.h"
#include "../tools/math_utitity.h"

#include <cassert>

ConstantBufferView::ConstantBufferView(Device& device, const std::shared_ptr<ConstantBuffer>& constant_buffer, size_t offset) : m_device(device), m_constant_buffer(constant_buffer) {
    assert(constant_buffer);

    auto d3d12_device = m_device.GetD3D12Device();
    auto d3d12_resource = m_constant_buffer->GetD3D12Resource();

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv;
    cbv.BufferLocation = d3d12_resource->GetGPUVirtualAddress() + offset;
    cbv.SizeInBytes = static_cast<UINT>(Math::AlignUp(m_constant_buffer->GetSizeInBytes(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

    m_descriptor = device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    d3d12_device->CreateConstantBufferView(&cbv, m_descriptor.GetDescriptorHandle());
}

std::shared_ptr<ConstantBuffer> ConstantBufferView::GetConstantBuffer() const {
    return m_constant_buffer;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferView::GetDescriptorHandle() {
    return m_descriptor.GetDescriptorHandle();
}