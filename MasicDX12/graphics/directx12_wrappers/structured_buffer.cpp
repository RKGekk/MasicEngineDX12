#include "structured_buffer.h"

#include "device.h"
#include "resource_state_tracker.h"

#include <directx/d3dx12.h>

StructuredBuffer::StructuredBuffer(Device& device, size_t num_elements, size_t element_size) : Buffer(device, CD3DX12_RESOURCE_DESC::Buffer(num_elements* element_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)), m_num_elements(num_elements), m_element_size(element_size) {
    m_counter_buffer = m_device.CreateByteAddressBuffer(4);
}

StructuredBuffer::StructuredBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t num_elements, size_t element_size) : Buffer(device, resource), m_num_elements(num_elements), m_element_size(element_size) {
    m_counter_buffer = m_device.CreateByteAddressBuffer(4);
}

size_t StructuredBuffer::GetNumElements() const {
    return m_num_elements;
}

size_t StructuredBuffer::GetElementSize() const {
    return m_element_size;
}

std::shared_ptr<ByteAddressBuffer> StructuredBuffer::GetCounterBuffer() const {
    return m_counter_buffer;
}