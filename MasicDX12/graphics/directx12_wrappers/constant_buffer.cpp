#include "constant_buffer.h"

#include "device.h"

#include <directx/d3dx12.h>

ConstantBuffer::ConstantBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource) : Buffer(device, resource) {
	m_size_in_bytes = GetD3D12ResourceDesc().Width;
}

ConstantBuffer::~ConstantBuffer() {}

size_t ConstantBuffer::GetSizeInBytes() const {
	return m_size_in_bytes;
}