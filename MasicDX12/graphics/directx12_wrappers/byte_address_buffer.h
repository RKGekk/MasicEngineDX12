#pragma once

#include "buffer.h"
#include "descriptor_allocation.h"

#include <directx/d3dx12.h>

class Device;

class ByteAddressBuffer : public Buffer {
public:
	size_t GetBufferSize() const;

	ByteAddressBuffer(Device& device, const D3D12_RESOURCE_DESC& resDesc);
	ByteAddressBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource);
	virtual ~ByteAddressBuffer() = default;

private:
	size_t m_buffer_size;
};