#include "byte_address_buffer.h"

#include "device.h"

size_t ByteAddressBuffer::GetBufferSize() const {
    return m_buffer_size;
}

ByteAddressBuffer::ByteAddressBuffer(Device& device, const D3D12_RESOURCE_DESC& res_desc) : Buffer(device, res_desc) {}

ByteAddressBuffer::ByteAddressBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource) : Buffer(device, resource) {}